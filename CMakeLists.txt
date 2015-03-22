cmake_minimum_required(VERSION 2.6)
project(fb16)

set(COMPILE_DEFINITIONS -Wall -g -O3)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-std=c++0x")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#--------------------------------------------------------------------------

add_library(fb16 STATIC libfb16/font.cxx
						libfb16/image565.cxx
						libfb16/framebuffer565.cxx
						libfb16/rgb565.cxx)

include_directories(${PROJECT_SOURCE_DIR}/libfb16)
include_directories(/opt/vc/include )
include_directories(/opt/vc/include/interface/vcos/pthreads)
include_directories(/opt/vc/include/interface/vmcs_host/linux)

set(EXTRA_LIBS ${EXTRA_LIBS} fb16)

#--------------------------------------------------------------------------

add_executable(raspinfo raspinfo/raspinfo.cxx
						raspinfo/cpuTrace.cxx
						raspinfo/dynamicInfo.cxx
						raspinfo/memoryTrace.cxx
						raspinfo/temperatureTrace.cxx)
target_link_libraries(raspinfo ${PROJECT_BINARY_DIR}/lib/libfb16.a
							   /opt/vc/lib/libbcm_host.so
							   -lbsd)

set_property(TARGET raspinfo PROPERTY SKIP_BUILD_RPATH TRUE)
install (TARGETS raspinfo RUNTIME DESTINATION bin)

#--------------------------------------------------------------------------

add_executable(test test/test.cxx)
target_link_libraries(test ${PROJECT_BINARY_DIR}/lib/libfb16.a)

