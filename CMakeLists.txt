cmake_minimum_required(VERSION 3.10)
project(raspifb16 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if (CMAKE_BUILD_TYPE STREQUAL "")
set(CMAKE_BUILD_TYPE Release)
endif()

message("Build type ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-psabi")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

#--------------------------------------------------------------------------

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak,undefined")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,leak,undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,leak,undefined")
endif()

#--------------------------------------------------------------------------

find_package(Freetype)
find_package(Libiw)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DRM QUIET libdrm)

#--------------------------------------------------------------------------

add_library(raspifb16 STATIC libraspifb16/fileDescriptor.cxx
                             libraspifb16/framebuffer565.cxx
                             libraspifb16/image565.cxx
                             libraspifb16/image565Font8x8.cxx
                             libraspifb16/image565Font8x16.cxx
                             libraspifb16/image565Graphics.cxx
                             libraspifb16/image565Qoi.cxx
                             libraspifb16/interface565.cxx
                             libraspifb16/interface565Factory.cxx
                             libraspifb16/interface565Font.cxx
                             libraspifb16/joystick.cxx
                             libraspifb16/rgb565.cxx)

if (FREETYPE_FOUND)
target_include_directories(raspifb16 PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_sources(raspifb16 PRIVATE libraspifb16/image565FreeType.cxx
                                 libraspifb16/dumbbuffer565.cxx)
endif()

if (DRM_FOUND)
target_compile_definitions(raspifb16 PRIVATE LIBDRM_INSTALLED)
target_include_directories(raspifb16 PUBLIC ${DRM_INCLUDE_DIRS})
target_sources(raspifb16 PRIVATE libraspifb16/drmMode.cxx)
endif()

include_directories(${PROJECT_SOURCE_DIR}/libraspifb16)

set(EXTRA_LIBS ${EXTRA_LIBS} raspifb16)

#--------------------------------------------------------------------------

add_executable(boxworld boxworld/main.cxx
                        boxworld/level.cxx
                        boxworld/levels.cxx
                        boxworld/boxworld.cxx)

target_link_libraries(boxworld raspifb16
                               ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(fbpipe fbpipe/fbpipe.cxx)
target_link_libraries(fbpipe raspifb16
                             ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(joystickConfigure joystick/joystickConfigure.cxx)
target_link_libraries(joystickConfigure raspifb16)

#--------------------------------------------------------------------------

add_executable(puzzle-15 puzzle-15/main.cxx
                         puzzle-15/puzzle.cxx)

target_link_libraries(puzzle-15 raspifb16
                                ${DRM_LIBRARIES})

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
                               -lbsd
                               ${FREETYPE_LIBRARIES}
                               ${DRM_LIBRARIES})

set_property(TARGET raspinfo PROPERTY SKIP_BUILD_RPATH TRUE)
install (TARGETS raspinfo RUNTIME DESTINATION bin)

#--------------------------------------------------------------------------

add_executable(splash splash/splash.cxx)
target_link_libraries(splash raspifb16
                             ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(test test/test.cxx)
target_link_libraries(test raspifb16
                           ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(testCircle test/testCircle.cxx)
target_link_libraries(testCircle raspifb16
                                 ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(testColour test/testColour.cxx)
target_link_libraries(testColour raspifb16
                                 ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(testColourBlend test/testColourBlend.cxx)
target_link_libraries(testColourBlend raspifb16
                                      ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

add_executable(testPutImage test/testPutImage.cxx)
target_link_libraries(testPutImage raspifb16
                                   ${DRM_LIBRARIES})

#--------------------------------------------------------------------------

if (FREETYPE_FOUND)
add_executable(testft test/testft.cxx)
target_link_libraries(testft raspifb16
                             ${FREETYPE_LIBRARIES}
                             ${DRM_LIBRARIES})

add_executable(testft2 test/testft2.cxx)
target_link_libraries(testft2 raspifb16
                              ${FREETYPE_LIBRARIES}
                              ${DRM_LIBRARIES})
endif()

#--------------------------------------------------------------------------

if (DRM_FOUND)
add_executable(modes modes/modes.cxx)

target_include_directories(modes PUBLIC ${DRM_INCLUDE_DIRS})
target_link_libraries(modes raspifb16
                            ${DRM_LIBRARIES})
endif()

#--------------------------------------------------------------------------

if (LIBIW_FOUND)
add_executable(wifiscan wifiscan/scanEntry.cxx
                        wifiscan/wifiscan.cxx)

target_link_libraries(wifiscan raspifb16
                               ${FREETYPE_LIBRARIES}
                               ${DRM_LIBRARIES}
                               ${LIBIW_LIBRARIES})
endif()
