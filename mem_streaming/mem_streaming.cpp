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
 *  uCLbench OpenCL Memory Streaming Benchmark
 *
 *  Authors: Klaus Kofler, Peter Thoman, Michael Brunner, Richard Weinberger
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "clcommon.h"
#include "clinfo.h"

// defines
#define BM_NAME "mem_streaming"
#define BM_VERSION "1.0"

#define MAX_SIZES 50
#define MAX_WORKITEMS 50
#define DEFAULT_REPEATS 1
#define DEFAULT_ITERATIONS 10
#define WARMUP_CYCLES 3

////////////////////////////////////////////////////////////////////////////////////
//  Measure the local memoy to local memoy bandwidth.
////////////////////////////////////////////////////////////////////////////////////
int measureLocalMemory(cl_device_id device_id, cl_context context, cl_command_queue commands, 
	unsigned int type, int f4, unsigned int elements, unsigned int iterations, bool larg, double time_taken[2]) {

    cl_int err = CL_SUCCESS;
 	const char* source_path = "mem_streaming.cl";
	char buf[512];

    int elementsToAlloc = elements;

	size_t local, global;

    for(size_t ws = 0; ws <= 1; ++ws)
    {
        if(ws == 0)
        {
           // Execute the kernel using just one single workitem
            local = 1;
            global = 1;
        }
        else
        {
            // Execute the kernel using the max number of threads on each processor
    	    _DEVICE_INFO* info = get_device_info(device_id);
            size_t* tmp = info->max_work_item_sizes;
            local = tmp[0];
            free(tmp);
            global = info->max_compute_units;

            while(local > elements)
                local /= 2;

            global *= local;
        }

        if(type == 1)
            elementsToAlloc = (elements + local-1)/local;

        if(f4 == 0)
            sprintf(buf, "#define dtype float\n");
        else
            sprintf(buf, "#define dtype float%d\n", (int)pow(2.0, f4));

        sprintf(buf+strlen(buf), "#define VEC %d\n#define ELEMENTS %d\n#define localRange %lu\n", f4, elementsToAlloc, local);

        if(larg)
            sprintf(buf+strlen(buf), "#define LARG\n");

		cl_program program = load_kernel(source_path, context, buf);
		if(!program)
		{
			fprintf(stderr, "Error: Failed to create compute program!\n");
			return 1;
		}

		// Build the program executable
		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if(err != CL_SUCCESS)
		{
			size_t len;
			char buffer[8096];
		
			fprintf(stderr, "Error: Failed to build program executable!\n");
			clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
			fprintf(stderr, "%s\n", buffer);
			return 1;
		}
	
		// Create the compute kernel 
        cl_kernel kernel;
        switch(type)
        {
        case 1:
		    kernel = clCreateKernel(program, "private_mem", &err);
            break;
        case 2:
		    kernel = clCreateKernel(program, "global_mem", &err);
            break;
        default:
    	    kernel = clCreateKernel(program, "local_mem", &err);
        }

		if (!kernel || err != CL_SUCCESS)
		{
			fprintf(stderr, "Error: Failed to create compute kernel!\n");
			return 1;
		}

 
        float* hOutput = (float*)malloc(global * sizeof(float));
        memset(hOutput, 0, global * sizeof(float));

        cl_mem output = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * global, hOutput, NULL);

        if (!output || err != CL_SUCCESS)
        {
            fprintf(stderr, "Error: Failed to allocate device memory!\n");
            return 1;
        }    

        // Set the arguments to our compute kernel
        err = CL_SUCCESS;
        err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &output);
        cl_mem g1, g2;
        switch(type)
        {
        case 1:
            break;
        case 2:
            switch(f4)
            {
                case(1):
                    g1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * elements, NULL, NULL);
                    g2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * elements*2, NULL, NULL);
                    break;
                case(2):
                    g1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * elements, NULL, NULL);
                    g2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * elements*2, NULL, NULL);
                    break;
                case(3):
                    g1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float8) * elements, NULL, NULL);
                    g2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float8) * elements*2, NULL, NULL);
                    break;
                case(4):
                    g1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float16) * elements, NULL, NULL);
                    g2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float16) * elements*2, NULL, NULL);
                    break;
                default:
                    g1 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * elements, NULL, NULL);
                    g2 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * elements*2, NULL, NULL);
                    break;
                break;
            }
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &g1);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &g2);
            break;
        default:
            if(larg)
            switch(f4)
            {
                case(1):
                    err |= clSetKernelArg(kernel, 1, sizeof(cl_float2)*elements, NULL);
                    err |= clSetKernelArg(kernel, 2, sizeof(cl_float2)*elements*2, NULL);
                    break;
                case(2):
                    err |= clSetKernelArg(kernel, 1, sizeof(cl_float4)*elements, NULL);
                    err |= clSetKernelArg(kernel, 2, sizeof(cl_float4)*elements*2, NULL);
                    break;
                case(3):
                    err |= clSetKernelArg(kernel, 1, sizeof(cl_float8)*elements, NULL);
                    err |= clSetKernelArg(kernel, 2, sizeof(cl_float8)*elements*2, NULL);
                    break;
                case(4):
                    err |= clSetKernelArg(kernel, 1, sizeof(cl_float8)*elements, NULL);
                    err |= clSetKernelArg(kernel, 2, sizeof(cl_float8)*elements*2, NULL);
                    break;
                default:
                    err |= clSetKernelArg(kernel, 1, sizeof(cl_float)*elements, NULL);
                    err |= clSetKernelArg(kernel, 2, sizeof(cl_float)*elements*2, NULL);
                    break;
                break;
            }
        }
        if (err != CL_SUCCESS)
        {
            fprintf(stderr, "Error: Failed to set kernel arguments! %d\n", err);
            return 1;
        }

		// warmup
		for(unsigned i = 0; i < WARMUP_CYCLES; ++i) 
		{
            err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
			clFinish(commands);
		}

	    // start actual measurement
		unsigned long start_time = current_msecs();
        for(unsigned i = 0; i < iterations; ++i)
        {
            err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
	        if (err)
            {
                fprintf(stderr, "Error %i: Failed to execute kernel!\n%s\n", err, oclErrorString(err));
                return 1;
            }
            clFlush(commands);
        }
        clFinish(commands);
		time_taken[ws] = elapsed_msecs(start_time) / 1000.0;

/*
        cl_event read;
        err = clEnqueueReadBuffer(commands, output, CL_FALSE, 0, global*sizeof(float), hOutput, 0, NULL, &read);
	    if (err)
        {
            fprintf(stderr, "Error %i: Failed read buffer!\n%s\n", err, oclErrorString(err));
            return 1;
        }

        clWaitForEvents(1, &read);

        for(size_t i = 0; i < global; ++i)
            printf(", %d %f ", i, hOutput[i]);
        printf("\n\n");
*/
        free(hOutput);
        clReleaseMemObject(output);
        if(type == 2)
        {
            clReleaseMemObject(g1);
            clReleaseMemObject(g2);
        }
		clReleaseProgram(program);
		clReleaseKernel(kernel);
    }

	
	return err;
}


///////////////////////////////////////////////////////////////////////////////
//  Prepare the benchmark for the given device
///////////////////////////////////////////////////////////////////////////////
int prepare_benchmark(cl_device_id device_id, cl_context *context, cl_command_queue *commands) {
	int err;
	
	// Create a compute context 
    *context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if(!*context)
    {
        printf("Error: Failed to create a compute context!\n");
        return err;
    }
	
    // Create a command queue
    *commands = clCreateCommandQueue(*context, device_id, 0, &err);
    if(!*commands)
    {
        printf("Error: Failed to create a command queue!\n");
        return err;
    }
		
	// everything was fine
	return CL_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//  Run the bandwidth benchmark for a single memory-size
///////////////////////////////////////////////////////////////////////////////
int run_benchmark(cl_device_id device, cl_context context, cl_command_queue commands, unsigned int type,
                  int f4, unsigned int elements, unsigned int iterations, bool larg, 
                  double *time_taken) {
	
    int err;  // error code returned from api calls
	
	// then do the localmem-to-localmem measurement
	err = measureLocalMemory(device, context, commands, type, f4, elements, iterations, larg, time_taken);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed local -> local!\n");
		return err;
	}
	    
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  Print short program-usage
///////////////////////////////////////////////////////////////////////////////
void print_usage() {
	printf("usage: bandwidth [options]\n");
	printf("\n");
	printf("where options include:\n");
	printf(" --info     print short info on available devices\n");
	printf(" --device=<integer>\n");
	printf("            benchmark given device (integer refers to output of --info)\n");
	printf(" --sizes=<integer-list>\n");
	printf("            perform benchmark with given list of memory-sizes (comma-separated)\n");
	printf(" --pinned   use pinned memory\n");
	printf(" --mapped   use mapped memory\n");
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
    int type_flag = 0;
	int c;
	char *sizes_str = NULL;
	char *devices_str = NULL;
    char *vector_str = NULL;
	char *repeat_str = NULL;
	char *iterations_str = NULL;
	
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"info", no_argument,       &info_flag, 1},
			{"help", no_argument,       &usage_flag, 1},
			{"csv",  no_argument,       &csv_flag, 1},
            {"private", no_argument,       &type_flag, 1},
            {"global", no_argument,       &type_flag, 2},
            {"local", no_argument,       &type_flag, 0},
			/* These options don't set a flag.
			 We distinguish them by their indices. */
			{"sizes",    optional_argument, 0, 'a'},
			{"device",  optional_argument, 0, 'b'},
			{"repeats",  optional_argument, 0, 'c'},
			{"iterations",  optional_argument, 0, 'd'},
			{"vector",  optional_argument, 0, 'e'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "a:", long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				break;
			case 'a':
				sizes_str = optarg;
				break;
			case 'b':
				devices_str = optarg;
				break;
			case 'c':
				repeat_str = optarg;
				break;
			case 'd':
				iterations_str = optarg;
				break;
			case 'e':
				vector_str = optarg;
				break;
			case '?':
				/* getopt_long already printed an error message. */
				break;
			default:
				abort ();
		}
	}
	
	
	// retrieve devices
	unsigned int num_devices = 0, num_platforms;

    cl_platform_id* platforms = get_all_platforms(&num_platforms, NULL);
	cl_device_id* devices = get_all_devices(platforms, num_platforms, &num_devices);
    free(platforms);
	if(devices == NULL) {
		printf("Error: Failed to create a device group!\n");
		return 1;
	}
		
	if (info_flag) {
		print_device_list(devices, num_devices);
		return 0;
	}
	
	if (usage_flag) {
		print_usage();
		return 0;
	}
	
	// retrieve devices to be benchmarked
	cl_device_id *used_devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);
	unsigned int used_num_devices = 0;
	if((*devices_str == '\0') || (strcmp(devices_str, "all") == 0)) {
		// nothing specified, run benchmark for all devices
		for(unsigned int i = 0; i < num_devices; i++) used_devices[i] = devices[i];
		used_num_devices = num_devices;
	} else {
		// check the given device-numbers and fill up the device-array
		char* ptr;
		used_num_devices = 0;
		ptr = strtok(devices_str, ",");
		while(ptr != NULL) {
			unsigned int id = 0;
			if(sscanf(ptr, "%i", &id) > 0) {
				if(id >= 0 && id < num_devices) {
					used_devices[used_num_devices] = devices[id];
					used_num_devices++;
				}
			} else {
				printf("invalid device-number given (%d)\n", id);
				return 1;
			}
			ptr = strtok(NULL, ",");
		}
	}
	
	// retrieve memory-sizes to run the benchmark with  -- momentary unused
	unsigned long* sizes = (unsigned long*) malloc(sizeof(unsigned long) * MAX_SIZES * used_num_devices);
	for (unsigned int i = 0; i < MAX_SIZES * used_num_devices; i++) {
		sizes[i] = 0;
	}
	unsigned int num_sizes = 0;
	if(*sizes_str == '\0') {
		// nothing specified, test for maximum
		num_sizes = 1;
		for (unsigned int i = 0; i < used_num_devices; i++) {
			sizes[i * MAX_SIZES] = 10 * 1024 * 1024;
		}
	} else {
		// check given numbers and fill up the device-array
		char* ptr;
		ptr = strtok(sizes_str, ",");
		while(ptr != NULL) {
			unsigned long size = 0;
			unsigned long _s;
			char _m;
			if (sscanf(ptr, "%lu%c", &_s, &_m) == 2) {
				switch (_m) {
					case 'K': case 'k':
						size = _s * 1024;
						break;
					case 'M': case 'm':
						size = _s * 1024 * 1024;
						break;
					default:
						printf("invalid size given (%s)\n", ptr);
						return 1;
				}
			} else if (sscanf(ptr, "%lu", &_s) > 0) {
				size = _s;
			} else {
				printf("invalid size given (%s)\n", ptr);
				return 1;
			}
			for (unsigned int i = 0; i < used_num_devices; i++) {
				sizes[num_sizes + i * MAX_SIZES] = size;
			}
			num_sizes ++;
			ptr = strtok(NULL, ",");
		}		
	}
	
	// retrieve amount of repeats for each data-point
	unsigned int repeats = 0;
	if (*repeat_str == '\0') {
		repeats = DEFAULT_REPEATS;
	} else {
		if (sscanf(repeat_str, "%d", &repeats) > 0) {
			// nothing more to do
		} else {
			printf("invalid number of repeats given (%s)\n", repeat_str);
			return 1;
		}
	}
	
	// retrieve amount of iterations used for each data-point
	unsigned int iterations = 0;
	if (*iterations_str== '\0') {
		iterations = DEFAULT_ITERATIONS;
	} else {
		if (sscanf(iterations_str, "%d", &iterations) > 0) {
			// nothing more to do
		} else {
			printf("invalid number of iterations given (%s)\n", iterations_str);
			return 1;
		}
	}

    int f4 = 0;
    char vec[16];
    if (*vector_str== '\0') {
        f4 = 0;
    } else {
        if (sscanf(vector_str, "%d", &f4) < 0 || f4 < 0 || f4 > 4) {
            printf("Request for unsupported vector type! Supported are:\n \
                   \t0 -> float\n \
                   \t1 -> float2\n \
                   \t2 -> float4\n \
                   \t3 -> float8\n \
                   \t4 -> float16\n");
            return 1;
        }
    }
    int sizeoft = 0;
    //Private memory test (float)
    //INTEL_MAX = 3794 (crash at clFinish after NDRangeKernel)
    //AMD_MAX = 637 (crash at releaseCommandQueue)
    //b09_MAX = 4325 (aborted)
    //b05_MAX = 662 (/home/ocl/openclmainline/runtime/src/thread/semaphore.cpp:60: pthread_mutex_lock() failed)
    //b01_MAX = 652 (Segmentation fault)
    //NVIDIA_MAX = 1365 (build error, uses too much local data)
    //ATI_MAX = 136 (driver crashes)
    int elements = 1024;
    switch(f4)
    {
    case(1):
	    sprintf(vec, "float2");
        sizeoft = sizeof(cl_float2);
        elements /= 2;
        break;
    case(2):
	    sprintf(vec, "float4");
        sizeoft = sizeof(cl_float4);
        elements /= 4;
        break;
    case(3):
	    sprintf(vec, "float8");
        sizeoft = sizeof(cl_float8);
        elements /= 8;
        break;
    case(4):
	    sprintf(vec, "float16");
        sizeoft = sizeof(cl_float16);
        elements /= 16;
        break;
    default:
        sprintf(vec, "float");
        sizeoft = sizeof(cl_float);
        break;
    }

    char type_str[16];
    switch(type_flag)
    {
    case 1:
        sprintf(type_str, "private");
        break;
    case 2:
        sprintf(type_str, "global");
        break;
    default:
        sprintf(type_str, "local");
        break;
    }

    // now after all is defined, start the benchmark(s) and print the results
	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: \n");
	}
	cl_context context = NULL;
	cl_command_queue commands = NULL;
	for (unsigned int i = 0; i < used_num_devices; i++) {
		prepare_benchmark(used_devices[i], &context, &commands);
		printf(csv_flag ? "# memspace; declaration; type; single-threaded; multi-threaded\n" : "");
		print_short_device_info(used_devices[i], i);
        for(int l = 0; l < (type_flag ? 1 : 2); ++l) // make two iterations for local memory, one for mem passed as argument, the other when declared in kernel
        {
			for(unsigned int j = 0; j < num_sizes; j++) {
			    double time[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
			    int err = 0;
			    for(unsigned int k = 0; k < repeats; k++) {
				    double _time[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
				    err = run_benchmark(used_devices[i], context, commands, type_flag, f4, elements, iterations, (bool)l, _time);
				    if(err > 0) {
					    time[0] = NAN; time[1] = NAN; time[2] = NAN; time[3] = NAN; time[4] = NAN; time[5] = NAN;
					    break;
				    }
				    for(int m = 0; m < 6; m++) time[m] += _time[m];
			    }
			    for(int m = 0; m < 6; m++) time[m] /= repeats;
			    printf(csv_flag ? "%s,%s,%s,%lf,%lf\n" : "%s memory, declared %s, %s single threaded: %.2lf, multi threaded: %.2lf MB/s\n", 
                     type_str, l==0 ? "inkernel" : "argument", vec, ((elements*(double)400240*sizeoft) / (time[0]*1024*1024)), ((elements*(double)400240*sizeoft) / (time[1]*1024*1024)));
			    fflush(stdout);
		    }
        }
		int err = clReleaseCommandQueue(commands);
		err |= clReleaseContext(context);
		if(err != CL_SUCCESS) {
			printf("Error freeing queue or device.\n");
			exit(err);
		}
	}
	
	
	// finally free up all the used memory-chunks
	free(sizes);
	free(used_devices);
	free(devices);
	
	return 0;
}
