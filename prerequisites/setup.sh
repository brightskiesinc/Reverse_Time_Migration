#!/bin/bash

#==============================================================================
# author		      :zeyad-osama
# usage		        :sudo ./<script-name>
# bash_version    :4.4.20(1)-release
#==============================================================================

echo "This script assumes you have sudo access..."

echo "This script shall ease the process of a complete installation of needed frameworks and data"

# Libraries
./libraries/oneapi/install_oneapi_base_kit.sh
./libraries/oneapi/install_oneapi_hpc_kit.sh
./libraries/opencv/install_opencv_apt.sh
./libraries/zfp/install_zfp.sh
./libraries/boost/install_boost_1.64.sh

# Data
./data-download/download_bp_iso_data.sh

cd ..
git submodule update --init --force
