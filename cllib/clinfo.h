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

#ifndef OCLINFO_H
#define OCLINFO_H

#include <stdlib.h>
#include <stdio.h>

#include "CL/cl.h"

typedef struct _platform_info {
	cl_platform_id id;
	char *profile;
	char *version;
	char *name;
	char *vendor;
	char *extensions;
} _PLATFORM_INFO;

typedef struct _device_info {
	cl_device_id id;
	cl_device_type type;
	cl_uint max_compute_units;
	cl_uint max_work_item_dimensions;
	size_t* max_work_item_sizes;
	size_t max_work_group_size;

	cl_uint max_clock_frequency;
	cl_uint address_bits;

	cl_ulong max_mem_alloc_size;
	cl_bool image_support;
	cl_uint max_samplers;
	size_t max_parameter_size;
	cl_device_fp_config single_fp_config;
	cl_device_mem_cache_type mem_cache_type;

	cl_ulong global_mem_cacheline_size;
	cl_ulong global_mem_cache_size;
	cl_ulong global_mem_size;
	cl_ulong max_constant_buffer_size;
	cl_uint max_constant_args;

	cl_device_local_mem_type local_mem_type;
	cl_ulong local_mem_size;
	cl_bool error_correction;

	size_t profiling_timer_resolution;
	cl_bool endian_little;
	cl_bool available;

	cl_bool compiler_available;
	cl_device_exec_capabilities exec_capabilities;
	cl_command_queue_properties queue_properties;

	char *name;
	char *vendor;
	char *driver_version;
	char *profile;
	char *version;
	char *extensions;	
} _DEVICE_INFO;

char* get_plat_profile(cl_platform_id);
char* get_plat_version(cl_platform_id);
char* get_plat_name(cl_platform_id);
char* get_plat_vendor(cl_platform_id);
char* get_plat_extensions(cl_platform_id);
_PLATFORM_INFO* get_platform_info(cl_platform_id);
void print_platform_info(_PLATFORM_INFO*, char*, char*);
void free_platform_info(_PLATFORM_INFO*);


cl_device_type get_type(cl_device_id);
const char* get_device_type_string(cl_device_type type);
cl_uint get_max_compute_units(cl_device_id);
cl_uint get_max_work_item_dimensions(cl_device_id);
size_t* get_max_work_item_sizes(cl_device_id);
size_t get_max_work_group_size(cl_device_id);

cl_uint get_max_clock_frequency(cl_device_id);
cl_uint get_address_bits(cl_device_id);

cl_ulong get_max_mem_alloc_size(cl_device_id);
cl_bool has_image_support(cl_device_id);
cl_uint get_max_samplers(cl_device_id);
size_t get_max_parameter_size(cl_device_id);
cl_device_fp_config get_single_fp_config(cl_device_id);
cl_device_mem_cache_type get_global_mem_cache_type(cl_device_id);

cl_uint get_global_mem_cacheline_size(cl_device_id);
cl_ulong get_global_mem_cache_size(cl_device_id);
cl_ulong get_global_mem_size(cl_device_id);
cl_ulong get_max_constant_buffer_size(cl_device_id);
cl_uint get_max_constant_args(cl_device_id);

cl_device_local_mem_type get_local_mem_type(cl_device_id);
cl_ulong get_local_mem_size(cl_device_id);
cl_bool has_error_correction(cl_device_id);

size_t get_profiling_timer_resolution(cl_device_id);
cl_bool is_endian_little(cl_device_id);
cl_bool is_available(cl_device_id);

cl_bool has_compiler_available(cl_device_id);
cl_device_exec_capabilities get_exec_capabilities(cl_device_id);
cl_command_queue_properties get_queue_properties(cl_device_id);

char* get_name(cl_device_id);
char* get_vendor(cl_device_id);
char* get_driver_version(cl_device_id);
char* get_profile(cl_device_id);
char* get_version(cl_device_id);
char* get_extensions(cl_device_id);

_DEVICE_INFO* get_device_info(cl_device_id);
void print_device_info(_DEVICE_INFO*, char*, char*);
void print_short_device_info(cl_device_id id, unsigned index);
void print_csv_device_info(cl_device_id id);
void free_device_info(_DEVICE_INFO*);

void print_device_list(cl_device_id* devices, int size);

#endif /* OCLINFO_H */
