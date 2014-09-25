# set(CMAKE_CXX_COMPILER "clang++")
project(ProjectAbstraction)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

set(CMAKE_CXX_FLAGS	"-Wall -std=c++11")

# find_package(PkgConfig)


#include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# OpenGL
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})
# if (OPENGL_FOUND)
#   target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
#   target_link_libraries(${PROJECT_NAME} m)  # if you use maths.h
# endif()

#include_directories(${PROJECT_SOURCE_DIR}/include/bullet)
##link directories
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

#include_directories(${PROJECT_SOURCE_DIR}/include/SFML)

# target_link_libraries(${PROJECT_NAME} optimized ${PROJECT_SOURCE_DIR}/lib/sdl2/libSDL2.so
#                                                 ${PROJECT_SOURCE_DIR}/lib/horde3d/release/libHorde3D.so
# 												${PROJECT_SOURCE_DIR}/lib/horde3d/release/libHorde3DUtils.so)

target_link_libraries(${PROJECT_NAME} debug ${PROJECT_SOURCE_DIR}/lib/sdl2/libSDL2.so
                                                ${PROJECT_SOURCE_DIR}/lib/horde3d/debug/libHorde3D.so
												${PROJECT_SOURCE_DIR}/lib/horde3d/debug/libHorde3DUtils.so)
