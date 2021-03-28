cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(segy LANGUAGES CXX)

add_library(openCV-vis
        STATIC
        visualization.h visualization.cpp
        )
if (USE_OpenCV)
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif ()
target_compile_definitions(openCV-vis PUBLIC $<$<BOOL:${USE_OpenCV}>:USE_OpenCV>)

if (USE_OpenCV)
    if (${USE_INTEL} STREQUAL "YES")
        target_link_libraries(openCV-vis ${OpenCV_LIBS} "-no-ipo")

    else ()
        target_link_libraries(openCV-vis ${OpenCV_LIBS})
    endif ()
endif ()



