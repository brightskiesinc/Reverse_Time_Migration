//
// Created by amr-nasr on 18/11/2019.
//

#include "operations/components/independents/concrete/boundary-managers/extensions/MinExtension.hpp"

#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

void MinExtension::VelocityExtensionHelper(float *property_array,
                                           int start_x, int start_y, int start_z,
                                           int end_x, int end_y, int end_z,
                                           int nx, int ny, int nz,
                                           uint boundary_length) {
    throw exceptions::NotImplementedException();
}

void MinExtension::TopLayerExtensionHelper(float *property_array,
                                           int start_x, int start_z,
                                           int start_y, int end_x, int end_y,
                                           int end_z, int nx, int nz, int ny,
                                           uint boundary_length) {
    // Do nothing, no top layer to extend in random boundaries.
}

void MinExtension::TopLayerRemoverHelper(float *property_array, int start_x,
                                         int start_z, int start_y, int end_x,
                                         int end_y, int end_z, int nx,
                                         int nz, int ny,
                                         uint boundary_length) {
    // Do nothing, no top layer to remove in random boundaries.
}
