add_executable(buffer_bandwidth buffer_bandwidth.cpp)
include_directories(${CLLIB_HEADER})
target_link_libraries(buffer_bandwidth ${EXTERNAL_LIBS} cllib)
