# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(RTM-OpenMP LANGUAGES CXX)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


add_library(
		Forward-Collector-Helpers
		SHARED
		./concrete-components/forward_collectors/file_handler/file_handler.cpp
		./concrete-components/forward_collectors/boundary_saver/boundary_saver.cpp
)

add_library(
        SA-Components
        SHARED
        ./concrete-components/acoustic_second_components.h
        ./concrete-components/source_injectors/ricker_source_injector.cpp
		################################
		##### BOUNDARIES COMPONENTS ####
		################################
        ./concrete-components/boundary_managers/no_boundary_manager.cpp
		./concrete-components/boundary_managers/random_boundary_manager.cpp
		./concrete-components/boundary_managers/sponge_boundary_manager.cpp
		./concrete-components/boundary_managers/cpml_boundary_manager.cpp
		./concrete-components/boundary_managers/staggered_cpml_boundary_manager.cpp
		./concrete-components/boundary_managers/extensions/extension.cpp
		./concrete-components/boundary_managers/extensions/zero_extension.cpp
		./concrete-components/boundary_managers/extensions/random_extension.cpp
		./concrete-components/boundary_managers/extensions/homogenous_extension.cpp
		./concrete-components/boundary_managers/extensions/min_extension.cpp
		################################
		##### FORWARD COLLECTORS    ####
		################################
        ./concrete-components/forward_collectors/reverse_propagation.cpp
		./concrete-components/forward_collectors/two_propagation.cpp
		./concrete-components/forward_collectors/reverse_injection_propagation.cpp
		./concrete-components/forward_collectors/staggered_two_propagation.cpp
		./concrete-components/forward_collectors/staggered_reverse_propagation.cpp
		./concrete-components/forward_collectors/staggered_reverse_injection_propagation.cpp
		################################
		##### COMPUTATION KERNELS   ####
		################################
        ./concrete-components/computation_kernels/second_order_computation_kernel.cpp
		./concrete-components/computation_kernels/staggered_computation_kernel.cpp
		################################
		################################
		################################
		./concrete-components/model_handlers/homogenous_model_handler.cpp
		./concrete-components/model_handlers/seismic_model_handler.cpp
        ./concrete-components/correlation_kernels/cross_correlation_kernel.cpp
        ./concrete-components/trace_managers/binary_trace_manager.cpp
		./concrete-components/trace_managers/seismic_trace_manager.cpp
		./concrete-components/modelling/trace_writer/binary_trace_writer.cpp
		./concrete-components/modelling/modelling_configuration_parser/text_modelling_configuration_parser.cpp
)

target_link_libraries(SA-Components RTM-Components RTM-Base RTM-Helpers Forward-Collector-Helpers FILE-COMPRESSION segy-tools seis-io)


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
		./concrete-parsers/acoustic_omp_parameter_parser.cpp
		./concrete-parsers/configuration_parser.cpp
		./concrete-parsers/components/model_handler_parser.cpp
		./concrete-parsers/components/source_injector_parser.cpp
		./concrete-parsers/components/boundary_manager_parser.cpp
		./concrete-parsers/components/forward_collector_parser.cpp
		./concrete-parsers/components/correlation_kernel_parser.cpp
		./concrete-parsers/components/trace_manager_parser.cpp
		./concrete-parsers/components/trace_writer_parser.cpp
		./concrete-parsers/components/modelling_configuration_parser_parser.cpp
		./concrete-parsers/callback_parser.cpp
)
target_link_libraries(Parameters-Parsers SA-Components Standard-Callback)
