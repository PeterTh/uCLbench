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

#ifndef CL_COMMON_H
#define CL_COMMON_H

//log to stdout
#define shrLog printf

#ifndef UNIX
	#include <stdio.h>
	#define PATH_MAX 255
	#define snprintf _snprintf
	#define no_argument             0
	#define required_argument       1
	#define optional_argument       2
	#define NAN 0xffffffff
	
	struct option {
	  const char* name;
	  int has_arg;
	  int* flag;
	  int val;
	};
	

	extern char *optarg;
	extern int optind, opterr, optopt;
	int getopt_long(int argc, char *const *argv,
           const char *shortopts, const struct option *longopts,
           int *longind);
	char *dirname(char *path);	   
#else
	#define _strdup strdup
	#define _snprintf snprintf
	#include <stdio.h>
	#include <sys/mman.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <getopt.h>
	#include <libgen.h>
	#include <limits.h>
	#include <string.h>
#endif

#include "CL/opencl.hpp"

bool oclCheckErr(int err, const char* errorMessage);

char* oclErrorString(int err);

cl_program load_kernel(const char *path, cl_context context,  const char* preamble);

cl_platform_id* get_all_platforms(unsigned int* num_platforms, cl_int* err);

cl_device_id* get_all_devices(cl_platform_id* platforms, unsigned int num_platforms, unsigned int* num_devices);

cl_device_id* get_device_by_platform(unsigned int* num_devices, const char* platformName, const cl_device_type type);
    
cl_device_id* get_device_by_platform(unsigned int* num_devices, int call);

// timing
#ifndef UNIX
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#include <windows.h>  

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
int gettimeofday(struct timeval *tv, struct timezone *tz);

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#endif

unsigned long current_msecs();
unsigned long elapsed_msecs(unsigned long start_msecs);

#endif /* CL_COMMON_H */
