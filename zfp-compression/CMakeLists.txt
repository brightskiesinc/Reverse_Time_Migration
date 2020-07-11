cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(compression)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
SET(COMPRESSION "ZFP")
SET(USE_INTEL "YES")
endif()

if ("${CMAKE}" STREQUAL "")
	SET(CMAKE "cmake")
endif()

if ("${COMPRESSION}" STREQUAL "ZFP")

	if ("${ZFP_PATH}" STREQUAL "")
		SET(ZFP_PATH ${COMPRESSION_PATH})
	endif()
	message(STATUS "Checking ZFP library")
	include_directories(${ZFP_PATH}/include/)

	set(ZFP_LIBS ${ZFP_PATH}/lib/libzfp.a)
	add_definitions( -DZFP_COMPRESSION)
	
	if ("${USE_INTEL}" STREQUAL "YES")

		if(NOT DEFINED ENV{IPPROOT})
        		message(FATAL_ERROR "IPPROOT environment variable should be set for Intel(R) IPP build")
    		endif()

		message("Using Intel(R) with ZFP compression")
		set(IPPROOT $ENV{IPPROOT})
		include_directories(${IPPROOT}/include)

		set(IPP_LIBS ${IPP_LIBS} ${IPPROOT}/lib/intel64/libippdc.a)
		set(IPP_LIBS ${IPP_LIBS} ${IPPROOT}/lib/intel64/libipps.a)
		set(IPP_LIBS ${IPP_LIBS} ${IPPROOT}/lib/intel64/libippcore.a)
		

		SET(COMPRESS_LIBS ${ZFP_LIBS} ${IPP_LIBS})
		message(STATUS "${ZFP_PATH}")
	else()
		message(STATUS "Using gcc compiler with ZFP compression")
		message(STATUS "${ZFP_PATH}")
		SET(COMPRESS_LIBS ${ZFP_LIBS})

	endif()
elseif ("${COMPRESSION}" STREQUAL "NO")
	message(STATUS "Not Using Any Compression Technique")
else ()
	message(ERROR " ${COMPRESSION} is not implemented")
	message(STATUS "Not Using Any Compression Technique")
endif()

add_library(
		FILE-COMPRESSION
		SHARED
		compress.cpp
)

target_link_libraries(FILE-COMPRESSION ${COMPRESS_LIBS})

add_executable(test_compress main_compress.cpp)
target_link_libraries(test_compress FILE-COMPRESSION RTM-Helpers)
