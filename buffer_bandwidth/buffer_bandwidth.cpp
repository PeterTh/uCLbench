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
 *  uCLbench OpenCL Memory Buffer Bandwidth Benchmark
 *
 *  Authors: Peter Thoman, Klaus Kofler
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "clcommon.h"
#include "clinfo.h"

// defines
#define BM_NAME "buffer_bandwidth"
#define BM_VERSION "1.0"

#define MAX_SIZES 64
#define MAX_WORKITEMS 64
#define DEFAULT_REPEATS 1
#define DEFAULT_ITERATIONS 25

// enums
enum _MemMode { PINNED, PAGEABLE };
enum _AccessMode { MAPPED, DIRECT };

///////////////////////////////////////////////////////////////////////////////
//  Measure the host-to-device bandwidth.
///////////////////////////////////////////////////////////////////////////////
int measureHostToDevice(cl_context context, cl_command_queue commands, cl_float* data, unsigned long size, 
		unsigned int iterations, _MemMode mm, _AccessMode am, unsigned long *msecs_taken) {
	int err = CL_SUCCESS;
	cl_mem dmem = NULL;				// device memory
	cl_mem pinned_hmem = NULL;		// pinned host memory
	void *mapped_dmem = NULL;		// mapped device memory
    cl_float *transfer_data = NULL;
	
    dmem = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(cl_float) * size, NULL, &err);
	if(err != CL_SUCCESS) {
		return err;
	}
	if(mm == PINNED) {
		// create pinned host buffer
		pinned_hmem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(cl_float), NULL, &err);
		if(err != CL_SUCCESS) {
			return err;
		}
		// move data to pinned host memory
		transfer_data = (float*) clEnqueueMapBuffer(commands, pinned_hmem, CL_TRUE, CL_MAP_WRITE, 0, size * sizeof(cl_float), 0, NULL, NULL, &err);
		if(err != CL_SUCCESS) {
			return err;
		}
		memcpy(transfer_data, data, sizeof(cl_float) * size);
		clEnqueueUnmapMemObject(commands, pinned_hmem, (void*) transfer_data, 0, NULL, NULL);
		// map again before use
		transfer_data = (cl_float*)clEnqueueMapBuffer(commands, pinned_hmem, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_float) * size, 0, NULL, NULL, &err);
		if(err != CL_SUCCESS) {
			return err;
		}
	} else { // use unpinned memory
		transfer_data = data;
	}
	clFinish(commands);

	// actual transfer
	unsigned long start_time = current_msecs();
	for(unsigned int i = 0; i < iterations; i++) {
		if(am == DIRECT) {
			err = clEnqueueWriteBuffer(commands, dmem, CL_FALSE, 0, sizeof(cl_float) * size, transfer_data, 0, NULL, NULL);
			if(err != CL_SUCCESS) {
				return err;
			}
		} else {
			mapped_dmem = clEnqueueMapBuffer(commands, dmem, CL_TRUE, CL_MAP_WRITE, 0, sizeof(cl_float) * size, 0, NULL, NULL, &err);
			if(err != CL_SUCCESS) {
				return err;
			}
			memcpy(mapped_dmem, data, sizeof(cl_float) * size);
			clEnqueueUnmapMemObject(commands, dmem, mapped_dmem, 0, NULL, NULL);
		}
	}
	clFinish(commands);
	*msecs_taken = elapsed_msecs(start_time);

	//if pinned, unmap
	if(mm == PINNED) {
		clEnqueueUnmapMemObject(commands, pinned_hmem, (void*) transfer_data, 0, NULL, NULL);
	}
	clFinish(commands);

	// get data back and check
	cl_float* check_data = (cl_float*)malloc(size*sizeof(cl_float));
	clEnqueueReadBuffer(commands, dmem, CL_TRUE, 0, size*sizeof(cl_float), check_data, 0, NULL, NULL);
	for(unsigned long i=0; i<size; ++i) {
		if(check_data[i] != data[i]) {
			printf("Error transferring data from host to device: %f != %f at index %lu\n", check_data[i], data[i], i);
			exit(-1);
		} 
	}
	free(check_data);

	// clean up all used buffers
	if(mm == PINNED) {
		clReleaseMemObject(pinned_hmem);
	}
	clReleaseMemObject(dmem);
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////
//  Measure the device-to-host bandwidth.
///////////////////////////////////////////////////////////////////////////////
int measureDeviceToHost(cl_context context, cl_command_queue commands, cl_float* data, unsigned long size, 
		unsigned int iterations, _MemMode mm, _AccessMode am, unsigned long *msecs_taken) {
	int err = CL_SUCCESS;
	cl_mem dmem = NULL;				// device memory
	cl_mem pinned_hmem = NULL;		// pinned host memory
	void *mapped_dmem = NULL;		// mapped device memory
    cl_float *transfer_data = NULL;
	
	// create device buffer and transfer data 
    dmem = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(cl_float) * size, NULL, &err);
	if(err != CL_SUCCESS) {
		return err;
	}
	err = clEnqueueWriteBuffer(commands, dmem, CL_FALSE, 0, sizeof(cl_float) * size, data, 0, NULL, NULL);

	// if pinned, create host buffer and map
	if(mm == PINNED) {
		pinned_hmem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(cl_float), NULL, &err);
		if(err != CL_SUCCESS) {
			return err;
		}
		transfer_data = (cl_float*)clEnqueueMapBuffer(commands, pinned_hmem, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_float) * size, 0, NULL, NULL, &err);
		if(err != CL_SUCCESS) {
			return err;
		}
	} else { // use unpinned memory
		transfer_data = data;
	}
	clFinish(commands);

	// actual transfer
	clFinish(commands);
	unsigned long start_time = current_msecs();
	for(unsigned int i = 0; i < iterations; i++) {
		if(am == DIRECT) {
			err = clEnqueueReadBuffer(commands, dmem, CL_FALSE, 0, sizeof(cl_float) * size, transfer_data, 0, NULL, NULL);
			if(err != CL_SUCCESS) {
				return err;
			}
		} else {
			mapped_dmem = clEnqueueMapBuffer(commands, dmem, CL_TRUE, CL_MAP_READ, 0, sizeof(cl_float) * size, 0, NULL, NULL, &err);
			if(err != CL_SUCCESS){
				continue;
			}
			memcpy(data, mapped_dmem, sizeof(cl_float) * size);
			clEnqueueUnmapMemObject(commands, dmem, mapped_dmem, 0, NULL, NULL);
		}
	}
	clFinish(commands);
	*msecs_taken = elapsed_msecs(start_time);

	//if pinned, unmap
	if(mm == PINNED) {
		clEnqueueUnmapMemObject(commands, pinned_hmem, (void*) transfer_data, 0, NULL, NULL);
	}
	clFinish(commands);

	// check data
	cl_float* check_data = (cl_float*)malloc(size*sizeof(cl_float));
	clEnqueueReadBuffer(commands, dmem, CL_TRUE, 0, size*sizeof(cl_float), check_data, 0, NULL, NULL);
	for(unsigned long i=0; i<size; ++i) {
		if(check_data[i] != data[i]) {
			printf("Error transferring data from device to host: %f != %f at index %lu\n", check_data[i], data[i], i);
			exit(-1);
		} 
	}
	free(check_data);
	
	// clean up all used buffers
	if(mm == PINNED) {
		clReleaseMemObject(pinned_hmem);
	}
	clReleaseMemObject(dmem);
	
	return err;
}

///////////////////////////////////////////////////////////////////////////////
//  Measure the device-to-device bandwidth.
///////////////////////////////////////////////////////////////////////////////
int measureDeviceToDevice(cl_context context, cl_command_queue commands, cl_float* data, unsigned long size, 
		unsigned int iterations, _MemMode mm, _AccessMode am, unsigned long *msecs_taken) {
	int err = CL_SUCCESS;
	cl_mem dmem_src = NULL;				// device memory - source
	cl_mem dmem_dst = NULL;				// device memory - destination
	
	// create device buffer and transfer data 
    dmem_src = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_float) * size, NULL, &err);
	if(err != CL_SUCCESS) {
		return err;
	}
    dmem_dst = clCreateBuffer(context,  CL_MEM_WRITE_ONLY,  sizeof(cl_float) * size, NULL, &err);
	if(err != CL_SUCCESS) {
		return err;
	}
	err = clEnqueueWriteBuffer(commands, dmem_src, CL_FALSE, 0, sizeof(cl_float) * size, data, 0, NULL, NULL);
	if(err != CL_SUCCESS) {
		return err;
	}
	
	// actual transfer
	clFinish(commands);
	unsigned long start_time = current_msecs();
	for (unsigned int i = 0; i < iterations; i++) {
		err = clEnqueueCopyBuffer(commands, dmem_src, dmem_dst, 0, 0, sizeof(cl_float) * size, 0, NULL, NULL);
	}
	clFinish(commands);
	*msecs_taken = elapsed_msecs(start_time);

	// get data back and check
	cl_float* check_data = (cl_float*)malloc(size*sizeof(cl_float));
	clEnqueueReadBuffer(commands, dmem_dst, CL_TRUE, 0, size*sizeof(cl_float), check_data, 0, NULL, NULL);
	for(unsigned long i=0; i<size; ++i) {
		if(check_data[i] != data[i]) {
			printf("Error transferring data from host to device: %f != %f at index %lu\n", check_data[i], data[i], i);
			exit(-1);
		} 
	}
	free(check_data);
	
    err = clReleaseMemObject(dmem_src);
    err |= clReleaseMemObject(dmem_dst);

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
int run_benchmark(cl_device_id device, cl_context context, cl_command_queue commands, _MemMode mm, 
                  _AccessMode am, unsigned long size, unsigned int iterations, double time_taken[]) {
	
    int err;                            // error code returned from api calls
	
	time_taken[0] = 0.0f;
	time_taken[1] = 0.0f;
	time_taken[2] = 0.0f;
	time_taken[3] = 0.0f;
	time_taken[4] = 0.0f;
	time_taken[5] = 0.0f;
	
	unsigned long amount = size / sizeof(cl_float);	// correct value for float
	float* data = (cl_float*) malloc(sizeof(cl_float) * amount);
    
	// Fill our data set with random floats
	for(long unsigned i = 0; i < amount; i++) {
		data[i] = (cl_float) rand();
	}
	
	unsigned long time_h2d, time_d2h, time_d2d;

	// start with the host-to-device measurement
	err = measureHostToDevice(context, commands, data, amount, iterations, mm, am, &time_h2d);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed host -> device!\n");
		return err;
	}
	
	// finish with the device-to-host measurement
	err = measureDeviceToHost(context, commands, data, amount, iterations, mm, am, &time_d2h);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed device -> host!\n");
		return err;
	}
	
	// then do the device-to-device measurement
	err = measureDeviceToDevice(context, commands, data, amount, iterations, mm, am, &time_d2d);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed device -> device!\n");
		return err;
	}
	
	// return time taken 
	time_taken[1] = (time_h2d/1000.0) / (double)iterations;
	time_taken[2] = (time_d2d/1000.0) / (double)iterations;
	time_taken[3] = (time_d2h/1000.0) / (double)iterations;
    
    // Shutdown and cleanup
	free(data);
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
	printf(" --mapped   use memory mapping\n");
	printf(" --repeats=<integer>\n");
	printf("            measure given amount of times and return mean values\n");
	printf(" --iterations=<integer>\n");
	printf("            number of iterations to perform\n");
	printf(" --csv      output results as csv\n");
}


///////////////////////////////////////////////////////////////////////////////
//  MAIN: collect input parameters and run benchmarks
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

	int info_flag = 0;
	int usage_flag = 0;
	int csv_flag = 0;
	int pinned_flag = 0;
	int mapped_flag = 0;
	int c;
	char *sizes_str = NULL;
	char *devices_str = NULL;
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
			{"pinned",  no_argument,       &pinned_flag, 1},
			{"pageable",  no_argument,       &pinned_flag, 0},
			{"mapped",  no_argument,       &mapped_flag, 1},
			{"direct",  no_argument,       &mapped_flag, 0},
			/* These options don't set a flag.
			 We distinguish them by their indices. */
			{"sizes",    optional_argument, 0, 'a'},
			{"device",  optional_argument, 0, 'b'},
			{"repeats",  optional_argument, 0, 'c'},
			{"iterations",  optional_argument, 0, 'd'},
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
	if((devices_str == '\0') || (strcmp(devices_str, "all") == 0)) {
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
	
	// retrieve memory-sizes to run the benchmark with
	unsigned long* sizes = (unsigned long*) malloc(sizeof(unsigned long) * MAX_SIZES * used_num_devices);
	for (unsigned int i = 0; i < MAX_SIZES * used_num_devices; i++) {
		sizes[i] = 0;
	}
	unsigned int num_sizes = 0;
	if(sizes_str == '\0') {
		// nothing specified, use default
		num_sizes = 1;
		for (unsigned int i = 0; i < used_num_devices; i++) {
			sizes[i * MAX_SIZES] = 128 * 1024 * 1024;
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
	if (repeat_str == '\0') {
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
	if (iterations_str== '\0') {
		iterations = DEFAULT_ITERATIONS;
	} else {
		if (sscanf(iterations_str, "%d", &iterations) > 0) {
			// nothing more to do
		} else {
			printf("invalid number of iterations given (%s)\n", iterations_str);
			return 1;
		}
	}

    // now after all is defined, start the benchmark(s) and print the results
	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: transfer_type:string, iterations:unsigned, size_bytes:unsigned, host2device:double, device2device:double, device2host:double\n");
	}
	cl_context context = NULL;
	cl_command_queue commands = NULL;
	for (unsigned int i = 0; i < used_num_devices; i++) {
		prepare_benchmark(used_devices[i], &context, &commands);
		if(!csv_flag) print_short_device_info(used_devices[i], i);
		else print_csv_device_info(used_devices[i]);

		for (unsigned int j = 0; j < num_sizes; j++) {
			unsigned long size = sizes[j + i * MAX_SIZES];
			double time[] = {0.0, 0.0, 0.0, 0.0};
			int err = 0;
			for(unsigned int k = 0; k < repeats; k++) {
				double _time[] = {0.0, 0.0, 0.0, 0.0};
				err = run_benchmark(used_devices[i], context, commands, pinned_flag ? PINNED : PAGEABLE, mapped_flag ? MAPPED : DIRECT, size, iterations, _time);
				if(err > 0) {
					time[0] = NAN; time[1] = NAN; time[2] = NAN; time[3] = NAN;
					break;
				}
				for(int m = 0; m < 4; m++) time[m] += _time[m];
			}
			for(int m = 0; m < 4; m++) time[m] /= repeats;
			// print results
			const char *transfer_type = mapped_flag ? "mapped" : (pinned_flag ? "pinned" : "direct");
			double h2d_mbps = (sizes[j + i * MAX_WORKITEMS] / time[1])/1024/1024;
			double d2d_mbps = (sizes[j + i * MAX_WORKITEMS] / time[2])/1024/1024;
			double d2h_mbps = (sizes[j + i * MAX_WORKITEMS] / time[3])/1024/1024;
			if(!csv_flag)
				printf("%i/%i %s %12lu Bytes \t%.2f MB/s(HD) \t%.2f MB/s(DD) \t%.2f MB/s(DH)\n", 
					j + 1, num_sizes, transfer_type, size, h2d_mbps, d2d_mbps, d2h_mbps);
			else
				printf("% 10s,% 8u,% 16lu,% 12.2lf,% 12.2lf,% 12.2lf\n", 
					transfer_type, iterations, size, h2d_mbps, d2d_mbps, d2h_mbps);
			fflush(stdout);
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
	
	exit(0);
}
