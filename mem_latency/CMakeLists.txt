add_executable(mem_latency mem_latency.cpp)
include_directories(${CLLIB_HEADER})
target_link_libraries(mem_latency ${EXTERNAL_LIBS} cllib)
