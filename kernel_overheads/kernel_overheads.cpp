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
 *  uCLbench OpenCL Kernel Compilation and Invocation Time Benchmark
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "clcommon.h"
#include "clinfo.h"

// defines
#define BM_NAME "kernel_overheads"
#define BM_VERSION "1.0"

// 16MiB maximum kernel source
#define MAX_KERNEL_SIZE (16 * 1024 * 1024)

struct _test_case {
  int kernel_args;
  int kernel_loc;
	int done;
};

static struct _test_case ki_tests[] =
{
#include "testdata_loc.h"
};

cl_program gen_kernel(int n_args, int n_lines, cl_context context)
{
	char *kernel_src = (char *)malloc(MAX_KERNEL_SIZE);
	int n = 0;
	int pos = 0;
	cl_program program;
	int err;

	if(n_args < 1 || n_lines < 1)
		return NULL;
	
	pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "%s", "__kernel void main_func(\n\t");
	for(n = 0; n < n_args - 1; n++){
		pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "__global float* arg_%i, ", n);
	}
	if(n_args)
		pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "__global float* arg_%i\n)\n", n);

	pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "%s", "{\n");
	
	pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "\t%s", "float tmp_0, tmp_1, tmp_2;\n\n");

	srand((unsigned)time(0)); // required to prevent binary caching

	for(n = 0; n < n_lines; n = n + 2){
		pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "\targ_%i[0] = arg_%i[0] * %i;\n",
			(int) (n_args * (rand()/(RAND_MAX + 1.0))), (int) (n_args * (rand()/(RAND_MAX + 1.0))), 
			(int) (100000 * (rand()/(RAND_MAX + 1.0))));
		pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "\ttmp_%i = arg_%i[0] * %i;\n", 
			(int) (2 * (rand()/(RAND_MAX + 1.0))), (int) (n_args * (rand()/(RAND_MAX + 1.0))), 
			(int) (100000 * (rand()/(RAND_MAX + 1.0))));
		if(n % 3 == 0 && n < n_lines - 2)
			pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "\tif(tmp_%i > arg_%i[0])\n\t", 
				(int) (2 * (rand()/(RAND_MAX + 1.0))), (int) (n_args * (rand()/(RAND_MAX + 1.0))));
	}

	pos += snprintf(kernel_src + pos, MAX_KERNEL_SIZE - pos, "%s", "}\n");

	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_src, NULL, &err);
	if(!program){
		fprintf(stderr, "clCreateProgramWithSource() failed!\n");
		free(kernel_src);
		return NULL;
	}
//	printf("%s\n", kernel_src);
	free(kernel_src);

	return program;
}

int dblcmp(const void *p1, const void *p2)
{
	if(*(double *)p1 > *(double *)p2)
		return -1;
	else if(*(double *)p1 < *(double *)p2)
		return 1;
	else
		return 0;
}

template<typename T>
int run_kernel_benchmark(cl_device_id did, cl_context context, cl_command_queue commands, int n_args, int n_lines, double *duration, double *delta, double *compile_time)
{
	int i;
	int err;
	char build_log[4096] = {0};
	T *tmp_args = NULL;
	cl_mem* mem_args = NULL;
	double durations[10];
	size_t len;

	cl_ulong t1;
	cl_ulong t2;
	cl_ulong t3;
	cl_ulong t4;

	size_t global = 1;
	size_t local = global;

	cl_program program;
	cl_kernel kernel;
	cl_event event;

	//printf("lines: %i, args %i\n", n_args, n_lines);
	
	program = gen_kernel(n_args, n_lines, context);
	if(!program)
		return -1;

    unsigned long start_time = current_msecs();
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(err != CL_SUCCESS){
		fprintf(stderr, "clBuildProgram() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		clGetProgramBuildInfo(program, did, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &len);
		puts(build_log);
		return -1;
	}

/*
	size_t pz = 0;
	clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &pz, NULL);
	printf("*Progsize: %i\n", pz);	
*/

	kernel = clCreateKernel(program, "main_func", &err);
	if(!kernel || err != CL_SUCCESS){
		fprintf(stderr, "clCreateKernel() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		return -1;
	}
	*compile_time = elapsed_msecs(start_time)*1.0;

	err = 0;
	tmp_args = (T *)malloc(n_args * sizeof(T));
	mem_args = (cl_mem*)malloc(n_args * sizeof(cl_mem));
	for(i = 0; i < n_args; i++){
		tmp_args[i] = (float)(1 + (int) (100.0 * (rand() / (RAND_MAX + 1.0))));
		mem_args[i] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(T), tmp_args+i, NULL);
		err |= clSetKernelArg(kernel, i, sizeof(cl_mem), mem_args+i);	
	}

	if(err != CL_SUCCESS){
		fprintf(stderr, "clSetKernelArg() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		return -1;
	}
	
	//warm up call
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		fprintf(stderr, "err: %i\n", err);
		fprintf(stderr, "clEnqueueNDRangeKernel() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		return -1;
	}

	clFinish(commands);
	for(i = 0; i < 10; i++){	
		err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, &event);
		if(err != CL_SUCCESS){
			fprintf(stderr, "err: %i\n", err);
			fprintf(stderr, "clEnqueueNDRangeKernel() failed!\n");
			fprintf(stderr, "err: %i\n", err);
			return -1;
		}
			
		cl_int errcode = clFinish(commands);
		errcode |= clWaitForEvents(1, &event);
		if(errcode != CL_SUCCESS) printf("Error waiting for kernel completion: %s\n", oclErrorString(errcode));
	
		clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &t1, NULL);
		clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &t2, NULL);
		clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &t3, NULL);
		clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &t4, NULL);

		durations[i] =  (t3 - t1) * 1e-6;
	}

/*
	printf("submit: %lu\n", (unsigned long)t2 - (unsigned long)t1);
	printf("start: %lu\n", (unsigned long)t3 - (unsigned long)t1);
	printf("end: %lu\n", (unsigned long)t4 - (unsigned long)t1);
*/

	qsort(durations, 10, sizeof(double), dblcmp);
	*duration = durations[4];
	*delta = durations[0] - durations[9];

	clReleaseEvent(event);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	free(tmp_args);

	return 0;
}

void show_help(const char *me)
{
	fprintf(stderr, "%s: usage --device N\n", me);

	exit(1);
}

int main(int argc, char **argv)
{
	int csv_flag = 0;
	unsigned int num_devices = 0, num_platforms = 0;
	cl_device_id *devices;
	int i = 0, j = 0;
	int err;
	double duration;
	double delta;

	int ret;
	int device_id;
	int plattform_id;

	static struct option long_options[] = 
	{
		{"csv",  no_argument,       &csv_flag, 1},
		{"device",  required_argument, 0, 'a'},
		{0, 0, 0, 0}
	};

	if(argc < 3)
		show_help(argv[0]);

	while(1){
		int option_index = 0;
		int c = getopt_long(argc, argv, "", long_options, &option_index);	

		if(c == -1)
			break;

		switch(c){
			case 0:
				break;
			case 'a':
				ret = sscanf(optarg, "%i", &device_id);
				plattform_id = 0;
				if(ret < 1 || plattform_id < 0 || device_id < -1)
					show_help(argv[0]);

				if(plattform_id > 0){
					fprintf(stderr, "sorry, changing the platform is currently not supported\n");
					exit(1);
				}

				break;
			case '?':
				break;
			default:
				abort();
		}
	}


    cl_platform_id* platforms = get_all_platforms(&num_platforms, NULL);
	devices = get_all_devices(platforms, num_platforms, &num_devices);
    free(platforms);
//	devices = get_all_devices(&num_devices);
//  	cl_device_id* devices = get_device_by_platform(&num_devices, 1);

    if(devices == NULL){
		fprintf(stderr, "get_all_devices() failed!\n");
		return 1;
	}

    int lower_id = 0, upper_id = 0;

    if(device_id == -1)
    {
        lower_id = 0;
        upper_id = num_devices;
    }
    else
    {
        lower_id = device_id;
        upper_id = device_id + 1;
    }

	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: args:unsigned, loc:unsigned, compile_time:double, invocation_time:double, delta:double\n");
	} else {
 		printf("# <cur_test>, <total_tests>, <args>, <loc>, <compile time (ms)>, <duration (ms)>, <delta (ms)>\n");
	}

	for(device_id = lower_id; device_id < upper_id; ++device_id){

		if(device_id > (int)num_devices){
			fprintf(stderr, "invalid device id!\n");
			exit(1);
		}

		if(!csv_flag) print_short_device_info(devices[device_id], i);
		else print_csv_device_info(devices[device_id]);
	
		cl_context context = clCreateContext(NULL, 1, &devices[device_id], NULL, NULL, &err);
		if(!context){
			fprintf(stderr, "clCreateContext() failed!\n");
			fprintf(stderr, "err: %i\n", err);
			return -1;
		}

		cl_command_queue commands = clCreateCommandQueue(context, devices[device_id], CL_QUEUE_PROFILING_ENABLE, &err);
		if(!commands){
			fprintf(stderr, "clCreateCommandQueue() failed!\n");
			fprintf(stderr, "err: %s\n", oclErrorString(err));
			return -1;
		}

		int avgLoc = 0;
		double compile = 0.0, avgCompile = 0.0, avgDuration = 0.0, avgDelta = 0.0;

		j = sizeof(ki_tests)/sizeof(struct _test_case);

		for(i = 0; i < j; i++){
			if(run_kernel_benchmark<float>(devices[device_id], context, commands, ki_tests[i].kernel_args, ki_tests[i].kernel_loc, &duration, &delta, &compile) < 0){
				fprintf(stderr, "run_kernel_benchmark() failed!\n");
				return 1;
			}
			avgLoc += ki_tests[i].kernel_loc;
			avgCompile += compile;
			avgDuration += duration;
			avgDelta += delta;
			if(csv_flag) printf("%8i,%8i,%10.4lf,%10.4lf,%10.4lf\n", i + 1, j, ki_tests[i].kernel_args, ki_tests[i].kernel_loc, compile ,duration, delta);
			else printf("%8i,%8i,%8i,%8i,%10.4lf,%10.4lf,%10.4lf\n", i + 1, j, ki_tests[i].kernel_args, ki_tests[i].kernel_loc, compile ,duration, delta);
			fflush(stdout);
		}

		if(!csv_flag) printf("avg,%i,-,%i,%f,%f,%f\n", j, avgLoc/j, avgCompile/j, avgDuration/j, avgDelta/j);

		clReleaseContext(context);
		clReleaseCommandQueue(commands);
	}

	return 0;
}
