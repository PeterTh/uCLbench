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
 *  uCLbench OpenCL Memory Latency Benchmark
 *
 *  Authors: Peter Thoman, Klaus Kofler
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cassert>

#include "clcommon.h"
#include "clinfo.h"

// defines
#define BM_NAME "mem_latency"
#define BM_VERSION "1.0"

#define CACHE_STEP_WIDTH 128
#define REQUIRED_ITERATION_DIVISOR 128

#define DEFAULT_LOCAL_ITERATIONS 1000
#define DEFAULT_LOCAL_BYTESIZE 32 * 1024 * 1024
#define DEFAULT_CONSTANT_ITERATIONS 1000
#define DEFAULT_CONSTANT_BYTESIZE 32 * 1024 * 1024
#define DEFAULT_GLOBAL_ITERATIONS 2
#define DEFAULT_GLOBAL_BYTESIZE 32 * 1024 * 1024

enum _MEM_TYPE { _GLOBAL, _CONSTANT, _LOCAL };
enum _DATA_LAYOUT { _SEQUENTIAL, _CACHE_STEP };

///////////////////////////////////////////////////////////////////////////////
//  Convert _MEM_TYPE to string
///////////////////////////////////////////////////////////////////////////////
char* mtype_to_string(_MEM_TYPE mtype) {
	switch (mtype) {
		case _GLOBAL: return (char*)"global";
		case _LOCAL: return (char*)"local";
		case _CONSTANT:	return (char*)"const";
		default: return (char*)"unknown_mem_type";
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Convert _DATA_LAYOUT to string
///////////////////////////////////////////////////////////////////////////////
char* data_layout_to_string(_DATA_LAYOUT layout) {
	switch (layout) {
	case _SEQUENTIAL: return (char*)"sequential";
	case _CACHE_STEP: return (char*)"cache_step";
	default: return (char*)"unknown_data_layout";
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Run the latency benchmark for the given array-size and memory type
///////////////////////////////////////////////////////////////////////////////
int run_latency_benchmark(cl_device_id device_id, cl_context context,							// OpenCL info
		cl_ulong size, _MEM_TYPE mt, _DATA_LAYOUT layout, unsigned long iterations,				// benchmark settings
		double *time_taken, unsigned long long *ops_performed, double *data_size_kb) {			// return values
	
    int err;                            // error code returned from api calls
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel
	cl_event event;						// event used to record timing information
	cl_uint* data;						// data buffer traversed
	
	// adjust requested size based on device capabilities
	_DEVICE_INFO* info = get_device_info(device_id);
	cl_ulong limit = 0;
	switch(mt) {
	case _GLOBAL: limit = info->global_mem_size; break;
	case _LOCAL: limit = info->local_mem_size; break;
	case _CONSTANT: limit = info->max_constant_buffer_size; break;
	}
	limit -= 64; // need some space left in smem
	if(mt == _LOCAL && info->type == CL_DEVICE_TYPE_ACCELERATOR) { // Cell SPE local mem size misreporting fix
		limit /= 2;
	}
	if(size > limit) size = limit;
	unsigned num_elems = size/sizeof(cl_uint);
	unsigned remainder = num_elems % REQUIRED_ITERATION_DIVISOR;
	num_elems -= remainder;
	size = num_elems * sizeof(cl_uint);
	//printf("\n--- %s - final size: %6.2f KB, number of elems: %12u\n", mtype_to_string(mt), size / 1024.0, num_elems);

	// prepare data buffer according to layout
	data = (cl_uint*) malloc(size);
	cl_uint index = 1;
	switch(layout) {
	case _CACHE_STEP:	
		data[0] = 1;
		while(true) {
			cl_uint newindex = index + CACHE_STEP_WIDTH;
			if(newindex == num_elems) {
				data[index] = 0;
				break;
			}
			if(newindex > num_elems) {
				newindex = newindex % (num_elems-1);
			}
			data[index] = newindex;
			index = newindex;
		}
	break;
	case _SEQUENTIAL:	
		for(unsigned i = 0; i<num_elems; ++i) {
			data[i] = i+1;
		}
		data[num_elems-1] = 0;
	break;
	}

	// check data layout and calculate expected result
	cl_uint next = data[0];
	cl_uint opcount = 1;
	cl_uint expected_result = 0;
	while(next) {
		next = data[next];
		opcount++;
		if(opcount%REQUIRED_ITERATION_DIVISOR == 0) expected_result += next;
	}
	expected_result *= iterations;
	assert(opcount == num_elems && "unexpected number of operations");
	
    // Create a command queue
    commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
    if(!commands) {
        fprintf(stderr, "Error: Failed to create a command commands!\n");
        return 1;
    }
	
    // Create the compute program from the source buffer
	char *source_path; 
	switch(mt) {
		case _LOCAL: source_path = "local_latency.cl"; break;
		case _CONSTANT: source_path = "constant_latency.cl"; break;
		default: source_path = "global_latency.cl"; break;
	}
	
	char prefix[256];
	snprintf(prefix, 256, "#define SIZE %d\n", num_elems);
	program = load_kernel(source_path, context, prefix);
	if(!program) {
        fprintf(stderr, "Error: Failed to create compute program!\n");
        return 1;
    }
	
    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if(err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
		
        fprintf(stderr, "Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        return 1;
    }
	
    kernel = clCreateKernel(program, "latencyCnt", &err);
    if(!kernel || err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to create compute kernel!\n");
        return 1;
    }
	
    // Create the input and output array in device memory and transfer data
	cl_mem input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  size, NULL, NULL);
	cl_mem result = clCreateBuffer(context, CL_MEM_WRITE_ONLY,  sizeof(cl_uint), NULL, NULL);
	if(!input || !result) {
		fprintf(stderr, "Error: Failed to allocate device memory!\n");
		return 1;
	}
	err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, size, data, 0, NULL, NULL);
	err |= clFinish(commands);
	if(err != CL_SUCCESS) {
		fprintf(stderr, "Error: Failed to write to source array!\n");
		return 1;
	}

    // Set the arguments to our compute kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &result);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &iterations);
    if(err != CL_SUCCESS) {
        fprintf(stderr, "Error: Failed to set kernel arguments! %d\n", err);
        return 1;
    }

	// Execute the kernel using just one single workitem
	size_t global = 1;
	size_t local = 1;

	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, &event);
	if(err != CL_SUCCESS)	{
		fprintf(stderr, "Error %i: Failed to execute kernel!\n%s\n", err, oclErrorString(err));
		return 1;
	}
	// Wait for the commands to get serviced before reading back results
	err = clFinish(commands);
	err |= clWaitForEvents(1, &event);
	if(err != CL_SUCCESS) {
		fprintf(stderr, "Error %i: Failed to wait for benchmarking kernel to finish.\n%s\n", err, oclErrorString(err));
		return 1;
	}

	// return time taken 
	cl_ulong time_start, time_end;
	err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
	err |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
	if(err != CL_SUCCESS) {
		fprintf(stderr, "Error %i: Failed to retrieve profiling info!\n%s\n", err, oclErrorString(err));
		return 1;
	}
	*time_taken = (time_end - time_start) * 1.0e-9;

	// check if result is as expected
	cl_uint cl_result;
	err = clEnqueueReadBuffer(commands, result, CL_TRUE, 0, sizeof(cl_uint), &cl_result, 0, NULL, NULL);
	err |= clFinish(commands);
	if(err != CL_SUCCESS) {
		fprintf(stderr, "Error %i: Failed to read back result.\n%s\n", err, oclErrorString(err));
		return 1;
	}
	if(cl_result != expected_result) {
		fprintf(stderr, "Error: result mismatch: expected %u, got %u\n", expected_result, cl_result);
	}

	// return number of operations performed and actual data block size
	*ops_performed = iterations * opcount;
	*data_size_kb = size / 1024.0;

    // Shutdown and cleanup
    clReleaseMemObject(input);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
	free(data);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  Print short program-usage
///////////////////////////////////////////////////////////////////////////////
void print_usage() {
	printf("usage: mem_latency [options]\n");
	printf("\n");
	printf("where options include:\n");
	printf(" --info     print short info on available devices\n");
	printf(" --device=<integer>\n");
	printf("            benchmark given device (integer refers to output of --info)\n");
	printf(" --size=<integer>\n");
	printf("            perform benchmark with given size (in bytes)\n");
	printf(" --type=[global|local|constant]\n");
	printf("            perform benchmark for the given memory type\n");
	printf(" --layout=[sequential|cachestep]\n");
	printf("            use given memory layout\n");
	printf(" --repeats=<integer>\n");
	printf("            measure given amount of times and return mean values\n");
	printf(" --csv      output results as csv\n");
}


///////////////////////////////////////////////////////////////////////////////
//  MAIN: collect input parameters and run benchmarks
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

	int info_flag = 0;
	int usage_flag = 0;
	int csv_flag = 0;
	_MEM_TYPE mem_type = _GLOBAL;
	_DATA_LAYOUT data_layout = _CACHE_STEP;
	bool type_set = false;
	bool layout_set = false;

	long bytesize_param = -1;
	int device_id = -1;
	unsigned repeats = 1;
	int iterations_param = -1;
	
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
			{"size",    optional_argument, 0, 'a'},
			{"device",  optional_argument, 0, 'b'},
			{"repeats",  optional_argument, 0, 'c'},
			{"type",  optional_argument, 0, 'd'},
			{"iterations", optional_argument, 0, 'e'},
			{"layout", optional_argument, 0, 'f'},
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
			bytesize_param = atol(optarg);
			break;
		case 'b':
			device_id = atoi(optarg);
			break;
		case 'c':
			repeats = atoi(optarg);
			break;
		case 'd':
			for(int i=0; i<3; ++i) {
				if(strcmp(mtype_to_string((_MEM_TYPE)i), optarg) == 0) {
					mem_type = (_MEM_TYPE)i;
					type_set = true;
				}
			}
			break;
		case 'e':
			iterations_param = atoi(optarg);
			break;
		case 'f':
			for(int i=0; i<2; ++i) {
				if(strcmp(data_layout_to_string((_DATA_LAYOUT)i), optarg) == 0) {
					data_layout = (_DATA_LAYOUT)i;
					layout_set = true;
				}
			}
			break;
		case '?':
			/* getopt_long already printed an error message. */
			break;
		default:
			;
		}
	}
	
	// retrieve devices
	unsigned int num_devices = 0, num_platforms = 0;
    cl_platform_id* platforms = get_all_platforms(&num_platforms, NULL);
	cl_device_id* devices = get_all_devices(platforms, num_platforms, &num_devices);
    free(platforms);

	if(devices == NULL) {
		fprintf(stderr, "Error: Failed to create a device group!\n");
		return 1;
	}
	if(usage_flag) {
		print_usage();
		return 0;
	}
	if(info_flag) {
		print_device_list(devices, num_devices);
		return 0;
	}
	
	// retrieve devices to be benchmarked
	cl_device_id *used_devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);
	unsigned int used_num_devices = 0;
	if(device_id == -1) {
		// nothing specified, run benchmark for all devices
		for(unsigned int i = 0; i < num_devices; i++) used_devices[i] = devices[i];
		used_num_devices = num_devices;
	} else {
		used_num_devices = 1;
		used_devices[0] = devices[device_id];
	}
	
	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: mem_space:string, data_layout:string, data_size:unsigned, operations:unsigned, exec_time:double, nsecs_per_op:double\n");
	}

	for (unsigned int i = 0; i < used_num_devices; i++) {
		if(!csv_flag) print_short_device_info(used_devices[i], i);
		else print_csv_device_info(used_devices[i]);
		for(unsigned int layoutcnt = 0; layoutcnt < 2; ++layoutcnt)
		{
			if(layout_set) { // layout specified on input
				if(layoutcnt>0) break;
			} else {
				data_layout = (_DATA_LAYOUT)layoutcnt;
			}

			for(unsigned int mtcnt = 0; mtcnt < 3; ++mtcnt)
			{
				unsigned long iterations = iterations_param;
				unsigned long bytesize = bytesize_param;

				if(type_set) { // mem type specified on input
					if(mtcnt>0) break;
				} else { // else test all mem types
					mem_type = (_MEM_TYPE)mtcnt;
					// set options default based on mem type
					switch(mem_type) {
					case _LOCAL:
						if(iterations_param == -1) iterations = DEFAULT_LOCAL_ITERATIONS;
						if(bytesize_param == -1) bytesize = DEFAULT_LOCAL_BYTESIZE;
						break;
					case _CONSTANT:
						if(iterations_param == -1) iterations = DEFAULT_CONSTANT_ITERATIONS;
						if(bytesize_param == -1) bytesize = DEFAULT_CONSTANT_BYTESIZE;
						break;
					case _GLOBAL:
						if(iterations_param == -1) iterations = DEFAULT_GLOBAL_ITERATIONS;
						if(bytesize_param == -1) bytesize = DEFAULT_GLOBAL_BYTESIZE;
						break;
					}
				}

				// Create a compute context 
				int err = 0;
				cl_context context = clCreateContext(0, 1, &used_devices[i], NULL, NULL, &err);
				if(!context)
				{
					fprintf(stderr, "Error: Failed to create a compute context!\n");
					return 1;
				}

				double time = 0.0;
				unsigned long long number_of_ops = 0;
				double data_size_kb = 0.0;
				for(unsigned int k = 0; k < repeats; k++) {
					double _time = 0.0;
					err = run_latency_benchmark(used_devices[i], context, bytesize, mem_type, data_layout, iterations, 
												&_time, &number_of_ops, &data_size_kb);
					if(err > 0) {
						time = NAN;
						break;
					}
					time += _time;
				}
				time /= repeats;
				double nanosperop = (time*1000000000) / (double)number_of_ops;
				unsigned long long data_size_b = (unsigned long long)data_size_kb*1000;
			
				if(!csv_flag) printf("Mem space: %8s, data layout: %12s, %8.2f KB data size, %12llu operations, %6.3f secs %8.2f nsecs/op\n", 
									 mtype_to_string(mem_type), data_layout_to_string(data_layout), data_size_kb, number_of_ops, time, nanosperop);
				else printf("%12s,%12s,%16llu,%16llu,%8.4f,%16.4f\n", 
						    mtype_to_string(mem_type), data_layout_to_string(data_layout), data_size_b, number_of_ops, time, nanosperop);
				fflush(stdout);

				clReleaseContext(context);
			}
		}
	}
	
	// finally free up all the used memory-chunks
	free(used_devices);
	free(devices);
	
	return 0;
}
