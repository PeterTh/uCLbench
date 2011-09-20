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

#include "clinfo.h"
#include <string.h>

void print_device_list(cl_device_id* devices, int size) {
	for (int i = 0; i < size; i++) {
		_DEVICE_INFO* info = get_device_info(devices[i]);
		printf("Device %i: ", i);
		printf("%s (%lu MB global memory)\n", info->name, (unsigned long) info->global_mem_size/1024/1024);
		printf("  max size: %u MB\n", (unsigned int)info->max_mem_alloc_size/1024/1024/2);
		free_device_info(info);
	}
}

const char* get_device_type_string(cl_device_type type) {
	return type == CL_DEVICE_TYPE_CPU ? "cpu" : 
		   type == CL_DEVICE_TYPE_GPU ? "gpu" : 
		   type == CL_DEVICE_TYPE_ACCELERATOR ? "accelerator" : 
		   "default";
}

char* get_plat_profile(cl_platform_id platform_id) {
	size_t size = 0;
	clGetPlatformInfo(platform_id, CL_PLATFORM_PROFILE, size, NULL, &size);
	char* value = (char*) malloc(sizeof(char) * size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_PROFILE, size, value, NULL);
	return value;
}
char* get_plat_version(cl_platform_id platform_id) {
	size_t size = 0;
	clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, size, NULL, &size);
	char* value = (char*) malloc(sizeof(char) * size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, size, value, NULL);
	return value;
}
char* get_plat_name(cl_platform_id platform_id) {
	size_t size = 0;
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, size, NULL, &size);
	char* value = (char*) malloc(sizeof(char) * size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, size, value, NULL);
	return value;	
}
char* get_plat_vendor(cl_platform_id platform_id) {
	size_t size = 0;
	clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, size, NULL, &size);
	char* value = (char*) malloc(sizeof(char) * size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, size, value, NULL);
	return value;	
}	
char* get_plat_extensions(cl_platform_id platform_id) {
	size_t size = 0;
	clGetPlatformInfo(platform_id, CL_PLATFORM_EXTENSIONS, size, NULL, &size);
	char* value = (char*) malloc(sizeof(char) * size);
	clGetPlatformInfo(platform_id, CL_PLATFORM_EXTENSIONS, size, value, NULL);
	return value;	
}
_PLATFORM_INFO* get_platform_info(cl_platform_id platform_id) {
	_PLATFORM_INFO* info = (_PLATFORM_INFO*) malloc(sizeof(_PLATFORM_INFO));
	info->id = platform_id;
	info->profile = get_plat_profile(platform_id);
	info->version = get_plat_version(platform_id);
	info->name = get_plat_name(platform_id);
	info->vendor = get_plat_vendor(platform_id);
	info->extensions = get_plat_extensions(platform_id);
	return info;
}
void print_platform_info(_PLATFORM_INFO* info, char* prefix, char* suffix) {
	char none[] = "";
	char nl[] = "\n";

	if (prefix == NULL) {
		prefix = none;
	}
	if (suffix == NULL) {
		suffix = nl;
	}
	printf("%sprofile:    %s%s", prefix, info->profile, suffix);
	printf("%sversion:    %s%s", prefix, info->version, suffix);
	printf("%sname:       %s%s", prefix, info->name, suffix);
	printf("%svendor:     %s%s", prefix, info->vendor, suffix);
	printf("%sextensions: %s%s", prefix, info->extensions, suffix);
	return;
}
void free_platform_info(_PLATFORM_INFO* info) {
	free(info->profile);
	free(info->version);
	free(info->name);
	free(info->vendor);
	free(info->extensions);
	free(info);
	return;
}




cl_device_type get_type(cl_device_id device_id) {
	cl_device_type retval;
	clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(retval), &retval, NULL);
	return retval;
}
cl_uint get_max_compute_units(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_uint get_max_work_item_dimensions(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(retval), &retval, NULL);
	return retval;	
}
size_t* get_max_work_item_sizes(cl_device_id device_id) {
	cl_uint size = get_max_work_item_dimensions(device_id);
	size_t *retval = (size_t*) malloc(size * sizeof(size_t));
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * size, retval, NULL);
	return retval;
}
size_t get_max_work_group_size(cl_device_id device_id) {
	size_t retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(retval), &retval, NULL);
	return retval;
}
cl_uint get_max_clock_frequency(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_uint get_address_bits(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_ADDRESS_BITS, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_ulong get_max_mem_alloc_size(cl_device_id device_id) {
	cl_ulong retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_bool has_image_support(cl_device_id device_id) {
	cl_bool retval;
	clGetDeviceInfo(device_id, CL_DEVICE_IMAGE_SUPPORT, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_uint get_max_samplers(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_SAMPLERS, sizeof(retval), &retval, NULL);
	return retval;	
}
size_t get_max_parameter_size(cl_device_id device_id) {
	size_t retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(retval), &retval, NULL);
	return retval;
}
cl_device_fp_config get_single_fp_config(cl_device_id device_id) {
	cl_device_fp_config retval;
	clGetDeviceInfo(device_id, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(retval), &retval, NULL);
	return retval;
}
cl_device_mem_cache_type get_global_mem_cache_type(cl_device_id device_id) {
	cl_device_mem_cache_type retval;
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(retval), &retval, NULL);
	return retval;
}
cl_uint get_global_mem_cacheline_size(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_ulong get_global_mem_cache_size(cl_device_id device_id) {
	cl_ulong retval;
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_ulong get_global_mem_size(cl_device_id device_id) {
	cl_ulong retval;
	clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_ulong get_max_constant_buffer_size(cl_device_id device_id) {
	cl_ulong retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_uint get_max_constant_args(cl_device_id device_id) {
	cl_uint retval;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_device_local_mem_type get_local_mem_type(cl_device_id device_id) {
	cl_device_local_mem_type retval;
	clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_ulong get_local_mem_size(cl_device_id device_id) {
	cl_ulong retval;
	clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_bool has_error_correction(cl_device_id device_id) {
	cl_bool retval;
	clGetDeviceInfo(device_id, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(retval), &retval, NULL);
	return retval;	
}
size_t get_profiling_timer_resolution(cl_device_id device_id) {
	size_t retval;
	clGetDeviceInfo(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_bool is_endian_little(cl_device_id device_id)  {
	cl_bool retval;
	clGetDeviceInfo(device_id, CL_DEVICE_ENDIAN_LITTLE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_bool is_available(cl_device_id device_id) {
	cl_bool retval;
	clGetDeviceInfo(device_id, CL_DEVICE_AVAILABLE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_bool has_compiler_available(cl_device_id device_id) {
	cl_bool retval;
	clGetDeviceInfo(device_id, CL_DEVICE_COMPILER_AVAILABLE, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_device_exec_capabilities get_exec_capabilities(cl_device_id device_id) {
	cl_device_exec_capabilities retval;
	clGetDeviceInfo(device_id, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(retval), &retval, NULL);
	return retval;	
}
cl_command_queue_properties get_queue_properties(cl_device_id device_id) {
	cl_command_queue_properties retval;
	clGetDeviceInfo(device_id, CL_DEVICE_QUEUE_PROPERTIES, sizeof(retval), &retval, NULL);
	return retval;	
}
char* get_name(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, size, retval, NULL);
	return retval;		
}
char* get_vendor(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, size, retval, NULL);
	return retval;		
}
char* get_driver_version(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_VERSION, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_VERSION, size, retval, NULL);
	return retval;		
}
char* get_profile(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_PROFILE, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_PROFILE, size, retval, NULL);
	return retval;		
}
char* get_version(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_VERSION, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_VERSION, size, retval, NULL);
	return retval;		
}
char* get_extensions(cl_device_id device_id) {
	size_t size = 0;
	clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, size, NULL, &size);
	char* retval = (char*) malloc(sizeof(char) * size);
	clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, size, retval, NULL);
	return retval;		
}
_DEVICE_INFO* get_device_info(cl_device_id device_id) {
	_DEVICE_INFO *info = (_DEVICE_INFO*) malloc(sizeof(_DEVICE_INFO));
	info->id = device_id;
	info->type = get_type(device_id);
	info->max_compute_units = get_max_compute_units(device_id);
	info->max_work_item_dimensions = get_max_work_item_dimensions(device_id);
	info->max_work_item_sizes = get_max_work_item_sizes(device_id);
	info->max_work_group_size = get_max_work_group_size(device_id);
	info->max_clock_frequency = get_max_clock_frequency(device_id);
	info->address_bits = get_address_bits(device_id);
	info->max_mem_alloc_size = get_max_mem_alloc_size(device_id);
	info->image_support = has_image_support(device_id);
	info->max_samplers = get_max_samplers(device_id);
	info->max_parameter_size = get_max_parameter_size(device_id);
	info->single_fp_config = get_single_fp_config(device_id);
	info->mem_cache_type = get_global_mem_cache_type(device_id);
	info->global_mem_cacheline_size = get_global_mem_cache_size(device_id);
	info->global_mem_cache_size = get_global_mem_cache_size(device_id);
	info->global_mem_size = get_global_mem_size(device_id);
	info->max_constant_buffer_size = get_max_constant_buffer_size(device_id);
	info->max_constant_args = get_max_constant_args(device_id);
	info->local_mem_type = get_local_mem_type(device_id);
	info->local_mem_size = get_local_mem_size(device_id);
	info->error_correction = has_error_correction(device_id);
	info->profiling_timer_resolution = get_profiling_timer_resolution(device_id);
	info->endian_little = is_endian_little(device_id);
	info->available = is_available(device_id);
	info->compiler_available = has_compiler_available(device_id);
	info->exec_capabilities = get_exec_capabilities(device_id);
	info->queue_properties = get_queue_properties(device_id);
	info->name = get_name(device_id);
	info->vendor = get_vendor(device_id);
	info->driver_version = get_driver_version(device_id);
	info->profile = get_profile(device_id);
	info->version = get_version(device_id);
	info->extensions = get_extensions(device_id);	
	return info;
}
void free_device_info(_DEVICE_INFO* info) {
	free(info->max_work_item_sizes);
	free(info->name);
	free(info->vendor);
	free(info->driver_version);
	free(info->profile);
	free(info->version);
	free(info->extensions);
	free(info);
	return;
}
void print_device_info(_DEVICE_INFO* info, char* prefix, char* suffix) {
	if (prefix == NULL) {
		prefix = strdup("");
	}
	if(suffix == NULL) {
		suffix = strdup("\n");
	}
	printf("%sname:                  %s%s", prefix, info->name, suffix);
	printf("%svendor:                %s%s", prefix, info->vendor, suffix);
	printf("%sdriver-version:        %s%s", prefix, info->driver_version, suffix);
	printf("%sprofile:               %s%s", prefix, info->profile, suffix);
	printf("%sversion:               %s%s", prefix, info->version, suffix);
	printf("%sextensions:            %s%s", prefix, info->extensions, suffix);
	printf("%stype:                  %s%s", prefix, get_device_type_string(info->type), suffix);
	printf("%smax compute-units:     %d%s", prefix, info->max_compute_units, suffix);
	printf("%smax work-item dims:    %d%s", prefix, info->max_work_item_dimensions, suffix);
	printf("%smax work-item sizes:   [", prefix);
	for (cl_uint i = 0; i < info->max_work_item_dimensions; i++) {
		printf("%zu", info->max_work_item_sizes[i]);
		if(i < info->max_work_item_dimensions - 1) printf(",");
	}
	printf("]%s", suffix);
	printf("%smax work-group size:   %zu%s", prefix, info->max_work_group_size, suffix);
	printf("%smax clock frequency:   %d%s", prefix, info->max_clock_frequency, suffix);
	printf("%saddress bits:          %d%s", prefix, info->address_bits, suffix);
	printf("%smax mem alloc size:    %ld%s", prefix, (unsigned long) info->max_mem_alloc_size, suffix);
	printf("%simage support:	     %s%s", prefix, info->image_support ? "yes" : "no", suffix);
	printf("%smax samplers:          %d%s", prefix, info->max_samplers, suffix);
	printf("%smax parameter size:    %u%s", prefix, (cl_uint) info->max_parameter_size, suffix);
	printf("%ssingle fp config:     ", prefix);
	if(info->single_fp_config & CL_FP_DENORM) printf(" denorm");
	if(info->single_fp_config & CL_FP_INF_NAN) printf(" inf_nan");
	if(info->single_fp_config & CL_FP_ROUND_TO_NEAREST) printf(" round_to_nearest");
	if(info->single_fp_config & CL_FP_ROUND_TO_ZERO) printf(" round_to_zero");
	if(info->single_fp_config & CL_FP_ROUND_TO_INF) printf(" round_to_inf");
	if(info->single_fp_config & CL_FP_FMA) printf(" fma");
	printf("%s", suffix);
	if(info->mem_cache_type == CL_NONE) {
		printf("%sglobal mem cache:	     none%s", prefix, suffix);		
	} else {
		printf("%sglobal mem cache:	     %s%s", prefix, info->mem_cache_type == CL_READ_ONLY_CACHE ? "read only" : "write_only", suffix);		
		printf("%sglobal mem cline size: %lu%s", prefix, info->global_mem_cacheline_size, suffix);
		printf("%sglobal mem cache size: %lu%s", prefix, (unsigned long) info->global_mem_cache_size, suffix);
	}
	printf("%sglobal mem size:       %lu%s", prefix, (unsigned long) info->global_mem_size, suffix);
	printf("%smax const buffer size: %lu%s", prefix, (unsigned long) info->max_constant_buffer_size, suffix);
	printf("%smax const args:        %u%s", prefix, info->max_constant_args, suffix);
	printf("%sdedicated local mem:   %s%s", prefix, info->local_mem_type == CL_LOCAL ? "yes" : "no", suffix);
	printf("%slocal mem size:        %lu%s", prefix, (unsigned long) info->local_mem_size, suffix);
	printf("%serror correction:	     %s%s", prefix, info->error_correction ? "yes" : "no", suffix);
	printf("%sprofiling timer res:   %zu%s", prefix, info->profiling_timer_resolution, suffix);
	printf("%sendian little:         %s%s", prefix, info->endian_little ? "yes" : "no", suffix);
	printf("%savailable:             %s%s", prefix, info->available ? "yes" : "no", suffix);
	printf("%scompiler available:    %s%s", prefix, info->compiler_available ? "yes" : "no", suffix);
	printf("%sexecution capabilities:", prefix);
	if(info->exec_capabilities & CL_EXEC_KERNEL) printf("exec_kernel ");
	if(info->exec_capabilities & CL_EXEC_NATIVE_KERNEL) printf("exec_native_kernel ");
	printf("%s", suffix);
	printf("%squeue properties:     ", prefix);
	if(info->queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) printf(" out_of_order_exec_mode");
	if(info->queue_properties & CL_QUEUE_PROFILING_ENABLE) printf(" profiling");
	printf("%s", suffix);
	return;
}

void print_short_device_info(cl_device_id id, unsigned index) {
	_DEVICE_INFO* info = get_device_info(id);
	printf("# device %i: %s // type %s (%lu MB global memory, %lu KB constant memory, %lu KB local memory)\n", 
		index, info->name, info->type == CL_DEVICE_TYPE_CPU ? "cpu" : info->type == CL_DEVICE_TYPE_GPU ? "gpu" : info->type == CL_DEVICE_TYPE_ACCELERATOR ? "accelerator" : "default", 
		(unsigned long) info->global_mem_size / 1024 /1024, (unsigned long) info->max_constant_buffer_size / 1024, (unsigned long) info->local_mem_size / 1024);
	free_device_info(info);
}

void print_csv_device_info(cl_device_id id) {
	_DEVICE_INFO* info = get_device_info(id);
	printf("#devspec: name:string, vendor:string, type:string, profile:string, version:string, "
		   "max_compute_units:unsigned, max_work_group_size:unsigned, max_work_item_dimensions:unsigned, max_work_item_size:unsigned, max_clock_frequency:unsigned, "
		   "global_mem_size:unsigned, global_mem_cache_size:unsigned, global_mem_cacheline_size:unsigned, "
		   "local_mem_size:unsigned, local_mem_type:unsigned, max_constant_buffer_size:unsigned, max_constant_args:unsigned\n");
	printf("#dev: %s,|, %s,|, %s,|, %s,|, %s,|, %u,|, %lu,|, %u,|, %lu,|, %u,|, %lu,|, %lu,|, %lu,|, %lu,|, %u,|, %lu,|, %u\n", 
		info->name, info->vendor, get_device_type_string(info->type), info->profile, info->version, 
		info->max_compute_units, info->max_work_group_size, info->max_work_item_dimensions, info->max_work_item_sizes[0], info->max_clock_frequency,
		info->global_mem_size, info->global_mem_cache_size, info->global_mem_cacheline_size,
		info->local_mem_size, info->local_mem_type, info->max_constant_buffer_size, info->max_constant_args);
		
	free_device_info(info);
}
