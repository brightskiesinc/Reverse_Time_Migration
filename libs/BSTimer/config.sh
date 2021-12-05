#!/bin/bash

#==============================================================================
echo \
 "
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer Library.
 *
 * BS Timer Library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer Library is distributed in the hope that it will be useful,
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
echo "Working on directory $PROJECT_SOURCE_DIR"

for arg in "$@"; do
  shift
  case "$arg" in
  "--help") set -- "$@" "-h" ;;
  "--tests") set -- "$@" "-t" ;;
  "--examples") set -- "$@" "-e" ;;
  "--tools") set -- "$@" "-x" ;;
  *) set -- "$@" "$arg" ;;
  esac
done

while getopts ":b:ghvte" opt; do
  case $opt in
  t) ##### Building tests enabled #####
    echo -e "${GREEN}Building tests enabled${NC}"
    BUILD_TESTS="ON"
    ;;

  e) ##### Building examples enabled #####
    echo -e "${GREEN}Building examples enabled${NC}"
    BUILD_EXAMPLES="ON"
    ;;

  g) ##### Build using gcc compilers instead of intel ####
    echo -e "${GREEN}Using gcc/g++ to compile project${NC}"
    USE_INTEL="NO"
    ;;

  v) ##### Verbose mode #####
    echo -e "${YELLOW}printing make with details${NC}"
    VERBOSE=ON
    ;;

  b) ##### Technology Switch #####
    TECH=$OPTARG
    if [ "$TECH" == "OMP" ] || [ "$TECH" == "omp" ]; then
      echo -e "${GREEN}Using OpenMP technology for backend"
      TECH="omp"
    else
      echo -e "${RED}Invalid technology argument for backend"
      echo -e "${GREEN}Using Default C++ for backend"
      TECH="cpp"
      exit 0
    fi
    ;;

  h) ##### Prints the help #####
    echo "Usage of $(basename "$0"):"
    echo ""
    printf "%20s %s\n" "-g :" "Build using gcc/g++ compilers (if not set it will build with icc/icpc)"
    echo ""
    printf "%20s %s\n" "-v | --verbose :" "to print the output of make with details (if not set it will build without details)"
    echo ""
    printf "%20s %s\n" "-b [backend] :" "Specifies the technology which will be used. values : omp | dpc | omp-offload"
    printf "%20s %s\n" "" "default tech = omp"
    echo ""
    printf "%20s %s\n" "-t | --test :" "Enables building tests."
    echo ""
    printf "%20s %s\n" "-e | --examples :" "Enable building examples."
    echo ""
    exit 1
    ;;

  \?) ##### using default settings #####
    echo -e "${GREEN}Using Debug mode${NC}"
    BUILD_TYPE="DEBUG"

    echo -e "${GREEN}Using icc/icpc compilers${NC}"
    USE_INTEL="YES"

    echo -e "${GREEN}Using Default C++ for backend"
    TECH="cpp"

    VERBOSE=OFF
    ;;

  :) ##### Error in an option #####
    echo "Option $OPTARG requires parameter(s)"
    exit 0
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
  TECH="cpp"
  echo -e "${RED}Using OpenMp technology${NC}"
fi

if [ -z "$VERBOSE" ]; then
  VERBOSE="OFF"
  echo -e "${YELLOW}Not printing make in details${NC}"
fi

if ! [ -d "${PROJECT_SOURCE_DIR}/bin" ]; then
  mkdir "${PROJECT_SOURCE_DIR}"/bin
fi

if [ "$TECH" == "omp" ]; then
  USE_OMP="ON"
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
fi
echo -e "${GREEN}Using Flags : $CXX_FLAGS${NC}"

# shellcheck disable=SC2115
rm -rf "${PROJECT_SOURCE_DIR}/bin"
mkdir bin/

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTS=$BUILD_TESTS \
  -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
  -DUSE_OMP=$USE_OMP \
  -DUSE_INTEL=$USE_INTEL \
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=$VERBOSE \
  -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
  -H"${PROJECT_SOURCE_DIR}" \
  -B"${PROJECT_SOURCE_DIR}"/bin
