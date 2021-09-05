#!/bin/bash

#==============================================================================
echo \
 "
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Seismic Toolbox.
 *
 * Seismic Toolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Seismic Toolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 "
#==============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_SOURCE_DIR=$(dirname "$0")
echo "working on directory $PROJECT_SOURCE_DIR"

for arg in "$@"; do
  shift
  case "$arg" in
  "--help") set -- "$@" "-h" ;;
  "--release") set -- "$@" "-r" ;;
  "--mpi") set -- "$@" "-m" ;;
  "--images") set -- "$@" "-i" ;;
  "--tests") set -- "$@" "-t" ;;
  "--examples") set -- "$@" "-e" ;;
  *) set -- "$@" "$arg" ;;
  esac
done

while getopts ":c:d:w:C:b:ghvimrte" opt; do
  case $opt in
  t) ##### Building tests enabled #####
    echo -e "${GREEN}Building tests enabled${NC}"
    BUILD_TESTS="ON"
    ;;

  e) ##### Building examples enabled #####
    echo -e "${GREEN}Building examples enabled${NC}"
    BUILD_EXAMPLES="ON"
    ;;

  c) ##### Setting compression type #####
    echo -e "${BLUE}Compression path is: $OPTARG${NC}"
    COMPRESSION="ZFP"
    COMPRESSION_PATH="$OPTARG"
    ;;

  C) ##### Set CMake path #####
    echo -e "${BLUE}using cmake path in $OPTARG${NC}"
    CMAKE=$OPTARG
    ;;

  d) ##### Set data path #####
    echo -e "${BLUE}using data path of $OPTARG${NC}"
    DATA_PATH=${OPTARG}/
    ;;

  w) ##### Set write path of all processed data #####
    echo -e "${BLUE}Using write path of $OPTARG${NC}"
    WRITE_PATH=${OPTARG}/
    ;;

  r) ##### Enabling release mode #####
    echo -e "${GREEN}Using release build mode${NC}"
    BUILD_TYPE="RELEASE"
    ;;

  g) ##### Build using gcc compilers instead of intel ####
    echo -e "${GREEN}Using gcc/g++ to compile project${NC}"
    USE_INTEL="NO"
    ;;

  m) ##### Build using MPI #####
    echo -e "${GREEN}Using MPI for nodes distribution"
    MPI="ON"
    ;;

  i) ##### Build using OpnCV for images #####
    echo -e "${GREEN}Using OpenCV for images"
    IMAGES="ON"
    ;;

  v) ##### Verbose mode #####
    echo -e "${YELLOW}printing make with details${NC}"
    VERBOSE=ON
    ;;

  \?) ##### using default settings #####
    echo -e "${RED}Using default compression -> NO${NC}"
    COMPRESSION="NO"

    echo -e "${GREEN}Using Debug mode${NC}"
    BUILD_TYPE="DEBUG"

    echo -e "${GREEN}Using icc/icpc compilers${NC}"
    USE_INTEL="YES"

    VERBOSE=OFF
    ;;

  :) ##### Error in an option #####
    echo "Option $OPTARG requires parameter(s)"
    exit 0
    ;;
  b) ##### Technology Switch #####
    TECH=$OPTARG
    if [ "$TECH" == "OMP" ] || [ "$TECH" == "omp" ]; then
      echo -e "${GREEN}Using OpenMP technology for backend"
      TECH="omp"
    elif [ "$TECH" == "DPC" ] || [ "$TECH" == "dpc" ]; then
      echo -e "${GREEN}Using DPC++ technology for backend"
      TECH="dpc"
    elif [ "$TECH" == "OMP_OFFLOAD" ] || [ "$TECH" == "omp-offload" ]; then
      echo -e "${GREEN}Using omp offload technology"
      TECH="omp-offload"

    else
      echo -e "${RED}Invalid technology argument for backend"
      exit 0
    fi
    ;;
  h) ##### Prints the help #####
    echo "Usage of $(basename "$0"):"
    echo ""
    printf "%20s %s\n" "-c [parameter] :" "Specifies ZFP Compression path for linking"
    printf "%20s %s\n" "" "default = ${PROJECT_SOURCE_DIR}"
    echo ""
    printf "%20s %s\n" "-C [executable] :" "Specifies CMake executable"
    printf "%20s %s\n" "" "default = cmake"
    echo ""
    printf "%20s %s\n" "-d [path] :" "Specifies the path of data"
    printf "%20s %s\n" "" "default path = data/synth/"
    echo ""
    printf "%20s %s\n" "-w [path] :" "Specifies the path where it will write all processed data"
    printf "%20s %s\n" "" "default path = ./"
    echo ""
    printf "%20s %s\n" "-r | --release:" "Build in release mode (if not set it will build in debug mode)"
    echo ""
    printf "%20s %s\n" "-g :" "Build using gcc/g++ compilers (if not set it will build with icc/icpc)"
    echo ""
    printf "%20s %s\n" "-i | --images :" "Enables OpenCV option. Enables if value=yes, disabled if value=no"
    printf "%20s %s\n" "" "default = off"
    echo ""
    printf "%20s %s\n" "-v | --verbose :" "to print the output of make with details (if not set it will build without details)"
    echo ""
    printf "%20s %s\n" "-b [backend] :" "Specifies the technology which will be used. values : omp | dpc | cuda"
    printf "%20s %s\n" "" "default tech = omp"
    echo ""
    printf "%20s %s\n" "-t | --test :" "Enables building tests."
    echo ""
    printf "%20s %s\n" "-e | --examples :" "Enable building examples."
    echo ""
    exit 1
    ;;
  esac
done

if [ -z "$BUILD_TESTS" ]; then
  BUILD_TESTS="OFF"
  echo -e "${RED}Building tests disabled${NC}"
fi

if [ -z "$BUILD_EXAMPLES" ]; then
  BUILD_EXAMPLES="OFF"
  echo -e "${RED}Building examples disabled${NC}"
fi

if [ -z "$TECH" ]; then
  TECH="omp"
  echo -e "${RED}Using OpenMp technology${NC}"
fi

if [ "$IMAGES" == "ON" ]; then
  USE_OpenCV="ON"
else
  USE_OpenCV="OFF"
  echo -e "${RED}Images disabled${NC}"
fi

if [ "$MPI" == "ON" ]; then
  USE_MPI="ON"
else
  USE_MPI="OFF"
  echo -e "${RED}Nodes distribution disabled${NC}"
fi

if [ -z "$COMPRESSION_PATH" ]; then
  COMPRESSION="NO"
  COMPRESSION_PATH="${PROJECT_SOURCE_DIR}"
  echo -e "${RED}Please use a valid path for ZFP compression${NC}"
fi

if [ -z "$CMAKE" ]; then
  CMAKE="cmake"
  echo -e "${BLUE}Using the default cmake executable${NC}"
fi

if [ -z "$DATA_PATH" ]; then
  DATA_PATH=data
  echo -e "${BLUE}Using default data directory path $DATA_PATH${NC}"
fi

if [ -z "$VERBOSE" ]; then
  VERBOSE="OFF"
  echo -e "${YELLOW}Not printing make in details${NC}"
fi

if [ -z "$WRITE_PATH" ]; then
  WRITE_PATH=results
  echo -e "${BLUE}Using default write directory path $WRITE_PATH${NC}"
fi

if ! [ -d "${WRITE_PATH}" ]; then
  mkdir ${WRITE_PATH}
fi

if ! [ -d "${PROJECT_SOURCE_DIR}/bin" ]; then
  mkdir "${PROJECT_SOURCE_DIR}"/bin
fi

if [ "$TECH" == "omp" ]; then
  USE_DPC="OFF"
  USE_OMP="ON"
  USE_OMP_OFFLOAD="OFF"
elif [ "$TECH" == "dpc" ]; then
  USE_DPC="ON"
  USE_OMP="OFF"
  USE_OMP_OFFLOAD="OFF"
elif [ "$TECH" == "omp-offload" ]; then
  USE_DPC="OFF"
  USE_OMP="OFF"
  USE_OMP_OFFLOAD="ON"
fi

if [ "$USE_OMP" == "ON" ]; then
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
  CXX_FLAGS="-O3 -fsycl -std=c++17"
elif [ "$USE_OMP_OFFLOAD" == "ON" ]; then
  echo -e "${YELLOW}Overriding--Building in NOMODE mode${NC}"
  echo -e "${YELLOW}Overriding--Using Omp_offload compiler${NC}"
  BUILD_TYPE="NOMODE"
  CXX_FLAGS="-fiopenmp -std=c++17 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__"
else

  echo -e "${RED}No technology selected...${NC}"
fi

echo -e "${GREEN}Using Flags : $CXX_FLAGS${NC}"

rm -rf ${PROJECT_SOURCE_DIR}/bin
mkdir bin/

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTS=$BUILD_TESTS \
  -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
  -DUSE_OMP=$USE_OMP \
  -DUSE_DPC=$USE_DPC \
  -DUSE_OMP_OFFLOAD=$USE_OMP_OFFLOAD\
  -DUSE_OpenCV=$USE_OpenCV \
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=$VERBOSE \
  -DDATA_PATH=$DATA_PATH \
  -DWRITE_PATH=$WRITE_PATH \
  -DUSE_INTEL=$USE_INTEL \
  -DCOMPRESSION=$COMPRESSION \
  -DCOMPRESSION_PATH=$COMPRESSION_PATH \
  -DUSE_MPI=$USE_MPI \
  -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
  -H"${PROJECT_SOURCE_DIR}" \
  -B"${PROJECT_SOURCE_DIR}"/bin
