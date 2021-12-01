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

verbose=
serial=-j

while getopts "vsh" opt; do
  case $opt in
  v)
    verbose="VERBOSE=1"
    echo "Using verbose mode"
    ;;
  s)
    serial=
    echo "Using serial mode"
    ;;
  h)
    echo "Usage of $(basename "$0"):"
    echo "	to clean the bin directory then builds the code and run it "
    echo ""
    echo "-v	: to print the output of make in details"
    echo ""
    echo "-s	: to compile serially rather than in parallel"
    echo ""
    exit 1
    ;;
  *)
    echo "Invalid flags entered. run using the -h flag for help"
    exit 1
    ;;
  esac
done

cd bin/ || exit
make clean
make all "$serial" $verbose
