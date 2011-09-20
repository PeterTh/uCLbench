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

#ifndef OCLUTILS_H_
#define OCLUTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define shrTRUE 1

#define shrCheckError(__a, __b) { if((__a)!=(__b)) {\
	printf("shrCheckError(" #__a ", " #__b ") failed!\n  - line %d in file " __FILE__ "\n  - " #__a ": %s\n", __LINE__, oclErrorString(__a)); } }

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
    // locals 
    FILE* pFileStream = NULL;
    size_t szSourceLength;

    // open the OpenCL source code file
	pFileStream = fopen(cFilename, "rb");
	if(pFileStream == 0) return NULL;

    size_t szPreambleLength = strlen(cPreamble);

    // get the length of the source code
    fseek(pFileStream, 0, SEEK_END); 
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET); 

    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1); 
    memcpy(cSourceString, cPreamble, szPreambleLength);
    if(fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1) {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    // close the file and return the total length of the combined (preamble + source) string
    fclose(pFileStream);
    if(szFinalLength != 0) {
        *szFinalLength = szSourceLength + szPreambleLength;
    }
    cSourceString[szSourceLength + szPreambleLength] = '\0';

    return cSourceString;
}
/*
const char* oclErrorString(int err) {
    switch(err) {
		case CL_SUCCESS: break;
		case CL_DEVICE_NOT_FOUND: 	return "DEVICE NOT FOUND";
		case CL_DEVICE_NOT_AVAILABLE: 	return "DEVICE NOT AVAILABLE";
		case CL_COMPILER_NOT_AVAILABLE: 	return "COMPILER NOT AVAILABLE";
		case CL_MEM_OBJECT_ALLOCATION_FAILURE: 	return "MEM OBJECT ALLOCATION FAILURE";
		case CL_OUT_OF_RESOURCES: 	return "OUT OF RESOURCES";
		case CL_OUT_OF_HOST_MEMORY: 	return "OUT OF HOST MEMORY";
		case CL_PROFILING_INFO_NOT_AVAILABLE: 	return "PROFILING INFO NOT AVAILABLE";
		case CL_MEM_COPY_OVERLAP: 	return "MEM COPY OVERLAP";
		case CL_IMAGE_FORMAT_MISMATCH: 	return "IMAGE FORMAT MISMATCH";
		case CL_IMAGE_FORMAT_NOT_SUPPORTED: 	return "IMAGE FORMAT NOT SUPPORTED";
		case CL_BUILD_PROGRAM_FAILURE: 	return "BUILD PROGRAM FAILURE";
		case CL_MAP_FAILURE: 	return "MAP FAILURE";
		case CL_INVALID_DEVICE_TYPE: 	return "INVALID DEVICE TYPE";
		case CL_INVALID_PLATFORM: 	return "INVALID PLATFORM";
		case CL_INVALID_DEVICE: 	return "INVALID DEVICE";
		case CL_INVALID_CONTEXT: 	return "INVALID CONTEXT";
		case CL_INVALID_HOST_PTR: 	return "INVALID HOST PTR";
		case CL_INVALID_MEM_OBJECT: 	return "INVALID MEM OBJECT";
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: 	return "INVALID IMAGE FORMAT DESCRIPTOR";
		case CL_INVALID_IMAGE_SIZE: 	return "INVALID IMAGE SIZE";
		case CL_INVALID_SAMPLER: 	return "INVALID SAMPLER";
		case CL_INVALID_BINARY: 	return "INVALID BINARY";
		case CL_INVALID_BUILD_OPTIONS: 	return "INVALID BUILD OPTIONS";
		case CL_INVALID_PROGRAM: 	return "INVALID PROGRAM";
		case CL_INVALID_PROGRAM_EXECUTABLE: 	return "INVALID PROGRAM EXECUTABLE";
		case CL_INVALID_KERNEL_NAME: 	return "INVALID KERNEL NAME";
		case CL_INVALID_KERNEL_DEFINITION: 	return "INVALID KERNEL DEFINITION";
		case CL_INVALID_KERNEL: 	return "INVALID KERNEL";
		case CL_INVALID_ARG_INDEX: 	return "INVALID ARG INDEX";
		case CL_INVALID_ARG_VALUE: 	return "INVALID ARG VALUE";
		case CL_INVALID_ARG_SIZE: 	return "INVALID ARG SIZE";
		case CL_INVALID_KERNEL_ARGS: 	return "INVALID KERNEL ARGS";
		case CL_INVALID_WORK_DIMENSION: 	return "INVALID WORK DIMENSION";
		case CL_INVALID_WORK_GROUP_SIZE: 	return "INVALID WORK GROUP SIZE";
		case CL_INVALID_GLOBAL_OFFSET: 	return "INVALID GLOBAL OFFSET";
		case CL_INVALID_EVENT_WAIT_LIST: 	return "INVALID EVENT WAIT LIST";
		case CL_INVALID_EVENT: 	return "INVALID EVENT";
		case CL_INVALID_OPERATION: 	return "INVALID OPERATION";
		case CL_INVALID_GL_OBJECT: 	return "INVALID GL OBJECT";
		case CL_INVALID_BUFFER_SIZE: 	return "INVALID BUFFER SIZE";
		case CL_INVALID_MIP_LEVEL: 	return "INVALID MIP LEVEL";
		case CL_INVALID_VALUE: 	return "INVALID_VALUE";
		case CL_INVALID_QUEUE_PROPERTIES: 	return "INVALID_QUEUE_PROPERTIES";
		case CL_INVALID_COMMAND_QUEUE: 	return "INVALID_COMMAND_QUEUE";
		default: return "Unexpected error has occured";
	}
}
*/
#endif // OCLUTILS_H_
