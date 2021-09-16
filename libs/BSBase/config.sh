#!/bin/bash

#==============================================================================
echo \
 "
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
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

for arg in "$@"; do
  shift
  case "$arg" in
  "--help") set -- "$@" "-h" ;;
  "--release") set -- "$@" "-r" ;;
  "--tests") set -- "$@" "-t" ;;
  "--examples") set -- "$@" "-e" ;;
  *) set -- "$@" "$arg" ;;
  esac
done

while getopts ":tevh" opt; do
  case $opt in
  t) ##### Building tests enabled #####
    echo -e "${GREEN}Building tests enabled${NC}"
    BUILDING_TESTS="ON"
    ;;

  e) ##### Building examples enabled #####
    echo -e "${GREEN}Building examples enabled${NC}"
    BUILDING_EXAMPLES="ON"
    ;;

  v) ##### printing full output of make #####
    echo -e "${YELLOW}printing make with details${NC}"
    VERBOSE=ON
    ;;
  \?) ##### using default settings #####
    echo -e "${RED}Building tests disabled${NC}"
    echo -e "${RED}Building examples disabled${NC}"
    BUILDING_EXAMPLES="OFF"
    BUILDING_TESTS="OFF"
    VERBOSE=OFF
    ;;
  :) ##### Error in an option #####
    echo "Option $OPTARG requires parameter(s)"
    exit 0
    ;;
  h) ##### Prints the help #####
    echo "Usage of $(basename "$0"):"
    echo ""
    printf "%20s %s\n" "-t :" "to enable building tests"
    echo ""
    printf "%20s %s\n" "-e :" "to enable building examples"
    echo ""
    printf "%20s %s\n" "-i | --images :" "to enable OpenCV option."
    echo ""
    exit 1
    ;;
  esac
done

if [ -z "$BUILDING_TESTS" ]; then
  BUILDING_TESTS="OFF"
  echo -e "${RED}Building tests disabled${NC}"
fi

if [ -z "$BUILDING_EXAMPLES" ]; then
  BUILDING_EXAMPLES="OFF"
  echo -e "${RED}Building examples disabled${NC}"
fi

rm -rf bin/
mkdir bin/

PROJECT_SOURCE_DIR=$(dirname "$0")
cmake -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTS=$BUILDING_TESTS \
  -DBUILD_EXAMPLES=$BUILDING_EXAMPLES \
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=$VERBOSE \
  -H"${PROJECT_SOURCE_DIR}" \
  -B"${PROJECT_SOURCE_DIR}/bin"
