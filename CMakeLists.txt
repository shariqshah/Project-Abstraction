
option(USE_CLANG "build with clang" OFF)

if(USE_CLANG)
  set(CMAKE_CXX_COMPILER "clang++")
endif(USE_CLANG)

project(ProjectAbstraction)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

set(CMAKE_CXX_FLAGS	"-Wall -std=c++11 -O3 -ftree-vectorize")

#include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/bullet)
##link directories
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

#include_directories(${PROJECT_SOURCE_DIR}/include/SFML)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  message("Release Build")
  target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/sdl2/libSDL2.so
	                                    ${PROJECT_SOURCE_DIR}/lib/bullet/release/libBulletCollision.so
										${PROJECT_SOURCE_DIR}/lib/bullet/release/libBulletDynamics.so
										${PROJECT_SOURCE_DIR}/lib/bullet/release/libLinearMath.so
                                        ${PROJECT_SOURCE_DIR}/lib/horde3d/release/libHorde3D.so
										${PROJECT_SOURCE_DIR}/lib/horde3d/release/libHorde3DUtils.so)
else()
  message("Debug Build")
  target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/sdl2/libSDL2.so
	                                    ${PROJECT_SOURCE_DIR}/lib/bullet/release/libBulletCollision.so
										${PROJECT_SOURCE_DIR}/lib/bullet/release/libBulletDynamics.so
										${PROJECT_SOURCE_DIR}/lib/bullet/release/libLinearMath.so
	                                    # ${PROJECT_SOURCE_DIR}/lib/bullet/debug/libBulletCollision-d.so
										# ${PROJECT_SOURCE_DIR}/lib/bullet/debug/libBulletDynamics-d.so
										# ${PROJECT_SOURCE_DIR}/lib/bullet/debug/libLinearMath-d.so
                                        ${PROJECT_SOURCE_DIR}/lib/horde3d/debug/libHorde3D.so
										${PROJECT_SOURCE_DIR}/lib/horde3d/debug/libHorde3DUtils.so)
endif()
