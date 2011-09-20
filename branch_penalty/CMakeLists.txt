add_executable(branch_penalty branch_penalty.cpp)
include_directories(${CLLIB_HEADER})
target_link_libraries(branch_penalty ${EXTERNAL_LIBS} cllib)
