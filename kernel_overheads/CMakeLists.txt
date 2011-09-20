add_executable(kernel_overheads ./kernel_overheads.cpp)
include_directories(${CLLIB_HEADER})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(kernel_overheads ${EXTERNAL_LIBS} cllib)
else()
	target_link_libraries(kernel_overheads ${EXTERNAL_LIBS} cllib m)
endif()
