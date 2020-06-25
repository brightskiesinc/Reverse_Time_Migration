# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(RTM-DPCPP LANGUAGES CXX)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
        SA-Components
        SHARED
        concrete-components/acoustic_second_components.h
		################################
		###### SOURCE INJECTIONS  ######
		################################
		concrete-components/source_injectors/ricker_source_injector.cpp
		################################
		##### BOUNDARIES COMPONENTS ####
		################################
		concrete-components/boundary_managers/no_boundary_manager.cpp
		concrete-components/boundary_managers/random_boundary_manager.cpp
		concrete-components/boundary_managers/extensions/extension.cpp
		concrete-components/boundary_managers/extensions/zero_extension.cpp
		concrete-components/boundary_managers/extensions/random_extension.cpp
		concrete-components/boundary_managers/extensions/homogenous_extension.cpp
		################################
		##### COMPUTATION KERNELS   ####
		################################
		concrete-components/computation_kernels/second_order_computation_kernel.cpp
		################################
		######   MODEL HANDLERS   ######
		################################
		concrete-components/model_handlers/homogenous_model_handler.cpp
        concrete-components/model_handlers/seismic_model_handler.cpp
		concrete-components/model_handlers/model_handler_helpers/model_handler_utils.cpp
		################################
		###### FORWARD COLLECTORS ######
		################################
		concrete-components/forward_collectors/reverse_propagation.cpp
		################################
		###### CORRELATION KERNEL ######
		################################
		concrete-components/correlation_kernels/cross_correlation_kernel.cpp
		################################
		######   TRACE MANAGERS   ######
		################################
		concrete-components/trace_managers/binary_trace_manager_oneApi.cpp
		concrete-components/trace_managers/seismic_trace_manager.cpp
		################################
		###### MODELLING COMPONENTS ####
		################################
		concrete-components/modelling/trace_writer/binary_trace_writer.cpp
		concrete-components/modelling/modelling_configuration_parser/text_modelling_configuration_parser.cpp
)

target_link_libraries(SA-Components RTM-Components RTM-Base RTM-Helpers segy-tools seis-io sycl OpenCL)


add_library(
		Standard-Callback
		SHARED
		concrete-callbacks/writer_callback.cpp
		concrete-callbacks/csv_writer.cpp
		concrete-callbacks/norm_writer.cpp
		concrete-callbacks/segy_writer.cpp
		concrete-callbacks/image_writer.cpp
		concrete-callbacks/su_writer.cpp
		concrete-callbacks/binary_writer.cpp
)

target_link_libraries(Standard-Callback general-utils openCV-vis)

add_library(
		Parameters-Parsers
		SHARED
		concrete-parsers/parameter_parser.cpp
		concrete-parsers/configuration_parser.cpp
		concrete-parsers/components/model_handler_parser.cpp
		concrete-parsers/components/source_injector_parser.cpp
		concrete-parsers/components/boundary_manager_parser.cpp
		concrete-parsers/components/forward_collector_parser.cpp
		concrete-parsers/components/correlation_kernel_parser.cpp
		concrete-parsers/components/trace_manager_parser.cpp
		concrete-parsers/components/trace_writer_parser.cpp
		concrete-parsers/components/modelling_configuration_parser_parser.cpp
		concrete-parsers/callback_parser.cpp
)
target_link_libraries(Parameters-Parsers SA-Components Standard-Callback)