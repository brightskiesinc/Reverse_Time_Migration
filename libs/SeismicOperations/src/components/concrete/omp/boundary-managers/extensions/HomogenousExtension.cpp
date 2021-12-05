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

#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

HomogenousExtension::HomogenousExtension(bool use_top_layer) {
    this->mUseTop = use_top_layer;
}

void HomogenousExtension::VelocityExtensionHelper(float *property_array,
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
        /*!putting the nearest property_array adjacent to the boundary as the value
         * for all velocities at the boundaries for y and with all x and z */
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = start_z; row < end_z; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH*/
                    property_array[(depth + start_y) * nz_nx + row * nx + column] =
                            property_array[(boundary_length + start_y) * nz_nx + row * nx +
                                           column];
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx + column] =
                            property_array[(end_y - 1 - boundary_length) * nz_nx + row * nx +
                                           column];
                }
            }
        }
    }
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for x and with all z and y */
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = start_z; row < end_z; row++) {
            for (int column = 0; column < boundary_length; column++) {
                /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
                property_array[depth * nz_nx + row * nx + column + start_x] =
                        property_array[depth * nz_nx + row * nx + boundary_length +
                                       start_x];
                /*!for values from x = nx-HALF_LENGTH TO x =
                 * nx-HALF_LENGTH-BOUND_LENGTH*/
                property_array[depth * nz_nx + row * nx + (end_x - 1 - column)] =
                        property_array[depth * nz_nx + row * nx +
                                       (end_x - 1 - boundary_length)];
            }
        }
    }
    if (this->mUseTop) {
        /*!putting the nearest property_array adjacent to the boundary as the value
         * for all velocities at the boundaries for z and with all x and y */
        for (int depth = start_y; depth < end_y; depth++) {
            for (int row = 0; row < boundary_length; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                    property_array[depth * nz_nx + (start_z + row) * nx + column] =
                            property_array[depth * nz_nx + (start_z + boundary_length) * nx +
                                           column];
                    /*!for values from z = nz-HALF_LENGTH TO z =
                     * nz-HALF_LENGTH-BOUND_LENGTH*/
                    property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
                            property_array[depth * nz_nx +
                                           (end_z - 1 - boundary_length) * nx + column];
                }
            }
        }
    } else {
        /*!putting the nearest property_array adjacent to the boundary as the value
         * for all velocities at the boundaries for z and with all x and y */
        for (int depth = start_y; depth < end_y; depth++) {
            for (int row = 0; row < boundary_length; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from z = nz-HALF_LENGTH TO z =
                     * nz-HALF_LENGTH-BOUND_LENGTH*/
                    property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
                            property_array[depth * nz_nx +
                                           (end_z - 1 - boundary_length) * nx + column];
                }
            }
        }
    }
}

void HomogenousExtension::TopLayerExtensionHelper(float *property_array,
                                                  int start_x, int start_y, int start_z,
                                                  int end_x, int end_y, int end_z,
                                                  int nx, int ny, int nz, uint boundary_length) {
    if (this->mUseTop) {
        int nz_nx = nx * nz;
        /*!putting the nearest property_array adjacent to the boundary as the value
         * for all velocities at the boundaries for z and with all x and y */
        for (int depth = start_y; depth < end_y; depth++) {
            for (int row = 0; row < boundary_length; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                    property_array[depth * nz_nx + (start_z + row) * nx + column] =
                            property_array[depth * nz_nx + (start_z + boundary_length) * nx +
                                           column];
                }
            }
        }
    }
}

void HomogenousExtension::TopLayerRemoverHelper(float *property_array,
                                                int start_x, int start_y, int start_z,
                                                int end_x, int end_y, int end_z,
                                                int nx, int ny, int nz, uint boundary_length) {
    if (this->mUseTop) {
        int nz_nx = nx * nz;
        /*!putting the nearest property_array adjacent to the boundary as the value
         * for all velocities at the boundaries for z and with all x and y */
        for (int depth = start_y; depth < end_y; depth++) {
            for (int row = 0; row < boundary_length; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                    property_array[depth * nz_nx + (start_z + row) * nx + column] = 0;
                }
            }
        }
    }
}
