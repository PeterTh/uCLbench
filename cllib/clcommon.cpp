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

#include "clcommon.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef UNIX
#include <unistd.h>
#endif

cl_program load_kernel(const char *path, cl_context context, const char* preamble)
{
    size_t szPreambleLength = strlen(preamble);
#ifndef UNIX
	FILE *f = NULL;
	char *fbuf;
	int err = 0;
	cl_program p;
	

	if(fopen_s(&f, path, "rb") != 0)
    {
	    fprintf(stderr, "Flile %s not found\n", path);
        return NULL;
    }
		
    // get the length of the source code
    fseek(f, 0, SEEK_END); 
    long szSourceLength = ftell(f);
    fseek(f, 0, SEEK_SET); 

    // allocate a buffer for the source code string and read it in
    fbuf = (char *)malloc(szSourceLength + szPreambleLength + 1); 
    memcpy(fbuf, preamble, szPreambleLength);
    if(fread((fbuf) + szPreambleLength, szSourceLength, 1, f) != 1) {
        fclose(f);
        free(fbuf);
        return 0;
    }

    // close the file and return the total length of the combined (preamble + source) string
    fclose(f);

    fbuf[szSourceLength + szPreambleLength] = '\0';

	p = clCreateProgramWithSource(context, 1, (const char **)&fbuf, NULL, &err);
	if(!p || err != CL_SUCCESS){
		fprintf(stderr, "clCreateProgramWithSource() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		free(fbuf);
		return NULL;
	}
	
	free(fbuf);
	return p;
#else
	char *fbuf;
	char *mm;
	struct stat st;
	int fd;
	int err = 0;
	cl_program p;
	
	fd = open(path, O_RDONLY);
	if(fd < 0)
		return NULL;

	memset(&st, 0, sizeof(struct stat));
	fstat(fd, &st);

	mm = (char *)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    size_t szSourceLength = strlen(mm);
    
    fbuf = (char *)malloc(szPreambleLength + szSourceLength + 1);

    sprintf(fbuf, "%s%s", preamble, mm);

	p = clCreateProgramWithSource(context, 1, (const char **)&fbuf, NULL, &err);
	if(!p){
		fprintf(stderr, "clCreateProgramWithSource() failed!\n");
		fprintf(stderr, "err: %i\n", err);
		munmap(mm, st.st_size);
        free(fbuf);
		return NULL;
	}

	close(fd);
	munmap(mm, st.st_size);
    free(fbuf);

	return p;
#endif
}

#ifndef UNIX
//getopt_long() and dirname() are take from http://www.fefe.de/dietlibc/

int opterr=1;
int optopt;
int optind=1;
char *optarg;

int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
  static int lastidx,lastofs;
  char *tmp;
  if (optind==0) { optind=1; lastidx=0; }	/* whoever started setting optind to 0 should be shot */
again:
  if (*optstring == '-' && optind<argc && *argv[optind]!='-') {
    optarg=argv[optind];
    ++optind;
    return 1;
  }
  if (optind>=argc || !argv[optind] || *argv[optind]!='-' || argv[optind][1]==0)
    return -1;
  if (argv[optind][1]=='-' && argv[optind][2]==0) {
    ++optind;
    return -1;
  }
  if (argv[optind][1]=='-') {	/* long option */
    char* arg=argv[optind]+2;
    char* max=strchr(arg,'=');
    const struct option* o;
    const struct option* match=0;
    if (!max) max=arg+strlen(arg);
    for (o=longopts; o->name; ++o) {
      size_t tlen=max-arg;
      if (!strncmp(o->name,arg,tlen)) {	/* match */
	if (strlen(o->name)==tlen) {
	  match=o;	/* perfect match, not just prefix */
	  break;
	}
	if (!match)
	  match=o;
	else
	  /* Another imperfect match. */
	  match=(struct option*)-1;
      }
    }
    if (match!=(struct option*)-1 && (o=match)) {
      if (longindex) *longindex=o-longopts;
      if (o->has_arg>0) {
	if (*max=='=')
	  optarg=max+1;
	else {
	  optarg=argv[optind+1];
	  if (!optarg && o->has_arg==1) {	/* no argument there */
	    if (*optstring==':') return ':';
	    ++optind;
	    return '?';
	  }
	  ++optind;
	}
      }
      ++optind;
      if (o->flag)
	*(o->flag)=o->val;
      else
	return o->val;
      return 0;
    }
    if (*optstring==':') return ':';
    ++optind;
    return '?';
  }
  if (lastidx!=optind) {
    lastidx=optind; lastofs=0;
  }
  optopt=argv[optind][lastofs+1];
  if ((tmp=(char *)strchr(optstring,optopt))) {
    if (*tmp==0) {	/* apparently, we looked for \0, i.e. end of argument */
      ++optind;
      goto again;
    }
    if (tmp[1]==':') {	/* argument expected */
      if (tmp[2]==':' || argv[optind][lastofs+2]) {	/* "-foo", return "oo" as optarg */
	if (!*(optarg=argv[optind]+lastofs+2)) optarg=0;
	goto found;
      }
      optarg=argv[optind+1];
      if (!optarg) {	/* missing argument */
	++optind;
	if (*optstring==':') return ':';
	//getopterror(1);
	return ':';
      }
      ++optind;
    } else {
      ++lastofs;
      return optopt;
    }
found:
    ++optind;
    return optopt;
  } else {	/* not found */
    //getopterror(0);
    ++optind;
    return '?';
  }
}

static char *dot=".";
#define SLASH '/'
#define EOL (char)0
char *dirname(char *path)
{
  char *c;
  if ( path  == NULL ) return dot;
  for(;;) {
    if ( !(c=strrchr(path,SLASH)) ) return dot; /* no slashes */
    if ( c[1]==EOL && c!=path ) {   /* remove trailing slashes */
      while ( *c==SLASH && c!=path ) *c--=EOL;
      continue;
    }
    if ( c!=path )
      while ( *c==SLASH && c>=path) *c--=EOL; /* slashes in the middle */
    else
      path[1]=EOL;                  /* slash is first symbol */
    return path;
  }
}
#endif

bool oclCheckErr(int err, const char* errorMessage)
{
    if(err != CL_SUCCESS)
    {
        fprintf(stderr, "ERROR %d: %s\n%s\n", err, oclErrorString(err), errorMessage);
        return false;
    }
    return true;
}


char* oclErrorString(int err)
{
    switch(err)
    {
  case CL_SUCCESS:
    break;
  case CL_DEVICE_NOT_FOUND:
    return "DEVICE NOT FOUND";
    break;
  case CL_DEVICE_NOT_AVAILABLE:
    return "DEVICE NOT AVAILABLE";
    break;
  case CL_COMPILER_NOT_AVAILABLE:
    return "COMPILER NOT AVAILABLE";
    break;
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:
    return "MEM OBJECT ALLOCATION FAILURE";
    break;
  case CL_OUT_OF_RESOURCES:
    return "OUT OF RESOURCES";
    break;
  case CL_OUT_OF_HOST_MEMORY:
    return "OUT OF HOST MEMORY";
    break;
  case CL_PROFILING_INFO_NOT_AVAILABLE:
    return "PROFILING INFO NOT AVAILABLE";
    break;
  case CL_MEM_COPY_OVERLAP:
    return "MEM COPY OVERLAP";
    break;
  case CL_IMAGE_FORMAT_MISMATCH:
    return "IMAGE FORMAT MISMATCH";
    break;
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    return "IMAGE FORMAT NOT SUPPORTED";
    break;
  case CL_BUILD_PROGRAM_FAILURE:
    return "BUILD PROGRAM FAILURE";
    break;
  case CL_MAP_FAILURE:
    return "MAP FAILURE";
    break;
  case CL_INVALID_DEVICE_TYPE:
    return "INVALID DEVICE TYPE";
    break;
  case CL_INVALID_PLATFORM:
    return "INVALID PLATFORM";
    break;
  case CL_INVALID_DEVICE:
    return "INVALID DEVICE";
    break;
  case CL_INVALID_CONTEXT:
    return "INVALID CONTEXT";
    break;
  case CL_INVALID_HOST_PTR:
    return "INVALID HOST PTR";
    break;
  case CL_INVALID_MEM_OBJECT:
    return "INVALID MEM OBJECT";
    break;
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    return "INVALID IMAGE FORMAT DESCRIPTOR";
    break;
  case CL_INVALID_IMAGE_SIZE:
    return "INVALID IMAGE SIZE";
    break;
  case CL_INVALID_SAMPLER:
    return "INVALID SAMPLER";
    break;
  case CL_INVALID_BINARY:
    return "INVALID BINARY";
    break;
  case CL_INVALID_BUILD_OPTIONS:
    return "INVALID BUILD OPTIONS";
    break;
  case CL_INVALID_PROGRAM:
    return "INVALID PROGRAM";
    break;
  case CL_INVALID_PROGRAM_EXECUTABLE:
    return "INVALID PROGRAM EXECUTABLE";
    break;
  case CL_INVALID_KERNEL_NAME:
    return "INVALID KERNEL NAME";
    break;
  case CL_INVALID_KERNEL_DEFINITION:
    return "INVALID KERNEL DEFINITION";
    break;
  case CL_INVALID_KERNEL:
    return "INVALID KERNEL";
    break;
  case CL_INVALID_ARG_INDEX:
    return "INVALID ARG INDEX";
    break;
  case CL_INVALID_ARG_VALUE:
    return "INVALID ARG VALUE";
    break;
  case CL_INVALID_ARG_SIZE:
    return "INVALID ARG SIZE";
    break;
  case CL_INVALID_KERNEL_ARGS:
    return "INVALID KERNEL ARGS";
    break;
  case CL_INVALID_WORK_DIMENSION:
    return "INVALID WORK DIMENSION";
    break;
  case CL_INVALID_WORK_GROUP_SIZE:
    return "INVALID WORK GROUP SIZE";
    break;
  case CL_INVALID_GLOBAL_OFFSET:
    return "INVALID GLOBAL OFFSET";
    break;
  case CL_INVALID_EVENT_WAIT_LIST:
    return "INVALID EVENT WAIT LIST";
    break;
  case CL_INVALID_EVENT:
    return "INVALID EVENT";
    break;
  case CL_INVALID_OPERATION:
    return "INVALID OPERATION";
    break;
  case CL_INVALID_GL_OBJECT:
    return "INVALID GL OBJECT";
    break;
  case CL_INVALID_BUFFER_SIZE:
    return "INVALID BUFFER SIZE";
    break;
  case CL_INVALID_MIP_LEVEL:
    return "INVALID MIP LEVEL";
    break;
  case CL_INVALID_VALUE:
    return "INVALID_VALUE";
    break;
  case CL_INVALID_QUEUE_PROPERTIES:
    return "INVALID_QUEUE_PROPERTIES";
    break;
  case CL_INVALID_COMMAND_QUEUE:
    return "INVALID_COMMAND_QUEUE";
    break;
  }
  return "Unexpected error has occured";
}

cl_platform_id* get_all_platforms(unsigned int* num_platforms, cl_int* err)
{
    cl_uint num_pf; 
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs(0, NULL, &num_pf);

    if (ciErrNum != CL_SUCCESS)
    {
        shrLog(" Error %i in clGetPlatformIDs Call !\n\n", ciErrNum);
        if(err != 0) *err = -1000;
        return 0;
    }
    if(num_pf == 0)
    {
        shrLog("No OpenCL platform found!\n\n");
        if(err != 0) *err = -2000;
        return 0;
    }
    // if there's a platform or more, make space for ID's
    if ((clPlatformIDs = (cl_platform_id*)malloc(num_pf * sizeof(cl_platform_id))) == NULL)
    {
        shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
        if(err != 0) *err = -3000;
        return 0;
    }

    // get platform info for each platform 
    ciErrNum = clGetPlatformIDs(num_pf, clPlatformIDs, NULL);
    num_platforms[0] = num_pf;

    return clPlatformIDs;
}

cl_device_id* get_all_devices(cl_platform_id* platforms, unsigned int num_platforms, unsigned int* num_devices)
{
	unsigned int* num_devs = (unsigned int*)malloc(num_platforms * sizeof(unsigned int));
    unsigned int offset = 0;
    *num_devices = 0;

    for(unsigned int i = 0; i < num_platforms; ++i)
    {
	    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devs[i]);
        num_devices[0] += num_devs[i];
    }

    cl_device_id* devs = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices[0]);

    for(unsigned int i = 0; i < num_platforms; ++i)
    {
	    int err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devs[i], devs+offset, NULL);

        if (err != CL_SUCCESS) {
		    num_devices[0] -= num_devs[i];
    	} else {
	    	offset += num_devs[i];
        }
    }

	return devs;
}

cl_int myGetPlatformID(cl_platform_id* clSelectedPlatformID, const char* platformName)
{
    char chBuffer[1024];
    cl_uint num_platforms; 
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    *clSelectedPlatformID = NULL;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        shrLog(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        return -1000;
    }
    else 
    {
        if(num_platforms == 0)
        {
            shrLog("No OpenCL platform found!\n\n");
            return -2000;
        }
        else 
        {
            // if there's a platform or more, make space for ID's
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
                return -3000;
            }

            // get platform info for each platform and trap the desired platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            for(cl_uint i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    if(strstr(chBuffer, platformName) != NULL)
                    {
                        *clSelectedPlatformID = clPlatformIDs[i];
                        break;
                    }
                }
            }

            // default error if platform not found
            if(*clSelectedPlatformID == NULL)
            {
                fprintf(stderr, "Error: Platform %s not found!\n", platformName);
                return CL_DEVICE_NOT_FOUND;
//                *clSelectedPlatformID = clPlatformIDs[0];
            }

            free(clPlatformIDs);
        }
    }

    return CL_SUCCESS;
}

cl_device_id* get_device_by_platform(unsigned int* num_devices, const char* platformName, const cl_device_type type)
{
    cl_platform_id cpPlatform = NULL;
	unsigned int num_devs = 0;
    int err = CL_SUCCESS;

    //Get the NVIDIA platform
    err = myGetPlatformID(&cpPlatform, platformName);
    if (err != CL_SUCCESS)
    {
        printf("Failed to get platform:\n%s\n", oclErrorString(err));
        return NULL;
    }
	err = clGetDeviceIDs(cpPlatform, type, 0, NULL, &num_devs);
    if (err != CL_SUCCESS)
    {
        printf("Failed to get number of devices: %u\n%s\n", *num_devices, oclErrorString(err));
        return NULL;
    }
	cl_device_id* devs = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devs);

	err = clGetDeviceIDs(cpPlatform, type, num_devs, devs, NULL);
    if (err != CL_SUCCESS) {
        printf("Failed to get device:%u\n%s\n", *num_devices, oclErrorString(err));
		*num_devices = 0;
		return NULL;
	} else {
		*num_devices = num_devs;
		return devs;
	}
}

cl_device_id* get_device_by_platform(unsigned int* num_devices, int call)
{
    switch(call)
    {
        case 1: return get_device_by_platform(num_devices, "ATI", CL_DEVICE_TYPE_CPU); break;
        case 2: return  get_device_by_platform(num_devices, "ATI", CL_DEVICE_TYPE_GPU); break;
        default: return  get_device_by_platform(num_devices, "NVIDIA", CL_DEVICE_TYPE_GPU);
    }
}

// timing

#ifndef UNIX
// gettimeofday() for windows
// taken from: http://www.cpp-programming.net/c-tidbits/gettimeofday-function-for-windows/

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  return 0;
}
#endif

unsigned long current_msecs() 
{
	struct timeval current;
	gettimeofday(&current, NULL);
	return current.tv_sec * 1000 + current.tv_usec / 1000;
}

unsigned long elapsed_msecs(unsigned long start_msecs) 
{
	return current_msecs() - start_msecs;
}

