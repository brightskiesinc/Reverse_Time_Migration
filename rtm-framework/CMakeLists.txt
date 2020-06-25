# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(RTM-Framework LANGUAGES CXX)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp" )
endif()

add_library(RTM-Base INTERFACE)
target_include_directories(
        RTM-Base
        INTERFACE
        skeleton/base
)

add_library(
        RTM-Helpers
        SHARED
        skeleton/helpers/memory_allocation/memory_allocator.cpp
        skeleton/helpers/memory_allocation/memory_allocator.h
        skeleton/helpers/timer/timer.cpp
        skeleton/helpers/timer/timer.hpp
        skeleton/helpers/memory_tracking/src/mem_list.cpp
        skeleton/helpers/memory_tracking/src/mem_utils.cpp
        skeleton/helpers/memory_tracking/src/logger.cpp
        skeleton/helpers/memory_tracking/src/memory_tracker.cpp
        skeleton/helpers/memory_tracking/src/string_list.cpp
)

add_library(RTM-Components INTERFACE)
target_include_directories(
        RTM-Components
        INTERFACE
        skeleton/components
)
target_link_libraries(RTM-Components INTERFACE RTM-Base INTERFACE)

add_library(
        RTM-Callbacks
        SHARED
        skeleton/helpers/callbacks/callback_collection.cpp
        skeleton/helpers/callbacks/callback_collection.h
)

add_library(
        RTM-DoutCallback
        SHARED
        skeleton/helpers/callbacks/example/dout_callback.cpp
)



add_library(
        RTM-Engine
        SHARED
        skeleton/engine/rtm_engine.cpp
        skeleton/engine/rtm_engine.h
        )
target_link_libraries(RTM-Engine RTM-Components RTM-Base RTM-Callbacks)

add_library(Modelling-Components INTERFACE)
target_include_directories(
        Modelling-Components
        INTERFACE
        skeleton/components/modelling
)
target_link_libraries(Modelling-Components INTERFACE RTM-Base INTERFACE)

add_library(
        Modelling-Engine
        SHARED
        skeleton/engine/modelling_engine.cpp
        skeleton/engine/modelling_engine.h
)
target_link_libraries(Modelling-Engine RTM-Components RTM-Base RTM-Callbacks)

add_library(
        RTM-Dummy-Components
        SHARED
        dummy_components/dummy_components.h
        dummy_components/boundary_managers/dummy_boundary_manager.cpp
        dummy_components/computation_kernels/dummy_computation_kernel.cpp
        dummy_components/correlation_kernels/dummy_correlation_kernel.cpp
        dummy_components/forward_collectors/dummy_forward_collector.cpp
        dummy_components/trace_managers/dummy_trace_manager.cpp
        dummy_components/source_injectors/dummy_source_injector.cpp
        dummy_components/model_handlers/dummy_model_handler.cpp
        dummy_components/modelling/modelling_configuration_parser/dummy_modelling_configuration_parser.cpp
        dummy_components/modelling/trace_writer/dummy_trace_writer.cpp
)
target_link_libraries(RTM-Dummy-Components RTM-Components RTM-Base RTM-Helpers Modelling-Components)

add_executable(dummy_migration_test test_migration.cpp)
target_link_libraries(dummy_migration_test RTM-Dummy-Components RTM-Engine RTM-DoutCallback)

add_executable(dummy_modelling_test test_modelling.cpp)
target_link_libraries(dummy_modelling_test RTM-Dummy-Components Modelling-Engine RTM-DoutCallback)
