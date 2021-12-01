#!/bin/bash

#==============================================================================
# author		      :bassant-magdy
# usage		        :sudo ./<script-name>
#==============================================================================

echo "Spdlog download script assumes you have sudo access..."

dir="$(pwd)"

$ git clone https://github.com/gabime/spdlog.git
$ cd spdlog && mkdir build && cd build
$ cmake .. && make -j

cd "$dir" || exit