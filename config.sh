#!/usr/bin/env bash
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_SOURCE_DIR=$(dirname "$0")
echo "working on directory $PROJECT_SOURCE_DIR"

while getopts ":c:xghvi:o:d:w:C:srt:" opt; do
	case $opt in
	  	c) 	##### Setting Compression type #####
			echo -e "${BLUE}Compression path is: $OPTARG${NC}"
			COMPRESSION_PATH="$OPTARG"
			;;

		C)	##### Set cmake path #####
			echo -e "${BLUE}using cmake path in $OPTARG${NC}"
			CMAKE=$OPTARG
			;;
		d)	##### Set the path of data #####
			echo -e "${BLUE}using data path of $OPTARG${NC}"
			DATA_PATH=${OPTARG}/
			;;
		w)	##### Set the path where it will write all processed data #####
			echo -e "${BLUE}using write path of $OPTARG${NC}"
			WRITE_PATH=${OPTARG}/
			;;
		r)  ##### Enabling Release mode #####
			echo -e "${GREEN}using release build mode${NC}"
			BUILD_TYPE="RELEASE"
			;;
		g)  ##### Build using gcc compilers instead of intel ####
			echo -e "${GREEN}using gcc/g++ to compile project${NC}"
			USE_INTEL="NO"
			;;
		v) 	##### printing full output of make #####
			echo -e "${YELLOW}printing make with details${NC}"
			VERBOSE=ON
			;;
		\?) 	##### using default settings #####
			echo -e "${RED}using default compression -> ZFP${NC}"
			echo -e "${GREEN}using Debug mode${NC}"
			echo -e "${GREEN}Using icc/icpc compilers${NC}"
			USE_INTEL="YES"
			BUILD_TYPE="DEBUG"
			COMPRESSION="ZFP"
			VERBOSE=OFF
			;;
		:)	##### Error in an option #####
			echo "Option $OPTARG requires parameter(s)"
			exit 0
			;;
		t)	##### Technology Switch #####
			TECH=$OPTARG
			if [ "$TECH" == "OMP" ] || [ "$TECH" == "omp" ]; then
				echo -e "${GREEN}Using OpenMP technology"
				TECH="omp"

			elif [ "$TECH" == "DPC" ] || [ "$TECH" == "dpc" ]; then
				echo -e "${GREEN}Using DPC++ technology"
				TECH="dpc"
			else
				echo -e "${RED}Invalid technology argument"
				exit 0
			fi
			;;
		i)	##### Images Switch #####
			Images=$OPTARG
			if [ "$Images" == "ON" ] || [ "$Images" == "on" ]; then
				echo -e "${GREEN}Using OpenCV for images"
				Images="on"
			fi
			;;
		h)	##### Prints the help #####
			echo "Usage of $(basename "$0"):"
			echo ""
			printf "%20s %s\n" "-c [parameter] :" "to set the zfp compression path for linking"
			printf "%20s %s\n" "" "default = ${PROJECT_SOURCE_DIR}"
			echo ""
			printf "%20s %s\n" "-C [executable] :" "specify cmake executable"
			printf "%20s %s\n" "" "default = cmake"
			echo ""
			printf "%20s %s\n" "-d [path] :" "specify the path of data"
			printf "%20s %s\n" "" "default path = data/synth/"
			echo ""
			printf "%20s %s\n" "-w [path] :" "specify the path where it will write all processed data"
			printf "%20s %s\n" "" "default path = ./"
			echo ""
			printf "%20s %s\n" "-r :" "to build in release mode (if not set it will build in debug mode)"
			echo ""
			printf "%20s %s\n" "-g :" "to build using gcc/g++ compilers (if not set it will build with icc/icpc)"
			echo ""
			printf "%20s %s\n" "-i [value] :" "enable OpenCV option. Enables if value=yes, disabled if value=no"
			printf "%20s %s\n" "" "default = off"
			echo ""
			printf "%20s %s\n" "-v :" "to print the output of make with details (if not set it will build without details)"
			echo ""
			printf "%20s %s\n" "-t [tech] :" "specify the technology which will be used. values : omp | dpc"
			printf "%20s %s\n" "" "default tech = omp"
			echo ""
			exit 1
			;;
	esac
done

if [ -z "$TECH" ]; then
	TECH="omp"
	echo -e "${RED}Using OpenMp technology${NC}"
fi
if [ "$Images" == "on" ]; then
	USE_OpenCV="ON"
else
	USE_OpenCV="OFF"
fi
if [ -z "$COMPRESSION_PATH" ]; then
	COMPRESSION_PATH="${PROJECT_SOURCE_DIR}"
	echo -e "${RED}Please use a valid path the zfp compression${NC}"
fi
if [ -z "$CMAKE" ]; then
	CMAKE="cmake"
	echo -e "${BLUE}Using the default cmake executable${NC}"
fi
if [ -z "$DATA_PATH" ]; then
	DATA_PATH=data
	echo -e "${BLUE}Using default data directory path $DATA_PATH${NC}"
fi
if [ -z "$WRITE_PATH" ]; then
	WRITE_PATH=results
	echo -e "${BLUE}Using default write directory path $WRITE_PATH${NC}"
fi
if [ -z "$VERBOSE" ]; then
	VERBOSE=OFF
	echo -e "${YELLOW}Not printing make in details${NC}"
fi

if ! [ -d "${WRITE_PATH}" ]; then
	mkdir ${WRITE_PATH}
fi
if ! [ -d "${PROJECT_SOURCE_DIR}/bin" ]; then
  mkdir ${PROJECT_SOURCE_DIR}/bin
fi
rm -r ${PROJECT_SOURCE_DIR}/bin/*
if [ "$TECH" == "omp" ]; then
	USE_DPC="OFF"
	USE_OpenMp="ON"
elif [ "$TECH" == "dpc" ]; then
	USE_DPC="ON"
	USE_OpenMp="OFF"
fi
if [ "$USE_OpenMp" == "ON" ]; then
	if [ -z "$BUILD_TYPE" ]; then
		BUILD_TYPE="DEBUG"
		echo -e "${GREEN}Building in $BUILD_TYPE mode${NC}"
	fi
	if [ -z "$USE_INTEL" ]; then
		echo -e "${GREEN}Using icc/icpc compilers${NC}"
		USE_INTEL="YES"
	fi
	if [ "$BUILD_TYPE" == "DEBUG" ]; then
		if [ "$USE_INTEL" == "YES" ]; then
			CXX_FLAGS="-std=c++17 -xHost -qopenmp -g -debug -O3 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding -qopt-report=4"
		else
			CXX_FLAGS="-fopenmp -g -ftree-vectorize -O3 -fopt-info-vec-optimized"		
		fi
	else
		if [ "$USE_INTEL" == "YES" ]; then
			CXX_FLAGS="-std=c++17 -xHost -qopenmp -O3 -fp-model fast=2 -no-prec-div -fma -qopt-assume-safe-padding -qopt-report=4"
		else
			CXX_FLAGS="-fopenmp -ftree-vectorize -O3 -fopt-info-vec-optimized"
		fi
	fi
elif [ "$USE_DPC" == "ON" ]; then
	echo -e "${YELLOW}Overriding--Building in NOMODE mode${NC}"
	echo -e "${YELLOW}Overriding--Using dpcpp compiler${NC}"
	BUILD_TYPE="NOMODE"
	CXX_FLAGS="-O3 -std=c++17"
else
	echo -e "${RED}No technology selected...${NC}"
fi


echo -e "${GREEN}Using Flags : $CXX_FLAGS${NC}"
##### Performing CMAKE command with all combined options #####
$CMAKE -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
-DUSE_OpenMp=$USE_OpenMp \
-DUSE_DPC=$USE_DPC \
-DUSE_OpenCV=$USE_OpenCV \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=$VERBOSE \
-DDATA_PATH=$DATA_PATH \
-DWRITE_PATH=$WRITE_PATH \
-DUSE_INTEL=$USE_INTEL \
-DCOMPRESSION_PATH=$COMPRESSION_PATH \
-DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
-H${PROJECT_SOURCE_DIR} -B${PROJECT_SOURCE_DIR}/bin 
#--target acoustic_engine


exit 0 # exit successfully without any errors

