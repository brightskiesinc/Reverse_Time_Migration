##############################################################################################################################################
#CMake version :
###############################################################################################################################################
# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
##############################################################################################################################################
#C++ standard and compiler options :
###############################################################################################################################################
find_program(BASH_EXECUTABLE NAMES bash REQUIRED)

#Intel Parralel Studio tools should be Sourced when using the Cmake Command or with the build script

if ("${USE_INTEL}" STREQUAL "")
	SET(USE_INTEL "NO")
endif()

option(USE_OpenMp "Use OpenMp technology" OFF)
option(USE_DPC "Use DPC++ technology" OFF)
message(STATUS "Use OpenMp ${USE_OpenMp} and Use DPC++ ${USE_DPC}")

if(NOT USE_OpenMp AND NOT USE_DPC)
message(FATAL_ERROR "Please specify a technology using the config.sh script")
elseif(USE_OpenMp AND USE_DPC)
message(FATAL_ERROR "Please specify only one technology using the config.sh script")
elseif(USE_OpenMp)
	if (${USE_INTEL} STREQUAL "YES")
		set(CMAKE_C_COMPILER icc)
		set(CMAKE_CXX_COMPILER icpc)

		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -xHost -qopenmp -g -debug -O3 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding -qopt-report=4")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -xHost -qopenmp -O3 -qopt-report=4 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding")
	else()
		set(CMAKE_C_COMPILER gcc)
		set(CMAKE_CXX_COMPILER g++)


		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp -g -ftree-vectorize -O3 -fopt-info-vec-optimized ")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp -ftree-vectorize -O3 -fopt-info-vec-optimized ")
	endif()
elseif(USE_DPC)
	set(CMAKE_C_COMPILER dpcpp)
	set(CMAKE_CXX_COMPILER dpcpp)
	set(CMAKE_CXX_FLAGS_DEBUG "-O3 -std=c++17 " )
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++17 " )
	set(CMAKE_CXX_FLAGS " -O3 -std=c++17 " )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenCL -lsycl")
endif()



if ("${CMAKE_BUILD_TYPE}" STREQUAL "" OR ${CMAKE_BUILD_TYPE} STREQUAL "NOMODE")
	message(STATUS "WORKING ON NO MODE")
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
	message(STATUS "WORKING ON RELEASE MODE")
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
elseif (${CMAKE_BUILD_TYPE} STREQUAL "DEBUG" OR ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message(STATUS "WORKING ON DEBUG MODE")
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
else ()
	message(FATAL_ERROR "Unrecognized build type")
endif()

message(STATUS "Flags : ${CMAKE_CXX_FLAGS}")
##############################################################################################################################################
#Compiler Definitions :
###############################################################################################################################################
if ("${DATA_PATH}" STREQUAL "")
	SET(DATA_PATH "./data")
endif()
if ("${WRITE_PATH}" STREQUAL "")
	SET(WRITE_PATH "./results")
endif()
if ("${WORKLOAD_PATH}" STREQUAL "")
	SET(WORKLOAD_PATH "./workloads/bp_model")
endif()
if ("${COMPRESSION}" STREQUAL "")
	SET(COMPRESSION "ZFP")
endif()

add_definitions(-DDATA_PATH="${DATA_PATH}" -DWRITE_PATH="${WRITE_PATH}" -DWORKLOAD_PATH="${WORKLOAD_PATH}")

##############################################################################################################################################
#Project settings and targets :
###############################################################################################################################################
# project name and language
project(Acoustic2nd-RTM LANGUAGES CXX)

include_directories(./)
include_directories(./parsers)
include_directories(./utils)
include_directories(./rtm-framework)
include_directories(./zfp-compression)
include_directories(./seismic-io-framework/)
include_directories(./seismic-io-framework/Segy)
include_directories(./seismic-io-framework/visualization)
option(USE_OpenCV "Use OpenCV technology" OFF)
if(USE_OpenCV)
message(STATUS "Using OpenCV technology")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
else()
message(STATUS "No OpenCV used")
endif()

add_subdirectory(rtm-framework)
add_subdirectory(seismic-io-framework)
add_subdirectory(utils)

if(NOT USE_OpenMp AND NOT USE_DPC)
message(FATAL_ERROR "Please specify a technology using the config.sh script")
elseif(USE_OpenMp AND USE_DPC)
message(FATAL_ERROR "Please specify only one technology using the config.sh script")
elseif(USE_OpenMp)
add_subdirectory(zfp-compression)
add_subdirectory(OpenMp)
elseif(USE_DPC)
add_subdirectory(dpcpp)
endif()

add_executable(acoustic_engine main_migration.cpp)
target_link_libraries(acoustic_engine SA-Components RTM-Engine RTM-Dummy-Components RTM-Callbacks Standard-Callback Parameters-Parsers cmd-parser noise-filtering)

add_executable(acoustic_modeller main_modelling.cpp)
target_link_libraries(acoustic_modeller SA-Components Modelling-Engine RTM-Dummy-Components RTM-Callbacks Standard-Callback Parameters-Parsers cmd-parser)

# enable testing functionality
enable_testing()
# define tests
add_test(
		NAME advisor_test
		#The Bash shell script advisor_test  which exists at the CMAKE_CURRENT_SOURCE_DIR
		#calls the executable, which is acoustic_engine
		#and it has commands to run different advisor tests
		#and also it check the result and return 0 in case of success and 1 in case of fail which is the default needed for ctest
		COMMAND ${BASH_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/advisor_test.sh $<TARGET_FILE:acoustic_engine>
)

# define tests
add_test(
		NAME vtune_test
		#The Bash shell script vtune_test  which exists at the CMAKE_CURRENT_SOURCE_DIR
		#calls the executable, which is acoustic_engine
		#and it has commands to run different vtune tests
		#and also it check the result and return 0 in case of success and 1 in case of fail which is the default needed for ctest
		COMMAND ${BASH_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/vtune_test.sh $<TARGET_FILE:acoustic_engine>
)

add_test(
		NAME snapshot_test
		#The Bash shell script snapshot_test  which exists at the CMAKE_CURRENT_SOURCE_DIR
		#calls the executable, which is acoustic_engine
		#and it has commands to run different snapshot test
		#and also it check the result and return 0 in case of success and 1 in case of fail which is the default needed for ctest
		COMMAND ${BASH_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/snapshot_test.sh $<TARGET_FILE:acoustic_engine>
)
