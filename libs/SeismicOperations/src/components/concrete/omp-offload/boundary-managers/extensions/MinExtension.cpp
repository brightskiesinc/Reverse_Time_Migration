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

#include <omp.h>
#include <algorithm>
#include <cstdlib>

#include <operations/components/independents/concrete/boundary-managers/extensions/MinExtension.hpp>
#include <operations/utils/checks/Checks.hpp>

using namespace std;
using namespace bs::base::exceptions;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;
using namespace operations::utils::checks;

void MinExtension::VelocityExtensionHelper(float *property_array,
                                           int start_x, int start_y, int start_z,
                                           int end_x, int end_y, int end_z,
                                           int nx, int ny, int nz,
                                           uint boundary_length)
{
    /*!
     * change the values of velocities at boundaries (HALF_LENGTH excluded) to
     * zeros the start for x , y and z is at HALF_LENGTH and the end is at (nx -
     * HALF_LENGTH) or (ny - HALF_LENGTH) or (nz- HALF_LENGTH)
     */

    // finding the gpu device
    int device_num = omp_get_default_device();

    if (is_device_not_exist())
    {
        throw DEVICE_NOT_FOUND_EXCEPTION();
    }

    int nz_nx = nx * nz;

    float *deviceMinValue = (float *)omp_target_alloc(1 * sizeof(float), device_num);

    float maxFloat = 100000.0f;
    Device::MemCpy(deviceMinValue, &maxFloat, sizeof(float), Device::COPY_HOST_TO_DEVICE);

    // Get maximum property_array value in 2D domain.
#pragma omp target is_device_ptr(property_array, deviceMinValue) device(device_num)
    for (int row = start_z + boundary_length; row < end_z - boundary_length;
         row++)
    {
        for (int column = start_x + boundary_length;
             column < end_x - boundary_length; column++)
        {
            deviceMinValue[0] =
                min(deviceMinValue[0], property_array[row * nx + column]);
        }
    }

    /*!putting random values for velocities at the boundaries for X and with all Y
     * and Z */
#pragma omp target is_device_ptr(property_array, deviceMinValue) device(device_num)
#pragma omp parallel for collapse(2)
    for (int row = start_z; row < end_z; row++)
    {
        for (int column = 0; column < boundary_length; column++)
        {
            /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
            property_array[row * nx + column + start_x] =
                deviceMinValue[0];
            /*!for values from x = nx-HALF_LENGTH TO x =
                * nx-HALF_LENGTH-BOUND_LENGTH*/
            property_array[row * nx + (end_x - 1 - column)] =
                deviceMinValue[0];
        }
    }

    /*!putting random values for velocities at the boundaries for z and with all x
     * and y */
#pragma omp target is_device_ptr(property_array, deviceMinValue) device(device_num)
#pragma omp parallel for collapse(2)
    for (int row = 0; row < boundary_length; row++)
    {
        for (int column = start_x; column < end_x; column++)
        {
            /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
            // Remove top layer boundary : give value as zero since having top layer
            // random boundaries will introduce too much noise.
            property_array[(start_z + row) * nx + column] = 0;
            // If we want random, give this value :
            // property_array[depth * nz_nx + (start_z + boundary_length) * nx +
            // column] - temp;
            /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH*/
            property_array[(end_z - 1 - row) * nx + column] =
                deviceMinValue[0];
        }
    }
}

void MinExtension::TopLayerExtensionHelper(float *property_array,
                                           int start_x, int start_z,
                                           int start_y, int end_x, int end_y,
                                           int end_z, int nx, int nz, int ny,
                                           uint boundary_length)
{
    // Do nothing, no top layer to extend in random boundaries.
}

void MinExtension::TopLayerRemoverHelper(float *property_array, int start_x,
                                         int start_z, int start_y, int end_x,
                                         int end_y, int end_z, int nx,
                                         int nz, int ny,
                                         uint boundary_length)
{
    // Do nothing, no top layer to remove in random boundaries.
}
