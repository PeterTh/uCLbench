add_executable(arith_speed arith_speed.cpp)
include_directories(${CLLIB_HEADER})
target_link_libraries(arith_speed ${EXTERNAL_LIBS} cllib)
