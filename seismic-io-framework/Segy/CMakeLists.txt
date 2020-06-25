cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

#set(CMAKE_CXX_STANDARD 17)

project(segy LANGUAGES CXX)



set(BUILD_SHARED_LIBS ON)

list(APPEND _sources segyelement.h swapbyte.h swapbyte.cpp suheaders.h suheaders.cpp segy_helpers.h segy_helpers.cpp  susegy.h susegy.cpp segy_io_manager.h segy_io_manager.cpp)



   
add_library(segy-tools SHARED ${_sources})



add_executable(segy_test test_segy.cpp)

target_link_libraries(segy_test segy-tools)





