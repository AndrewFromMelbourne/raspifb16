cmake_minimum_required(VERSION 2.6)
project(raspifb16)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#--------------------------------------------------------------------------

add_library(raspifb16 STATIC libraspifb16/image565.cxx
							 libraspifb16/image565Font.cxx
							 libraspifb16/image565Graphics.cxx
							 libraspifb16/framebuffer565.cxx
							 libraspifb16/rgb565.cxx)

include_directories(${PROJECT_SOURCE_DIR}/libraspifb16)
include_directories(/opt/vc/include )
include_directories(/opt/vc/include/interface/vcos/pthreads)
include_directories(/opt/vc/include/interface/vmcs_host/linux)

set(EXTRA_LIBS ${EXTRA_LIBS} raspifb16)

#--------------------------------------------------------------------------

add_executable(raspinfo raspinfo/raspinfo.cxx
						raspinfo/cpuTrace.cxx
						raspinfo/dynamicInfo.cxx
						raspinfo/memoryTrace.cxx
						raspinfo/system.cxx
						raspinfo/temperatureTrace.cxx
						raspinfo/trace.cxx)

target_link_libraries(raspinfo ${PROJECT_BINARY_DIR}/lib/libraspifb16.a
							   /opt/vc/lib/libbcm_host.so
							   -lbsd)

set_property(TARGET raspinfo PROPERTY SKIP_BUILD_RPATH TRUE)
install (TARGETS raspinfo RUNTIME DESTINATION bin)

#--------------------------------------------------------------------------

add_executable(raspifb16test test/test.cxx)
target_link_libraries(raspifb16test ${PROJECT_BINARY_DIR}/lib/libraspifb16.a)

