cmake_minimum_required (VERSION 2.6)
project (uclbench C CXX)

# set OpenCL paths
set(CLLIB_HEADER ../cllib/)
set(EXTERNAL_LIBS OpenCL)

include_directories($ENV{OPENCL_INCLUDE})
message("INCLUDE = " $ENV{OPENCL_INCLUDE})

link_directories($ENV{OPENCL_LIBRARY})
message("LIBRARY = " $ENV{OPENCL_LIBRARY})

# set platform-specific defines
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -O2")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIX -g -O2 -fPIC")
endif()

# setup binary directory + copy kernels to it
set(BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set(EXECUTABLE_OUTPUT_PATH  ${BIN_DIR})

file(GLOB_RECURSE kernelFiles *.cl)
foreach(kernelFile ${kernelFiles})
	file(COPY ${kernelFile} DESTINATION ${BIN_DIR})
endforeach(kernelFile)

# add individual projects
add_subdirectory(cllib)
add_subdirectory(branch_penalty)
add_subdirectory(arith_speed)
add_subdirectory(mem_latency)
add_subdirectory(buffer_bandwidth)
add_subdirectory(mem_streaming)
add_subdirectory(kernel_overheads)

