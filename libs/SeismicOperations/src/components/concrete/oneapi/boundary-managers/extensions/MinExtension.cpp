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

#include <algorithm>
#include <cstdlib>
#include <vector>

#include <operations/components/independents/concrete/boundary-managers/extensions/MinExtension.hpp>
#include <bs/base/backend/Backend.hpp>

using namespace std;
using namespace cl::sycl;
using namespace bs::base::backend;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

void MinExtension::VelocityExtensionHelper(float *property_array,
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
    float min_velocity = MAXFLOAT;

    float *dev_min_velocity = (float *) cl::sycl::malloc_device(
            sizeof(float) * 1,
            Backend::GetInstance()->GetDeviceQueue()->get_device(),
            Backend::GetInstance()->GetDeviceQueue()->get_context());
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.memcpy(dev_min_velocity, &min_velocity, sizeof(float) * 1);
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
    // In case of 2D
    if (ny == 1) {
        end_y = 1;
        start_y = 0;
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.single_task([=]() {
                // Get maximum property_array value
                for (int row = start_z + boundary_length; row < end_z - boundary_length;
                     row++) {
                    for (int column = start_x + boundary_length;
                         column < end_x - boundary_length; column++) {
                        if (dev_min_velocity[0] > property_array[row * nx + column]) {
                            dev_min_velocity[0] = property_array[row * nx + column];
                        }
                    }
                }
            });
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.memcpy(&min_velocity, dev_min_velocity, sizeof(float) * 1);
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
    } else {
        // Get maximum property_array value.
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.single_task([=]() {
                // Get maximum property_array_value
                for (int depth = start_y + boundary_length;
                     depth < end_y - boundary_length; depth++) {
                    for (int row = start_z + boundary_length;
                         row < end_z - boundary_length; row++) {
                        for (int column = start_x + boundary_length;
                             column < end_x - boundary_length; column++) {
                            if (dev_min_velocity[0] >
                                property_array[depth * nz_nx + row * nx + column]) {
                                dev_min_velocity[0] =
                                        property_array[depth * nz_nx + row * nx + column];
                            }
                        }
                    }
                }
            });
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.memcpy(&min_velocity, dev_min_velocity, sizeof(float) * 1);
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
        /*!putting random values for velocities at the boundaries for y and with all
         * x and z */
        vector<uint> gridDims{(uint) end_x - start_x, boundary_length, (uint) end_z - start_z};
        vector<uint> blockDims{1, 1, 1};
        auto configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
                int column = it.get_global_id(0) + start_x;
                int depth = it.get_global_id(1);
                int row = it.get_global_id(2) + start_z;

                /*! Create temporary value */
                float temp =
                        min_velocity;
                /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
                int p_idx = (depth + start_y) * nz_nx + row * nx + column;
                property_array[p_idx] = temp;
                /*! Create temporary value */
                temp = min_velocity;
                /*!for values from x = nx-HALF_LENGTH TO x =
                 * nx-HALF_LENGTH-BOUND_LENGTH*/
                p_idx = (end_y - 1 - depth) * nz_nx + row * nx + column;
                property_array[p_idx] = temp;
            });
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
    }
    /*!putting random values for velocities at the boundaries for X and with all Y
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

            /*! Create temporary value */
            float temp = min_velocity;
            /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
            int p_idx = depth * nz_nx + row * nx + column + start_x;
            property_array[p_idx] = temp;
            /*! Create temporary value */
            temp = min_velocity;
            /*!for values from x = nx-HALF_LENGTH TO x =
             * nx-HALF_LENGTH-BOUND_LENGTH*/
            p_idx = depth * nz_nx + row * nx + (end_x - 1 - column);
            property_array[p_idx] = temp;
        });
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();

    /*!putting random values for velocities at the boundaries for z and with all x
     * and y */
    gridDims = {(uint) end_x - start_x, (uint) end_y - start_y, boundary_length};
    configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

        cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
            int column = it.get_global_id(0) + start_x;
            int depth = it.get_global_id(1) + start_y;
            int row = it.get_global_id(2);

            /*! Create temporary value */
            float temp = min_velocity;
            /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
            int p_idx = depth * nz_nx + (start_z + row) * nx + column;
            // Remove top layer boundary : give value as zero since having top
            // layer random boundaries will introduce too much noise.
            property_array[p_idx] = 0; //_abs(property_array[p2_idx] - temp);
            /*! Create temporary value */
            temp = min_velocity;
            /*!for values from x = nx-HALF_LENGTH TO x =
             * nx-HALF_LENGTH-BOUND_LENGTH*/
            p_idx = depth * nz_nx + (end_z - 1 - row) * nx + column;
            property_array[p_idx] = temp;
        });
    });

    Backend::GetInstance()->GetDeviceQueue()->wait();

    // Random-Corners in the boundaries nx-nz boundary intersection at bottom--
    // top boundaries not needed.
    gridDims = {(uint) boundary_length, (uint) end_y - start_y, boundary_length};
    configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
    Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

        cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
            int column = it.get_global_id(0);
            int depth = it.get_global_id(1) + start_y;
            int row = it.get_global_id(2);

            uint offset = std::min(row, column);
            /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
            /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
            /*! Top left boundary in other words */
            property_array[depth * nz_nx + (start_z + row) * nx + column +
                           start_x] = 0;
            /*!for values from z = nz-HALF_LENGTH TO z =
             * nz-HALF_LENGTH-BOUND_LENGTH*/
            /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
            /*! Bottom left boundary in other words */
            float temp = min_velocity;
            int p_idx = depth * nz_nx + (end_z - 1 - row) * nx + column + start_x;
            property_array[p_idx] = temp;
            /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
            /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH */
            /*! Top right boundary in other words */
            property_array[depth * nz_nx + (start_z + row) * nx +
                           (end_x - 1 - column)] = 0;
            /*!for values from z = nz-HALF_LENGTH TO z =
             * nz-HALF_LENGTH-BOUND_LENGTH*/
            /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH - BOUND_LENGTH
             */
            /*! Bottom right boundary in other words */
            temp = min_velocity;
            /*!for values from x = nx-HALF_LENGTH TO x =
             * nx-HALF_LENGTH-BOUND_LENGTH*/
            p_idx = depth * nz_nx + (end_z - 1 - row) * nx + (end_x - 1 - column);
            property_array[p_idx] = temp;
        });
    });

    Backend::GetInstance()->GetDeviceQueue()->wait();

    // If 3-D, zero corners in the y-x and y-z plans.
    if (ny > 1) {
        // Random-Corners in the boundaries ny-nz boundary intersection at bottom--
        // top boundaries not needed.
        vector<uint> gridDims{(uint) end_x - start_x, boundary_length, boundary_length};
        vector<uint> blockDims{1, 1, 1};
        auto configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
                int column = it.get_global_id(0) + start_x;
                int depth = it.get_global_id(1);
                int row = it.get_global_id(2);

                uint offset = std::min(row, depth);
                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for y = HALF_LENGTH to y = HALF_LENGTH + BOUND_LENGTH */
                property_array[(depth + start_y) * nz_nx + (start_z + row) * nx +
                               column] = 0;
                /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for y = HALF_LENGTH to y = HALF_LENGTH + BOUND_LENGTH */
                float temp = min_velocity;
                int p_idx =
                        (depth + start_y) * nz_nx + (end_z - 1 - row) * nx + column;
                property_array[p_idx] = temp;
                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for y = ny-HALF_LENGTH to y = ny-HALF_LENGTH - BOUND_LENGTH
                 */
                property_array[(end_y - 1 - depth) * nz_nx + (start_z + row) * nx +
                               column] = 0;
                /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH */
                /*! and for y = ny-HALF_LENGTH to y = ny - HALF_LENGTH -
                 * BOUND_LENGTH */
                temp = min_velocity;
                /*!for values from x = nx-HALF_LENGTH TO x =
                 * nx-HALF_LENGTH-BOUND_LENGTH*/
                p_idx =
                        (end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * nx + column;
                property_array[p_idx] = temp;
            });
        });

        Backend::GetInstance()->GetDeviceQueue()->wait();

        // Zero-Corners in the boundaries nx-ny boundary intersection on the top
        // layer--boundaries not needed.
        gridDims = {boundary_length, boundary_length, boundary_length};
        configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
                int column = it.get_global_id(0);
                int depth = it.get_global_id(1);
                int row = it.get_global_id(2) + start_z;

                /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                property_array[(depth + start_y) * nz_nx + row * nx + column +
                               start_x] = 0;
                /*!for values from y = ny-HALF_LENGTH TO y =
                 * ny-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                property_array[(end_y - 1 - depth) * nz_nx + row * nx + column +
                               start_x] = 0;
                /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH
                 */
                property_array[(depth + start_y) * nz_nx + row * nx +
                               (end_x - 1 - column)] = 0;
                /*!for values from y = ny-HALF_LENGTH TO y =
                 * ny-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH -
                 * BOUND_LENGTH */
                property_array[(end_y - 1 - depth) * nz_nx + row * nx +
                               (end_x - 1 - column)] = 0;
            });
        });

        Backend::GetInstance()->GetDeviceQueue()->wait();

        // Random-Corners in the boundaries nx-ny boundary intersection.
        gridDims = {boundary_length, boundary_length, boundary_length};
        configs = Backend::GetInstance()->CreateKernelConfiguration(gridDims, blockDims);
        Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            auto global_nd_range = nd_range<3>(configs.mGridDimensions, configs.mBlockDimensions);

            cgh.parallel_for(global_nd_range, [=](nd_item<3> it) {
                int column = it.get_global_id(0);
                int depth = it.get_global_id(1);
                int row = it.get_global_id(2) + start_z;

                uint offset = std::min(column, depth);
                /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                float temp = min_velocity;
                property_array[(depth + start_y) * nz_nx + row * nx + column +
                               start_x] = temp;
                /*!for values from y = ny-HALF_LENGTH TO y =
                 * ny-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                temp = min_velocity;
                property_array[(end_y - 1 - depth) * nz_nx + row * nx + column +
                               start_x] = temp;
                /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH
                 */
                /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH
                 */
                temp = min_velocity;
                property_array[(depth + start_y) * nz_nx + row * nx +
                               (end_x - 1 - column)] = temp;
                /*!for values from y = ny-HALF_LENGTH TO y =
                 * ny-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH -
                 * BOUND_LENGTH */
                temp = min_velocity;
                property_array[(end_y - 1 - depth) * nz_nx + row * nx +
                               (end_x - 1 - column)] = temp;
            });
        });
        Backend::GetInstance()->GetDeviceQueue()->wait();
    }
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
