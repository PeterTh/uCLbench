add_executable(mem_streaming mem_streaming.cpp)
include_directories(${CLLIB_HEADER})
target_link_libraries(mem_streaming ${EXTERNAL_LIBS} cllib)
