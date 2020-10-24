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
 *  uCLbench OpenCL Arithmetic Benchmark
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
#define BM_NAME "arith_speed"
#define BM_VERSION "1.0"

#define MAX_SIZES 50			// maximum number of operations for a single measurement
#define MAX_FUNCTIONS 50		// maximum number of functions for a single measurement
#define MAX_ERROR 1.0e-6		// maximum error allowed to pass the consistency checks
#define ARITH_OPERATIONS 100	// amount of operations each arithmetic kernel performs
#define TRANS_OPERATIONS 10     // amount of operations each trancendential kenrnel peforms

// enums
enum _FUNCTION_TYPE {_DEFAULT, _NATIVE};
enum _FUNCTION {
	_ADD, _MUL,
	_SUB, _DIV,
    _MAD, _MAD_MUL,
    _VEC, _VLIW,
	_EXP, _LOG, 
	_SQR, _SQRT, 
	_COS, _SIN, _TAN,
    _NUM_FCT
};

bool trans = true;

///////////////////////////////////////////////////////////////////////////////
//  Convert _FUNCTION to string
///////////////////////////////////////////////////////////////////////////////
const char* function_to_string(_FUNCTION function) {
	switch (function) {
		case _ADD:
			return (char*)"add";
			break;
		case _MUL:
			return (char*)"mul";
			break;
		case _SUB:
			return (char*)"sub";
			break;
		case _DIV:
			return (char*)"div";
			break;
		case _MAD:
			return (char*)"mad";
			break;
		case _MAD_MUL:
			return (char*)"mad_mul";
			break;
		case _VEC:
			return (char*)"vec";
			break;
		case _VLIW:
			return (char*)"vliw";
			break;
		case _EXP:
			return (char*)"exp";
			break;
		case _LOG:
			return (char*)"log";
			break;
		case _SQR:
			return (char*)"sqr";
			break;
		case _SQRT:
			return (char*)"sqrt";
			break;
		case _COS:
			return (char*)"cos";
			break;
		case _SIN:
			return (char*)"sin";
			break;
		case _TAN:
			return (char*)"tan";
			break;
		default:
			return (char*)"???";
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Convert _FUNCTION to source code
///////////////////////////////////////////////////////////////////////////////
const char* function_to_code(_FUNCTION function, _FUNCTION_TYPE ftype, bool &binary_op, int &ntemps) {
	binary_op = false;
    ntemps = 0;
    // Create the kernel for the given function
	switch (function) {
		case _ADD:
            binary_op = true;
            ntemps = 1;
			return (char*)"tmp = a + b; a = b; b = tmp;";
		case _SUB:
            binary_op = true;
            ntemps = 1;
			return (char*)"tmp = a - b; a = b; b = tmp;";
		case _MUL:
            binary_op = true;
            ntemps = 1;
			return (char*)"tmp = a * b; a = b; b = tmp;";
		case _DIV:
            binary_op = true;
            ntemps = 1;
			return ftype == _DEFAULT ? (char*)"tmp = a / b; a = b; b = tmp;" : 
                (char*)"tmp = native_divide(a,b); a = b; b = tmp;";
		case _MAD:
            binary_op = true;
            ntemps = 1;
			return (char*)"tmp = a * b + (%s)0.1; a = b; b = tmp;";
		case _MAD_MUL:
            binary_op = true;
            ntemps = 2;
			return (char*)"tmp = a * b + (%s)0.1; tmp2 = c * d; a = b; b = tmp; c = d; d = tmp2;";
		case _VEC:
            binary_op = true;
 			return (char*)"a%d -= b%d;;";
		case _VLIW:
            binary_op = true;
 			return (char*)"a%d %c= b%d;";
		case _EXP:
			return ftype == _DEFAULT ? (char*)"a = exp(a);" : (char*)"a = native_exp(a);";
		case _LOG:
			return ftype == _DEFAULT ? (char*)"a = log(a);" : (char*)"a = native_log(a);";
		case _SQR: 
			return ftype == _DEFAULT ? (char*)"a = powr(a, (%s)2.0);" : (char*)"a = native_powr(a, (%s)2.0);";
		case _SQRT:
			return ftype == _DEFAULT ? (char*)"a = sqrt(a);" : (char*)"a = native_sqrt(a);";
		case _COS:
			return ftype == _DEFAULT ? (char*)"a = cos(a);" : (char*)"a = native_cos(a);";
		case _SIN:
			return ftype == _DEFAULT ? (char*)"a = sin(a);" : (char*)"a = native_sin(a);";
		case _TAN:
			return ftype == _DEFAULT ? (char*)"a = tan(a);" : (char*)"a = native_tan(a);";
		default: 
			return NULL;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Convert _FUNCTION_TYPE to string
///////////////////////////////////////////////////////////////////////////////
char* ftype_to_string(_FUNCTION_TYPE ftype) {
	switch (ftype) {
		case _DEFAULT:
			return (char*)"def";
			break;
		case _NATIVE:
			return (char*)"nat";
			break;
		default: 
			return (char*)"???";
	}
}


///////////////////////////////////////////////////////////////////////////////
//  Transfer data from host to device
///////////////////////////////////////////////////////////////////////////////
template<typename T>
int transferDataToDevice(cl_context context, cl_command_queue commands, T* data, unsigned long size, cl_mem device_data) {
	int err = CL_SUCCESS;
	
	err = clEnqueueWriteBuffer(commands, device_data, CL_FALSE, 0, sizeof(T) * size, data, 0, NULL, NULL);
	clFinish(commands);
	
	return err;
}
template<typename cl_int>
int transferDataToDevice(cl_context context, cl_command_queue commands, cl_int* data, unsigned long size, cl_mem device_data);
template<typename cl_float>
int transferDataToDevice(cl_context context, cl_command_queue commands, cl_float* data, unsigned long size, cl_mem device_data);
template<typename cl_double>
int transferDataToDevice(cl_context context, cl_command_queue commands, cl_double* data, unsigned long size, cl_mem device_data);


///////////////////////////////////////////////////////////////////////////////
//  Transfer data from device to host
///////////////////////////////////////////////////////////////////////////////
template<typename T>
int transferDataFromDevice(cl_context context, cl_command_queue commands, T* data, unsigned long size, cl_mem device_data) {
	int err = CL_SUCCESS;
	
	err = clEnqueueReadBuffer(commands, device_data, CL_FALSE, 0, sizeof(T) * size, data, 0, NULL, NULL);
	clFinish(commands);

	return err;
}
template<typename cl_int>
int transferDataFromDevice(cl_context context, cl_command_queue commands, cl_int* data, unsigned long size, cl_mem device_data);
template<typename cl_float>
int transferDataFromDevice(cl_context context, cl_command_queue commands, cl_float* data, unsigned long size, cl_mem device_data);
template<typename cl_double>
int transferDataFromDevice(cl_context context, cl_command_queue commands, cl_double* data, unsigned long size, cl_mem device_data);


///////////////////////////////////////////////////////////////////////////////
//  Write kernel preamble
///////////////////////////////////////////////////////////////////////////////
static const char *fun_start = 
"__kernel __attribute__((reqd_work_group_size (%d,1,1))) void arith_speed(\n \
	            __global %s* input, \n \
	            __global %s* output, \n \
	            const unsigned int iterations) \n \
{ \n \
  unsigned int i; \n \
  unsigned int gid = get_global_id(0);\n";

template<typename T>
void writeStart(char* prog, int local);
template<>
void writeStart<cl_int>(char* prog, int local)
{
	sprintf(prog, fun_start, local, "int", "int");
}
template<>
void writeStart<cl_float>(char* prog, int local)
{
	sprintf(prog, fun_start, local, "float", "float");
}
template<>
void writeStart<cl_double>(char* prog, int local)
{
	static const char *preamble = "#ifdef cl_khr_fp64\n#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n#endif\n"
		"#ifdef cl_amd_fp64\n#pragma OPENCL EXTENSION cl_amd_fp64 : enable\n#endif\n\n";
	sprintf(prog, preamble);
	sprintf(prog+strlen(preamble), fun_start, local, "double", "double");
}

///////////////////////////////////////////////////////////////////////////////
//  Generate strings for type names
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void getTypeStr(char* type);
template<>
void getTypeStr<cl_int>(char* type)
{
    sprintf(type, "int");
}
template<>
void getTypeStr<cl_float>(char* type)
{
    sprintf(type, "float");
}
template<>
void getTypeStr<cl_double>(char* type)
{
    sprintf(type, "double");
}

template<typename T>
void getTypeStr(char* type, int vec, cl_int* err);
template<>
void getTypeStr<cl_int>(char* type, int vec, cl_int* err)
{
    if(vec != 1)
    {
        *err = -1000;
        return;
    }

    sprintf(type, "int");
}
template<>
void getTypeStr<cl_float>(char* type, int vec, cl_int* err)
{
    if(vec != 1 && vec != 2 && vec != 4 && vec != 8 && vec != 16)
    {
        *err = -1000;
        return;
    }

    if(vec == 1)
        sprintf(type, "float");
    else
        sprintf(type, "float%d", vec);
}
template<>
void getTypeStr<cl_double>(char* type, int vec, cl_int* err)
{
    if(vec != 1 && vec != 2 && vec != 4 && vec != 8 && vec != 16)
    {
        *err = -1000;
        return;
    }

    if(vec == 1)
        sprintf(type, "double");
    else
        sprintf(type, "double%d", vec);
}

///////////////////////////////////////////////////////////////////////////////
//  Generate kernel code
///////////////////////////////////////////////////////////////////////////////
template<typename T>
cl_kernel genKernel(cl_context context, cl_device_id device_id, _FUNCTION fct, _FUNCTION_TYPE ftype, int vec, int local, int unroll_factor, cl_int* err)
{
    char* kernelCode = (char*)malloc(131072*sizeof(char));
    bool binary_op = false;
    int ntemps = 0;
    size_t pos = 0;
  
    //function header
    writeStart<T>(kernelCode, local);
    pos = strlen(kernelCode);

    char type[8];
	char fctStr[512];
    int fctLen = 0;

    if(fct == _VEC || fct == _VLIW)
    {
        getTypeStr<T>(type);

        for(int i = 0; i < vec; ++i)
        {
            sprintf(kernelCode+pos, "  %s a%d = input[%d], b%d = input[%d];\n", type, i, i, i, i+vec);
            pos = strlen(kernelCode);
        }

        if(fct == _VEC)
        {
            for(int i = 0; i < vec; ++i)
            {
                sprintf(fctStr+fctLen, function_to_code(fct, ftype, binary_op, ntemps), i, i);
                fctLen = strlen(fctStr);
            }
        }
        else
            strcpy(fctStr, function_to_code(fct, ftype, binary_op, ntemps));
    }
    else
    {
        getTypeStr<T>(type, vec, err);
        if(!oclCheckErr(*err, "Invalid vector type\n"))
            return NULL;

        //private variable initialization
        sprintf(kernelCode+pos, "  %s a = (%s)(input[0]", type, type);
        pos = strlen(kernelCode);

        for(int i = 1; i < vec; ++i)
        {
            sprintf(kernelCode+pos, ", input[%d]", i);
            pos = strlen(kernelCode);
        }

	    if(fct == _SQR || fct == _MAD || fct == _MAD_MUL)
		    sprintf(fctStr, function_to_code(fct, ftype, binary_op, ntemps), type);
	    else 
		    sprintf(fctStr, function_to_code(fct, ftype, binary_op, ntemps));

        fctLen = strlen(fctStr);

		sprintf(kernelCode+pos, ");\n");
        pos += 3;

        if(binary_op)
        {
            sprintf(kernelCode+pos, "  %s b = (%s)(input[%d]", type, type, vec);
            pos = strlen(kernelCode);

            for(int i = 1; i < vec; ++i)
            {
                sprintf(kernelCode+pos, ", input[%d]", i + vec);
                pos = strlen(kernelCode);
            }

            sprintf(kernelCode+pos, ");\n  %s tmp;\n", type);
            pos = strlen(kernelCode);
            if(ntemps > 1)
            {
                sprintf(kernelCode+pos, "  %s c = (%s)(input[%d]", type, type, vec);
                pos = strlen(kernelCode);

                for(int i = 1; i < vec; ++i)
                {
                    sprintf(kernelCode+pos, ", input[%d]", i + vec);
                    pos = strlen(kernelCode);
                }

                sprintf(kernelCode+pos, ");\n  %s d = (%s)(input[0]", type, type);
                pos = strlen(kernelCode);

                for(int i = 1; i < vec; ++i)
                {
                    sprintf(kernelCode+pos, ", input[%d]", i);
                    pos = strlen(kernelCode);
                }

                sprintf(kernelCode+pos, ");\n  %s tmp2;\n", type);
                pos = strlen(kernelCode);
            }
        }
    }

    //insert loop
    sprintf(kernelCode+pos, "  for(i = 0; i < iterations; ++i) {\n");
    pos += 36;
 
    //insert operations
    if(fct == _VLIW)
    {
        char operation[4] = "+*-";
        int cnt = 0;
        for(int i = 0; i < unroll_factor; ++i)
        {
            sprintf(kernelCode+pos, "    ");
            pos += 4;
            for(int j = 0; j < vec; ++j)
            {
                sprintf(kernelCode+pos, fctStr, j, operation[cnt%3], j);
                ++cnt;
                pos = strlen(kernelCode);
            }
            sprintf(kernelCode+pos, "\n");
            pos += 1;
        }
    }
    else
    {
        for(int i = 0; i < unroll_factor; ++i)
        {
            sprintf(kernelCode+pos, "    %s\n", fctStr);
            pos += fctLen+5;
        }
    }
    sprintf(kernelCode+pos, "  }\n");
    pos += 4;

    //copy values back to global memory
    if(fct == _VEC || fct == _VLIW)
    {
        for(int i = 0; i < vec; ++i)
        {
            sprintf(kernelCode+pos, "  output[%d*gid+%d] = a%d;\n", vec, i, i);
            pos = strlen(kernelCode);
        }
    }
    else
    {
        if(vec == 1)
        {
            if(ntemps < 2)
                sprintf(kernelCode+pos, "  output[gid] = a;\n");
            else
                sprintf(kernelCode+pos, "  output[gid] = a + c;\n");
            pos = strlen(kernelCode);
        }
        else
        {
            for(int i = 0; i < vec; ++i)
            {
                if(ntemps < 2)
                    sprintf(kernelCode+pos, "  output[%d*gid+%d] = a.s%x;\n", vec, i, i);
                else
                    sprintf(kernelCode+pos, "  output[%d*gid+%d] = a.s%x + c.s%x;\n", vec, i, i, i);
                pos = strlen(kernelCode);
            }
        }
    }

    sprintf(kernelCode+pos, "}\n");
    pos += 2;
	//printf(kernelCode);

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernelCode, NULL, err);
    if(!oclCheckErr(*err, "Failed to create compute program!\n"))
        return NULL;

    // Build the program executable
    *err = clBuildProgram(program, 1, &device_id, "-cl-mad-enable -cl-unsafe-math-optimizations", NULL, NULL);
    if (*err != CL_SUCCESS)
    {
        size_t len;
        char buffer[4096];
        sprintf(buffer, "Log empty...\n");
		
        fprintf(stderr, "Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        return NULL;
    }

    cl_kernel kernel = clCreateKernel(program, "arith_speed", err);;
    if(!oclCheckErr(*err, "Failed to create kernel!\n"))
        return NULL;

	clReleaseProgram(program);
	free(kernelCode);

    return kernel;
}

///////////////////////////////////////////////////////////////////////////////
//  Run the arithmetic speed benchmark for a single function
///////////////////////////////////////////////////////////////////////////////
template<typename T>
int run_benchmark(cl_context context, cl_command_queue commands, cl_device_id device_id, cl_ulong size, _FUNCTION_TYPE ftype, _FUNCTION function, bool check_consistency, double *time_taken, size_t local, size_t global, int vec) {
	
    int err = CL_SUCCESS;               // error code returned from api calls
	
    cl_kernel kernel;                   // compute kernel
    
    cl_mem input;                       // device memory used for the input array
    cl_mem output;                      // device memory used for the output array
	cl_event event;
		
	T* data;
	T* results;

	// Initialize the data according to the given function
	data = (T*)malloc(sizeof(T) * 2 * vec);
    results = (T*) malloc(sizeof(T) * vec);
    for(int i = 0; i < vec; ++i)
    {
	    data[i] = (T)0.0;
	    data[i+vec] = (T)0.0;
	    if (function == _ADD || function == _SUB) {
		    data[i] = (T)1.0;
		    data[i+vec] = (T)1.0;
	    } else if (function == _MUL || function == _DIV || function == _MAD_MUL || function == _VEC || function == _VLIW) {
		    data[i] = (T)1.00000001;
		    data[i+vec] = (T)0.99999999;
	    } else if (function == _MAD ) {
		    data[i] = (T)1.0;
		    data[i+vec] = (T)1.0;
	    } else if (function == _TAN) {
		    data[i] = (T)0.001;
	    } else {
		    data[i] = (T)1.0;
        }
	
	    results[i] = 0.0f;
    }
		
    // Create the compute kernel 
    kernel = genKernel<T>(context, device_id, function, ftype, vec, local, trans ? TRANS_OPERATIONS : ARITH_OPERATIONS, &err);
    if (!kernel || err != CL_SUCCESS)
    {
		fprintf(stderr, "Error: Failed to create compute kernel!\n");
		free(data);
		free(results);
        return 1;
    }

    // Create the input and output arrays in device memory 
    input = clCreateBuffer(context,  CL_MEM_READ_ONLY, sizeof(T) * 2 * vec, NULL, &err);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(T) * global * vec, NULL, NULL);

    if (!input || !output || err != CL_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to allocate device memory!\n");
        return 1;
    }    
    
	// correct the size since each loop-iteration within the kernel performs KERNEL_OPERATIONS operations
	cl_ulong _size = size / (trans ? TRANS_OPERATIONS : ARITH_OPERATIONS);

    // Set the arguments to our compute kernel
    err = 0;
    err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &_size);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to set kernel arguments! %d\n", err);
        return 1;
    }
	
    // Write our data set into the input array in device memory 
	err = transferDataToDevice(context, commands, data, 2 * vec, input);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to write to source array!\n");
        return 1;
    }

	clFinish(commands);
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, &event);
	if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error %i: Failed to execute kernel!\n%s\n", err, oclErrorString(err));
        return 1;
    }
	
    // Wait for the commands to get serviced before reading back results
    err = clFinish(commands);
	if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error %i: Failed to finish kernel!\n%s\n", err, oclErrorString(err));
        return 1;
    }

	// Read back the results from the device
	err = transferDataFromDevice<T>(context, commands, results, vec, output);
	if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to read output array! %s\n", oclErrorString(err));
        return 1;
    }

	// Check consistency
	if (check_consistency) {
        T a = 0;
		switch (function) {
			case _ADD: {
				a = data[0];
				T b = data[vec];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a + b;
					a = b;
					b = tmp;
                }}
				break;
			case _MUL: {
				a = data[0];
				T b = data[vec];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a * b;
					a = b;
					b = tmp;
                }}
				break;
			case _SUB: {
				a = data[0];
				T b = data[vec];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a - b;
					a = b;
					b = tmp;
                }}
				break;
			case _DIV: {
				a = data[0];
				T b = data[vec];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a / b;
					a = b;
					b = tmp;
                }}
				break;
			case _MAD: {
				a = data[0];
				T b = data[vec];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a * b + (T)0.1;
					a = b;
					b = tmp;
                }}
                break;
			case _MAD_MUL: {
				a = data[0];
				T b = data[vec];
                T c = data[vec];
                T d = data[0];
				T tmp, tmp2;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = a * b + (T)0.1;
					a = b;
					b = tmp;
					tmp2 = c * d;
					c = d;
					d = tmp;
                }}
				break;
			case _VEC: {
				a = data[0];
				T b = data[vec];
				for (cl_ulong i = 0; i < size; i++) {
					a -= b;
                }}
                break;
			case _VLIW: {
				a = data[0];
				T b = data[vec];
                int ops = vec % 3;
                for(cl_ulong j = 0; j < _size; ++j)
                    switch (ops) {
                    case 1:
                        for (cl_ulong i = 0; i < ARITH_OPERATIONS; i++) {
                            if(i % 3 == 0) a += b;
                            if(i % 3 == 1) a *= b;
                            if(i % 3 == 2) a -= b;
                        }
                        break;
                    case 2:
                       for (cl_ulong i = 0; i < ARITH_OPERATIONS; i++) {
					       if(i % 3 == 0) a += b;
                           if(i % 3 == 1) a -= b;
                           if(i % 3 == 2) a *= b;
                       }
                       break;
                    default:
                        for (cl_ulong i = 0; i < ARITH_OPERATIONS; i++) {
					        a += b;
                        }
                        break;
                }}
                break;
			case _EXP: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = expf(a);
					a = tmp;
                }}
				break;
			case _LOG: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = logf(a);
					a = tmp;
                }}
				break;
			case _SQR: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = powf(a, 2.0f);
					a = tmp;
                }}
				break;
			case _SQRT: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = sqrtf(a);
					a = tmp;
                }}
				break;
			case _COS: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = cosf(a);
					a = tmp;
                }}
				break;
			case _SIN: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = sinf(a);
					a = tmp;
				}
				if (fabs(a - results[0]) > MAX_ERROR*5000)
				{
				printf("SIN: %f vs %f\n", a, results[0]);
				return 1;
				}}
				break;
			case _TAN: {
				a = data[0];
				T tmp;
				for (cl_ulong i = 0; i < size; i++) {
					tmp = tanf(a);
					a = tmp;
				}
				if (fabs(a - results[0]) > MAX_ERROR*100)
				{
					printf("TAN: %f vs %f\n", a, results[0]);
					return 1;
				}}
				break;
			default: {
				break;
			}
		}
    	if (fabs(a - results[0]) > MAX_ERROR && function != _SIN && function != _TAN)
		{
			printf("%f vs %f\n", a, results[0]);
			return 1;
		}
	}
	
	// return time taken 
	cl_ulong time_start, time_end;
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &time_end, NULL);
	*time_taken = (time_end - time_start) * 1.0e-9;

    // Shutdown and cleanup
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseKernel(kernel);
	clReleaseEvent(event);
	free(data);
	free(results);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  Print short program-usage
///////////////////////////////////////////////////////////////////////////////
void print_usage() {
	printf("usage: arith_speed [options]\n");
	printf("\n");
	printf("where options include:\n");
	printf(" --info     print short info on available devices\n");
	printf(" --device=<integer>\n");
	printf("            benchmark given device (integer refers to output of --info)\n");
	printf(" --operations=<integer-list>\n");
	printf("            perform benchmark with given list of operations (comma-separated)\n");
	printf(" --functions=<function-list>\n");
	printf("            perform benchmark for given function (comma-separated)\n");
	printf(" --native   use native function-implementations\n");
	printf(" --repeats=<integer>\n");
	printf("            measure given amount of times and return mean values\n");
	printf(" --nochecks don't perform consistency checks\n");
	printf(" --csv      output results as csv\n");
	printf(" --double   perform benchs of available functions in double precision\n");
	printf(" --local=<integer>\n");
	printf(" --groups=<integer>\n");
	printf(" --maxvec=<integer>\n");
}


///////////////////////////////////////////////////////////////////////////////
//  MAIN: collect input parameters and run benchmarks
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
	
    int info_flag = 0;
	int csv_flag = 0;
	int native_flag = 0;
	int check_flag = 1;
	int usage_flag = 0;
    int double_flag = 0;
	int c;
	char *sizes_str = NULL;
	char *devices_str = NULL;
	char *repeat_str = NULL;
	char *function_str = NULL;
    char *local_str = NULL;
    char *blocks_str = NULL;
	unsigned max_vec = 16;
    char buf[4];
	int err = 0;

    //size_t grid[2] = {0,0};
    size_t blocksA = 0;
    size_t localA = 0;

    cl_command_queue commands;          // compute command queue
	
	cl_context context = NULL;

	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"info", no_argument, &info_flag, 1},
			{"csv",  no_argument, &csv_flag, 1},
			{"default",  no_argument, &native_flag, 0},
			{"native",  no_argument, &native_flag, 1},
			{"nochecks",  no_argument, &check_flag, 0},
			{"checks",  no_argument, &check_flag, 1},
            {"double", no_argument, &double_flag, 1},
			{"help",  no_argument, &usage_flag, 1},
			/* These options don't set a flag.
			 We distinguish them by their indices. */
			{"operations",    optional_argument, 0, 'a'},
			{"device",  optional_argument, 0, 'b'},
			{"repeats",  optional_argument, 0, 'c'},
			{"functions",  optional_argument, 0, 'd'},
			{"local",  optional_argument, 0, 'e'},
			{"groups",  optional_argument, 0, 'f'},
			{"maxvec",  optional_argument, 0, 'g'},
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
				function_str = optarg;
				break;
			case 'e':
				local_str = optarg;
				break;
			case 'f':
				blocks_str = optarg;
				break;
			case 'g':
				max_vec = atoi(optarg);
				break;
			case '?':
				/* getopt_long already printed an error message. */
				break;
			default:
				abort ();
		}
	}
	
	
	// retrieve devices
	unsigned int num_devices = 0, num_platforms = 0;
    cl_platform_id* platforms = get_all_platforms(&num_platforms, &err);
	cl_device_id* devices = get_all_devices(platforms, num_platforms, &num_devices);
    free(platforms);

//    cl_device_id* devices = get_device_by_platform(&num_devices, "NVIDIA", CL_DEVICE_TYPE_ALL);
	if(devices == NULL) {
		fprintf(stderr, "Error: Failed to create a device group!\n");
		return 1;
	}
	
	
	if (info_flag) {
		for(unsigned int i = 0; i < num_devices; i++) {
			print_short_device_info(devices[i], i);
		}
		return 0;
	}
	
	if (usage_flag) {
		print_usage();
		return 0;
	}
	
	// retrieve devices to be benchmarked
	cl_device_id *used_devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);
	unsigned int used_num_devices = 0;
	if (!devices_str or devices_str[0] == '\0' or strcmp(devices_str, "all") == 0) {
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
				fprintf(stderr, "invalid device-number given (%d)\n", id);
				return 1;
			}
			ptr = strtok(NULL, ",");
		}
	}
	
	// retrieve sizes to run the benchmark with
	cl_ulong* sizes = (cl_ulong*) malloc(sizeof(cl_ulong) * MAX_SIZES * used_num_devices);
	for (unsigned int i = 0; i < MAX_SIZES * used_num_devices; i++) {
		sizes[i] = 0;
	}
	unsigned int num_sizes = 0;
	if (!sizes_str or sizes_str[0] == '\0') {
		// nothing specified, test for maximum
		num_sizes = 1;
		for (unsigned int i = 0; i < used_num_devices; i++) {
			sizes[i * MAX_SIZES] = 100000;
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
						fprintf(stderr, "invalid size given (%s)\n", ptr);
						return 1;
				}
			} else if (sscanf(ptr, "%lu", &_s) > 0) {
				size = _s;
			} else {
				fprintf(stderr, "invalid size given (%s)\n", ptr);
				return 1;
			}
			for (unsigned int i = 0; i < used_num_devices; i++) {
				sizes[num_sizes + i * MAX_SIZES] = size;
			}
			num_sizes ++;
			ptr = strtok(NULL, ",");
		}		
	}
	
	// retrieve functions to be tested
	unsigned int num_functions = 0;
	_FUNCTION* functions = NULL;
	if (function_str == NULL or function_str[0] == '\0') {
		functions = (_FUNCTION*) malloc(sizeof(_FUNCTION) * MAX_FUNCTIONS);
		functions[0] = _ADD;
		functions[1] = _SUB;
		functions[2] = _MUL;
		functions[3] = _DIV;
        functions[4] = _MAD;
        functions[5] = _MAD_MUL;
		functions[6] = _VEC;
		functions[7] = _VLIW;
		functions[8] = _EXP;
		functions[9] = _LOG;
		functions[10] = _SQR;
		functions[11] = _SQRT;
		functions[12] = _COS;
		functions[13] = _SIN;
		functions[14] = _TAN;
		num_functions = (int)_NUM_FCT;
	} else {
		// check the given functions and fill up the function-array
		num_functions = 0;
		functions = (_FUNCTION*) malloc(sizeof(_FUNCTION) * MAX_FUNCTIONS);
		char* ptr;
		ptr = strtok(function_str, ",");
		while(ptr != NULL) {
			if (strcmp(ptr, "exp") == 0) {
				functions[num_functions] = _EXP;
			} else if (strcmp(ptr, "log") == 0) {
					functions[num_functions] = _LOG;
			} else if (strcmp(ptr, "sqr") == 0) {
				functions[num_functions] = _SQR;
			} else if (strcmp(ptr, "sqrt") == 0) {
				functions[num_functions] = _SQRT;
			} else if (strcmp(ptr, "cos") == 0) {
				functions[num_functions] = _COS;
			} else if (strcmp(ptr, "sin") == 0) {
				functions[num_functions] = _SIN;
			} else if (strcmp(ptr, "tan") == 0) {
				functions[num_functions] = _TAN;
			} else if (strcmp(ptr, "add") == 0) {
				functions[num_functions] = _ADD;
			} else if (strcmp(ptr, "mul") == 0) {
				functions[num_functions] = _MUL;
			} else if (strcmp(ptr, "sub") == 0) {
				functions[num_functions] = _SUB;
			} else if (strcmp(ptr, "div") == 0) {
				functions[num_functions] = _DIV;
			} else if (strcmp(ptr, "mad") == 0) {
				functions[num_functions] = _MAD;
			} else if (strcmp(ptr, "mad_mul") == 0) {
				functions[num_functions] = _MAD_MUL;
			} else if (strcmp(ptr, "vec") == 0) {
				functions[num_functions] = _VEC;
			} else if (strcmp(ptr, "vliw") == 0) {
				functions[num_functions] = _VLIW;
			} else {
				fprintf(stderr, "Error: unrecognized function %s\n", ptr);
			}
			num_functions ++;
			ptr = strtok(NULL, ",");
		}
	}
	if (local_str != NULL && local_str[0] != '\0') {
        localA = atoi(local_str);
        if(localA <= 0) fprintf(stderr, "Error: invalid argument for local: %s\n", local_str);
    }
	if (blocks_str != NULL && blocks_str[0] != '\0') {
        blocksA = atoi(blocks_str);
        if(blocksA <= 0) fprintf(stderr, "Error: invalid argument for blocks: %s\n", blocks_str);
    }

	// retrieve amount of repeats for each data-point
	unsigned int repeats = 0;
	if (!repeat_str or repeat_str[0] == '\0') {
		repeats = 1;
	} else {
		if (sscanf(repeat_str, "%d", &repeats) > 0) {
			// nothing more to do
		} else {
			fprintf(stderr, "invalid number of repeats given (%s)\n", repeat_str);
			return 1;
		}
	}
	
	if(csv_flag) {
		printf("#bm: %s, %s\n", BM_NAME, BM_VERSION);
		printf("#spec: wi_count:unsigned, type:string, function:string, fun_variant:string, operations:unsigned, exec_time:double, mops_per_sec:double\n");
	}

	// now after all is defined, start the benchmark(s) and print the results
	for (unsigned int i = 0; i < used_num_devices; i++) 
    {
		if(!csv_flag) print_short_device_info(used_devices[i], i);
		else print_csv_device_info(used_devices[i]);

        // Execute the kernel using the max number of threads on each processor
    	_DEVICE_INFO* info = get_device_info(used_devices[i]);
        size_t* tmp = info->max_work_item_sizes;
        size_t maxLocal = localA > 0 && localA <= tmp[0] ? localA : tmp[0];

        size_t maxGlobal = blocksA > 0 ? blocksA : info->max_compute_units;
        maxGlobal *= maxLocal;
        free(tmp);

        // Get amount of global memory
	    cl_ulong memory = info->global_mem_size/1024;
		
		context = clCreateContext(0, 1, &used_devices[i], NULL, NULL, &err);
		if (!context) {
			fprintf(stderr, "Error: Failed to create a compute context!\n%s\n", oclErrorString(err));
			return 1;
		}
        // Create a command queue
        commands = clCreateCommandQueue(context, used_devices[i], CL_QUEUE_PROFILING_ENABLE, &err);
        if (!commands || err != CL_SUCCESS)
        {
            fprintf(stderr, "Error: Failed to create a command queue!\n%d: %s\n", err, oclErrorString(err));
            return 1;
        }
		for(unsigned int f = 0; f < num_functions; f++) {
            if((functions[f] == _ADD || functions[f] == _MUL || functions[f] == _SUB || functions[f] == _DIV || 
                functions[f] == _MAD || functions[f] == _MAD_MUL || functions[f] == _VEC || functions[f] == _VLIW))
               trans = false;
            else
                trans = true;

			// TODO refactor float/double duplication
 			for (unsigned int j = 0; j < num_sizes; j++) {
				cl_ulong size = sizes[j + i * MAX_SIZES];
                double mops = 0.0;
				int err = 0;
                for(unsigned int vec = 1; vec <= max_vec; vec *= 2)
                {
                    sprintf(buf, "%d ", vec);
                    buf[2] = '\0';
                    for(unsigned int ws = 0; ws <= 1; ++ws)
                    {
                        size_t local, global;
    				    double time = 0.0;
                        if(ws == 0)
                        {
                            // Execute the kernel using just one single work item
                            local = 1;
                            global = 1;
                        }
                        else
                        {
                            local = maxLocal;
                            global = maxGlobal;
                            while((global + 2) * sizeof(cl_float) * vec > memory)
                            {
                                global /= 2;
                                local /= 2;
                            }
                        }
 				        for(unsigned int k = 0; k < repeats; k++) {
					        double _time = 0.0;
					        err = run_benchmark<cl_float>(context, commands, used_devices[i], size, (native_flag ? _NATIVE : _DEFAULT), functions[f], 
														  (bool)check_flag, &_time, local, global, vec);
					        if(err > 0) {
						        time = NAN;
						        break;
					        }
					        time += _time;
				        }
						if(err > 0) {
							printf("# error\n");
							break;
						}
				        time /= repeats;
                        mops = global * (sizes[j + i * MAX_SIZES] / 1000000.0) / time * vec;
				        if(csv_flag)
					        printf("%8lu, float%2s,%8s,%8s,%12lu,%12.4f,%16.4f\n", global, vec > 1 ? buf : "", function_to_string(functions[f]), 
								ftype_to_string(native_flag ? _NATIVE : _DEFAULT), (unsigned long)sizes[j + i * MAX_SIZES], time, mops);
				        else
					        printf("%lu/%u %5s float%s (%s) %12lu Operations \t%f secs\t%.2f MOp/s\n", global , num_functions * num_sizes, 
								function_to_string(functions[f]), vec > 1 ? buf : "", ftype_to_string(native_flag ? _NATIVE : _DEFAULT), 
								(unsigned long)sizes[j + i * MAX_SIZES], time, mops);
						fflush(stdout);
                    }
                }

                //Double precision benchs
                if(!trans && double_flag)
                {
                    for(unsigned int vec = 1; vec <= max_vec; vec *= 2)
                    {
                        sprintf(buf, "%d ", vec);
                        buf[2] = '\0';
                        for(unsigned int ws = 0; ws <= 1; ++ws)
                        {
                            size_t local, global;
                            if(ws == 0)
                            {
                                // Execute the kernel using just one single workitem
                                local = 1;
                                global = 1;
                            }
                            else
                            {
                                local = maxLocal;
                                global = maxGlobal;
                                while((global + 2) * sizeof(cl_double) * vec > memory)
                                {
                                    global /= 2;
                                    local /= 2;
                                }
                            }
    				        double time = 0.0;
				            for(unsigned int k = 0; k < repeats; k++) {
					            double _time = 0.0;
					            err = run_benchmark<cl_double>(context, commands, used_devices[i], size, (native_flag ? _NATIVE : _DEFAULT), 
															   functions[f], check_flag, &_time, local, global, vec);
					            if(err > 0) {
						            time = NAN;
						            break;
					            }
					            time += _time;
							}
							if(err > 0) {
								printf("# error\n");
								break;
							}
				            time /= repeats;
                            mops = global * (sizes[j + i * MAX_SIZES] / 1000000.0) / time * vec;
				            if(csv_flag)
								printf("%8lu,double%2s,%8s,%8s,%12lu,%12.4f,%16.4f\n", global, vec > 1 ? buf : "", function_to_string(functions[f]), 
									ftype_to_string(native_flag ? _NATIVE : _DEFAULT), (unsigned long)sizes[j + i * MAX_SIZES], time, mops);
				            else
					            printf("%lu/%u %5s double%s (%s) %12lu Operations \t%f secs\t%.2f MOp/s\n", global , num_functions * num_sizes, 
									function_to_string(functions[f]), vec > 1 ? buf : "", ftype_to_string(native_flag ? _NATIVE : _DEFAULT), 
									(unsigned long)sizes[j + i * MAX_SIZES], time, mops);
							fflush(stdout);
                        }
                    }
                }
			}
		}
        clReleaseCommandQueue(commands);
	}
	
	// finally free up all the used memory-chunks
	clReleaseContext(context);
	free(functions);
	free(sizes);
	free(used_devices);
	free(devices);
	
	return 0;
}
