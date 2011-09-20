/*
Copyright 2011 Peter Thoman, Klaus Kofler and contributors.
(for a full list check the AUTHORS file)

This file is part of uCLbench.
uCLbench is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

uCLbench is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with uCLbench.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *  uCLbench OpenCL Dynamic Branching Penalty Benchmark
 *
 *  Authors: Peter Thoman
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <CL/cl.h>

#include "oclutils.h"
#include "clcommon.h"
#include "clinfo.h"

// defines
#define BM_NAME "branch_penalty"
#define BM_VERSION "1.0"

#define BPSIZE 128
#define WARMUP_CYCLES 3

int main(int argc, char** argv)
{
	// Parse command line options
	int info_flag = 0, csv_flag = 0, usage_flag = 0;
	int device_id = -1;
	unsigned repeats = 1;
	int c = 0;
	while(c != -1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"info", no_argument,       &info_flag, 1},
			{"help", no_argument,       &usage_flag, 1},
			{"csv",  no_argument,       &csv_flag, 1},
			/* These options don't set a flag.
			   We distinguish them by their indices. */
			{"device",  optional_argument, 0, 'a'},
			{"repeats",  optional_argument, 0, 'b'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		c = getopt_long(argc, argv, "a:", long_options, &option_index);
		
		switch(c) {
		case 0:
			/* This option set a flag, do nothing else */
			break;
		case 'a':
			device_id = atoi(optarg);
			break;
		case 'b':
			repeats = atoi(optarg);
			break;
		case '?':
			/* getopt_long already printed an error message. */
			break;
		default:
			;
		}
	}
 
	// retrieve devices
	cl_int errcode = 0;
    unsigned int num_devices = 0, num_platforms = 0;
    cl_platform_id* platforms = get_all_platforms(&num_platforms, &errcode);
    cl_device_id* clDevices = get_all_devices(platforms, num_platforms, &num_devices);
    free(platforms);

	if(clDevices == NULL) {
		fprintf(stderr, "Error: Failed to create a device group!\n");
		return 1;
	}

	if (info_flag) {
		print_device_list(clDevices, num_devices);
		return 0;
	}

	// run benchmark for each device
	float branchSelector[BPSIZE];
	float results[BPSIZE];

	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: num_branches:unsigned, branch_width:unsigned, exec_time:double, rel_exec_time:double\n");
	}
 
	for(int i = 0; i < (int)num_devices; ++i)
	{
		if(device_id != -1 && device_id != i) continue;
		if(!csv_flag) print_short_device_info(clDevices[i], i);
		else print_csv_device_info(clDevices[i]);

		cl_context clCPUContext = clCreateContext(0, 1, &clDevices[i], NULL, NULL, &errcode);
		// Create a command-queue
		cl_command_queue clCommandQueue = clCreateCommandQueue(clCPUContext, clDevices[i], CL_QUEUE_PROFILING_ENABLE, &errcode);
		shrCheckError(errcode, CL_SUCCESS);

		// Setup device memory
		cl_mem d_branchSelector = clCreateBuffer(clCPUContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			BPSIZE*4, branchSelector, &errcode);
		cl_mem d_results = clCreateBuffer(clCPUContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
			BPSIZE*4, results, &errcode);
		shrCheckError(errcode, CL_SUCCESS);

		// Load and build OpenCL kernel
		size_t kernelLength;
		char buf[64];
		sprintf(buf, "#define localRange %d\n", BPSIZE);

		char *clBranchPenalty = oclLoadProgSource("bp_kernel.cl", buf, &kernelLength);
		shrCheckError(clBranchPenalty != NULL, shrTRUE);

		cl_program clProgram = clCreateProgramWithSource(clCPUContext, 1, (const char **)&clBranchPenalty, 
			&kernelLength, &errcode);
		shrCheckError(errcode, CL_SUCCESS);

		errcode = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
		shrCheckError(errcode, CL_SUCCESS);
		if(errcode != CL_SUCCESS)
		{
			char buffer[8192];
			errcode = clGetProgramBuildInfo(clProgram, clDevices[0], CL_PROGRAM_BUILD_LOG, 8192, buffer, NULL);
			if(errcode != CL_SUCCESS) printf("Cannot get build info: %s\n", oclErrorString(errcode));
			else printf("Build log:\n%s\n", buffer);
			return -1;
		}

		cl_kernel clKernel = clCreateKernel(clProgram, "branchPenalty", &errcode);
		shrCheckError(errcode, CL_SUCCESS);

		// Launch OpenCL kernel
		size_t localWorkSize[2], globalWorkSize[2];

		errcode = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *)&d_branchSelector);
		errcode |= clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *)&d_results);
		shrCheckError(errcode, CL_SUCCESS);

		localWorkSize[0] = BPSIZE;
		localWorkSize[1] = 1;
		globalWorkSize[0] = BPSIZE;
		globalWorkSize[1] = 1;

		// warmup
		for(int k=0; k<WARMUP_CYCLES; ++k)
			clEnqueueNDRangeKernel(clCommandQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		cl_event event;	

		cl_ulong normtime = 0;
		for(int branchtypes = 1; branchtypes <= BPSIZE; ++branchtypes) {
			for(int branchsize = 1; branchsize <= (BPSIZE/branchtypes); ++branchsize) {
				int curbranch = 0;
				//printf("branchtypes: % 3d, branchsize: % 3d\n", branchtypes, branchsize);
				for(int i = 0; i<BPSIZE; ++i) {
					if(i%branchsize == 0) curbranch++;
					if(curbranch%(branchtypes+1) == 0) curbranch = 1;
					branchSelector[i] = (float)curbranch;
					//printf("% 4d", curbranch);
				} //printf("\n");

				errcode = clEnqueueWriteBuffer(clCommandQueue, d_branchSelector, CL_TRUE, 0, BPSIZE*4, branchSelector, 0, NULL, NULL);
				shrCheckError(errcode, CL_SUCCESS);

				// run benchmark kernel
				errcode = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &event);
				shrCheckError(errcode, CL_SUCCESS);

				errcode = clFinish(clCommandQueue);
				errcode |= clWaitForEvents(1, &event);
				shrCheckError(errcode, CL_SUCCESS);
				cl_ulong time_start, time_end, time_taken;
				errcode = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
				errcode |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
				shrCheckError(errcode, CL_SUCCESS);
				time_taken = (time_end - time_start);
				if(branchtypes == 1 && branchsize == 1) normtime = time_taken;
				if(!csv_flag) printf("%ld, ", time_taken);
				else {
					printf("% 8u, % 8u, % 12.2lf, % 8.2lf\n", branchtypes, branchsize, time_taken/1000.0f, time_taken/((double)normtime));
				}
			}
			if(!csv_flag) printf("\n");
		}

		// Retrieve result from device
		errcode = clEnqueueReadBuffer(clCommandQueue, d_results, CL_TRUE, 0, BPSIZE*4, results, 0, NULL, NULL);
		shrCheckError(errcode, CL_SUCCESS);

		// print out the results
		//printf("\nResults:\n");
		//for(int i = 0; i < BPSIZE; i++) {
		//	printf("% 4.2f ", results[i]);
		//} printf("\n");

		// clean up memory
		clReleaseMemObject(d_branchSelector);
		clReleaseMemObject(d_results);
		free(clBranchPenalty);
		clReleaseContext(clCPUContext);
		clReleaseKernel(clKernel);
		clReleaseProgram(clProgram);
		clReleaseCommandQueue(clCommandQueue);
	}
 
    free(clDevices);
}
