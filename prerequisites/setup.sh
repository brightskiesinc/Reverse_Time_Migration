#!/bin/bash

#==============================================================================
:'
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
'
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
