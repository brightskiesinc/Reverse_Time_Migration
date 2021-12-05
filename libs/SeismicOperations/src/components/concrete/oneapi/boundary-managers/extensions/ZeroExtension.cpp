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

#include <vector>

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/boundary-managers/extensions/ZeroExtension.hpp>

using namespace std;
using namespace cl::sycl;
using namespace bs::base::backend;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

void ZeroExtension::VelocityExtensionHelper(float *property_array,
                                            int start_x, int start_y, int start_z,
                                            int end_x, int end_y, int end_z,
                                            int nx, int ny, int nz,
                                            uint boundary_length) {
    /*!
     * change the values of velocities at boundaries (HALF_LENGTH excluded) to
     * zeros the start for x , y and z is at HALF_LENGTH and the end is at (nx -
     * HALF_LENGTH) or (ny - HALF_LENGTH) or (nz- HALF_LENGTH)
     */
    int nz_nx = nx * nz;

    // In case of 2D
    if (ny == 1) {
        end_y = 1;
        start_y = 0;
    } else {
        // general case for 3D
        /*!putting zero values for velocities at the boundaries for y and with all x
         * and z */
        vector<uint> gridDims = {(uint) end_x - start_x, boundary_length, (uint) end_z - start_z};
        vector<uint> blockDims = {1, 1, 1};
        auto configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
                int column = it.get_global_id(0) + start_x;
                int depth = it.get_global_id(1);
                int row = it.get_global_id(2) + start_z;

                /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH
                 * +BOUND_LENGTH*/
                property_array[(depth + start_y) * nz_nx + row * nx + column] = 0;
                /*!for values from y = ny-HALF_LENGTH TO y =
                 * ny-HALF_LENGTH-BOUND_LENGTH*/
                property_array[(end_y - 1 - depth) * nz_nx + row * nx + column] = 0;
            });
        });
    }
    /*!putting zero values for velocities at the boundaries for X and with all Y
     * and Z */
    vector<uint> gridDims{boundary_length, (uint) end_y - start_y, (uint) end_z - start_z};
    vector<uint> blockDims{1, 1, 1};
    auto configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

        cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
            int column = it.get_global_id(0);
            int depth = it.get_global_id(1) + start_y;
            int row = it.get_global_id(2) + start_z;

            /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH*/
            property_array[depth * nz_nx + row * nx + column + start_x] = 0;
            /*!for values from y = ny-HALF_LENGTH TO y =
             * ny-HALF_LENGTH-BOUND_LENGTH*/
            property_array[depth * nz_nx + row * nx + (end_x - 1 - column)] = 0;
        });
    });
    /*!putting zero values for velocities at the boundaries for z and with all x
     * and y */
    gridDims = {(uint) end_x - start_x, (uint) end_y - start_y, boundary_length};
    configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

        cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
            int column = it.get_global_id(0) + start_x;
            int depth = it.get_global_id(1) + start_y;
            int row = it.get_global_id(2);

            /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH*/
            property_array[depth * nz_nx + (start_z + row) * nx + column] = 0;
            /*!for values from y = ny-HALF_LENGTH TO y =
             * ny-HALF_LENGTH-BOUND_LENGTH*/
            property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] = 0;
        });
    });
}

void ZeroExtension::TopLayerExtensionHelper(float *property_array,
                                            int start_x, int start_y, int start_z,
                                            int end_x, int end_y, int end_z,
                                            int nx, int ny, int nz, uint boundary_length) {
    // Do nothing, no top layer to extend in random boundaries.
}

void ZeroExtension::TopLayerRemoverHelper(float *property_array, int start_x,
                                          int start_z, int start_y,
                                          int end_x, int end_y, int end_z,
                                          int nx, int nz, int ny,
                                          uint boundary_length) {
    // Do nothing, no top layer to remove in random boundaries.
}
