# sets a minimum required version for CMake. A fatal error
#will be issued if a version of CMake lower than that is used
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

#declares the name of the project (recipe-01) and the
#supported language (CXX stands for C++):

project(seismic_io LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 17)

add_subdirectory(./Segy)
add_subdirectory(./visualization)


add_library(seis-io SHARED datatypes.cpp datatypes.h )

add_executable(datatypes datatypes.cpp)