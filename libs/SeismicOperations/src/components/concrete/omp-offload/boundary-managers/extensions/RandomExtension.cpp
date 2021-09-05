/**
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
 */



#include "operations/components/independents/concrete/boundary-managers/extensions/RandomExtension.hpp"
#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

void RandomExtension::VelocityExtensionHelper(float *property_array,
                                              int start_x, int start_y, int start_z,
                                              int end_x, int end_y, int end_z,
                                              int nx, int ny, int nz,
                                              uint boundary_length) {
    throw bs::base::exceptions::NOT_IMPLEMENTED_EXCEPTION();
}

void RandomExtension::TopLayerExtensionHelper(float *property_array,
                                              int start_x, int start_y, int start_z,
                                              int end_x, int end_y, int end_z,
                                              int nx, int ny, int nz, uint boundary_length) {
    // Do nothing, no top layer to extend in random boundaries.
}

void RandomExtension::TopLayerRemoverHelper(float *property_array,
                                            int start_x, int start_y, int start_z,
                                            int end_x, int end_y, int end_z,
                                            int nx, int ny, int nz, uint boundary_length) {
    // Do nothing, no top layer to remove in random boundaries.
}
