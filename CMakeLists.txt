cmake_minimum_required(VERSION 2.9)
project(raspifb16 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#--------------------------------------------------------------------------

find_package(Freetype)

#--------------------------------------------------------------------------

add_library(raspifb16 STATIC libraspifb16/fileDescriptor.cxx
							 libraspifb16/framebuffer565.cxx
							 libraspifb16/image565.cxx
							 libraspifb16/image565Font8x16.cxx
							 libraspifb16/image565Graphics.cxx
							 libraspifb16/image565Qoi.cxx
							 libraspifb16/interface565.cxx
							 libraspifb16/interface565Font.cxx
							 libraspifb16/joystick.cxx
							 libraspifb16/rgb565.cxx)

if (FREETYPE_FOUND)
target_include_directories(raspifb16 PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_sources(raspifb16 PRIVATE libraspifb16/image565FreeType.cxx)
endif()

include_directories(${PROJECT_SOURCE_DIR}/libraspifb16)

set(EXTRA_LIBS ${EXTRA_LIBS} raspifb16)

#--------------------------------------------------------------------------

add_executable(boxworld boxworld/main.cxx
                        boxworld/images.cxx
                        boxworld/level.cxx
                        boxworld/levels.cxx
                        boxworld/boxworld.cxx)

target_link_libraries(boxworld raspifb16)

#--------------------------------------------------------------------------

add_executable(fbpipe fbpipe/fbpipe.cxx)
target_link_libraries(fbpipe raspifb16)

#--------------------------------------------------------------------------

add_executable(puzzle-15 puzzle-15/main.cxx
                         puzzle-15/images.cxx
                         puzzle-15/puzzle.cxx)

target_link_libraries(puzzle-15 raspifb16)

#--------------------------------------------------------------------------

add_executable(raspinfo raspinfo/raspinfo.cxx
						raspinfo/cpuTrace.cxx
						raspinfo/dynamicInfo.cxx
						raspinfo/memoryTrace.cxx
						raspinfo/networkTrace.cxx
						raspinfo/panel.cxx
						raspinfo/system.cxx
						raspinfo/temperatureTrace.cxx
						raspinfo/trace.cxx
						raspinfo/traceGraph.cxx
						raspinfo/traceStack.cxx)

target_link_libraries(raspinfo raspifb16
							   -lbsd)

set_property(TARGET raspinfo PROPERTY SKIP_BUILD_RPATH TRUE)
install (TARGETS raspinfo RUNTIME DESTINATION bin)

#--------------------------------------------------------------------------

add_executable(splash splash/splash.cxx)
target_link_libraries(splash raspifb16)

#--------------------------------------------------------------------------

add_executable(test test/test.cxx)
target_link_libraries(test raspifb16)

#--------------------------------------------------------------------------

add_executable(testColour test/testColour.cxx)
target_link_libraries(testColour raspifb16)

#--------------------------------------------------------------------------

if (FREETYPE_FOUND)

add_executable(testft test/testft.cxx)
target_link_libraries(testft raspifb16
                             ${FREETYPE_LIBRARIES})

add_executable(testft2 test/testft2.cxx)
target_link_libraries(testft2 raspifb16
                             ${FREETYPE_LIBRARIES})

endif()

