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

echo "ZFP download script assumes you have sudo access..."

# Save PWD to return back to it at the end.
dir="$(pwd)"

# Create directory for installations.
cd ~ || exit
if [ ! -d "hpclibs" ]; then
  mkdir hpclibs && cd hpclibs || exit
fi

# Download
git clone https://github.com/LLNL/zfp.git
cd zfp || exit
make

# Return to PWD.
cd "$dir" || exit
