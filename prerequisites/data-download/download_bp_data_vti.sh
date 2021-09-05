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

echo "VTI BP download script assumes you have sudo access..."

echo "This script will download the BP velocity model, density model and all shot files"
echo "The size of the data to be downloaded is around 8 GB, after extraction it will take 13 GB"

# Save PWD to return back to it at the end.
dir="$(pwd)"

cd ../..

if [ ! -d "data" ]; then
  mkdir data
fi

cd data || exit

if [ ! -d "vti" ]; then
  mkdir vti
fi

cd vti || exit

wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/README_Modification.txt
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/DatasetInformation_And_Disclaimer.txt

if [ ! -d "params" ]; then
  mkdir params
fi

cd params || exit

wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/ModelParams.tar.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/OtherFiles-2.tar.gz

cd .. || exit

if [ ! -d "shots" ]; then
  mkdir shots
fi

cd shots || exit

wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/Anisotropic_FD_Model_Shots_part1.sgy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/Anisotropic_FD_Model_Shots_part2.sgy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/Anisotropic_FD_Model_Shots_part3.sgy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/Anisotropic_FD_Model_Shots_part4.sgy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bptti2007/Anisotropic_FD_Model_VSP.tar.gz

# Return to PWD.
cd "$dir" || exit
