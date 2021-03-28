# Set minimum CMake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Project settings.
project(Helpers LANGUAGES CXX VERSION 1.0.0)

# set C++ standard to C++11 to allow for the advanced features used in code.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Modules addition
add_subdirectory(MemoryManager)
add_subdirectory(Timer)