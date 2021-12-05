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

#include <operations/components/independents/concrete/boundary-managers/extensions/MinExtension.hpp>

using namespace std;
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
    float target_velocity = 100000.0f, temp = 0;
    // In case of 2D
    if (ny == 1) {
        end_y = 1;
        start_y = 0;
        // Get maximum property_array value in 2D domain.
        for (int row = start_z + boundary_length; row < end_z - boundary_length;
             row++) {
            for (int column = start_x + boundary_length;
                 column < end_x - boundary_length; column++) {
                target_velocity =
                        min(target_velocity, property_array[row * nx + column]);
            }
        }
    } else {
        // Get maximum property_array value.
        for (int depth = start_y + boundary_length; depth < end_y - boundary_length;
             depth++) {
            for (int row = start_z + boundary_length; row < end_z - boundary_length;
                 row++) {
                for (int column = start_x + boundary_length;
                     column < end_x - boundary_length; column++) {
                    target_velocity =
                            min(target_velocity,
                                property_array[depth * nz_nx + row * nx + column]);
                }
            }
        }
        // general case for 3D
        /*!putting random values for velocities at the boundaries for y and with all
         * x and z */
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = start_z; row < end_z; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH*/
                    property_array[(depth + start_y) * nz_nx + row * nx + column] =
                            target_velocity;
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx + column] =
                            target_velocity;
                }
            }
        }
    }
    /*!putting random values for velocities at the boundaries for X and with all Y
     * and Z */
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = start_z; row < end_z; row++) {
            for (int column = 0; column < boundary_length; column++) {
                /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
                property_array[depth * nz_nx + row * nx + column + start_x] =
                        target_velocity;
                /*!for values from x = nx-HALF_LENGTH TO x =
                 * nx-HALF_LENGTH-BOUND_LENGTH*/
                property_array[depth * nz_nx + row * nx + (end_x - 1 - column)] =
                        target_velocity;
            }
        }
    }
    /*!putting random values for velocities at the boundaries for z and with all x
     * and y */
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = 0; row < boundary_length; row++) {
            for (int column = start_x; column < end_x; column++) {
                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                // Remove top layer boundary : give value as zero since having top layer
                // random boundaries will introduce too much noise.
                property_array[depth * nz_nx + (start_z + row) * nx + column] = 0;
                // If we want random, give this value :
                // property_array[depth * nz_nx + (start_z + boundary_length) * nx +
                // column] - temp;
                /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH*/
                property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
                        target_velocity;
            }
        }
    }
    uint offset = 0;
    // Random-Corners in the boundaries nx-nz boundary intersection at bottom--
    // top boundaries not needed.
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = 0; row < boundary_length; row++) {
            for (int column = 0; column < boundary_length; column++) {
                offset = min(row, column);
                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                /*! Top left boundary in other words */
                property_array[depth * nz_nx + (start_z + row) * nx + column +
                               start_x] = 0;
                /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                /*! Bottom left boundary in other words */
                property_array[depth * nz_nx + (end_z - 1 - row) * nx + column +
                               start_x] = target_velocity;
                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH */
                /*! Top right boundary in other words */
                property_array[depth * nz_nx + (start_z + row) * nx +
                               (end_x - 1 - column)] = 0;
                /*!for values from z = nz-HALF_LENGTH TO z =
                 * nz-HALF_LENGTH-BOUND_LENGTH*/
                /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH - BOUND_LENGTH */
                /*! Bottom right boundary in other words */
                property_array[depth * nz_nx + (end_z - 1 - row) * nx +
                               (end_x - 1 - column)] = target_velocity;
            }
        }
    }
    // If 3-D, zero corners in the y-x and y-z plans.
    if (ny > 1) {
        // Random-Corners in the boundaries ny-nz boundary intersection at bottom--
        // top boundaries not needed.
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = 0; row < boundary_length; row++) {
                for (int column = start_x; column < end_x; column++) {
                    offset = min(row, depth);
                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for y = HALF_LENGTH to y = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + (start_z + row) * nx +
                                   column] = 0;
                    /*!for values from z = nz-HALF_LENGTH TO z =
                     * nz-HALF_LENGTH-BOUND_LENGTH*/
                    /*! and for y = HALF_LENGTH to y = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + (end_z - 1 - row) * nx +
                                   column] = target_velocity;
                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for y = ny-HALF_LENGTH to y = ny-HALF_LENGTH - BOUND_LENGTH */
                    property_array[(end_y - 1 - depth) * nz_nx + (start_z + row) * nx +
                                   column] = 0;
                    /*!for values from z = nz-HALF_LENGTH TO z =
                     * nz-HALF_LENGTH-BOUND_LENGTH */
                    /*! and for y = ny-HALF_LENGTH to y = ny - HALF_LENGTH - BOUND_LENGTH
                     */
                    property_array[(end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * nx +
                                   column] = target_velocity;
                }
            }
        }
        // Zero-Corners in the boundaries nx-ny boundary intersection on the top
        // layer--boundaries not needed.
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = start_z; row < start_z + boundary_length; row++) {
                for (int column = 0; column < boundary_length; column++) {
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + row * nx + column +
                                   start_x] = 0;
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx + column +
                                   start_x] = 0;
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + row * nx +
                                   (end_x - 1 - column)] = 0;
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH - BOUND_LENGTH
                     */
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx +
                                   (end_x - 1 - column)] = 0;
                }
            }
        }
        // Random-Corners in the boundaries nx-ny boundary intersection.
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = start_z + boundary_length; row < end_z; row++) {
                for (int column = 0; column < boundary_length; column++) {
                    offset = min(column, depth);
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + row * nx + column +
                                   start_x] = target_velocity;
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH + BOUND_LENGTH */
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx + column +
                                   start_x] = target_velocity;
                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH */
                    /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH - BOUND_LENGTH */
                    property_array[(depth + start_y) * nz_nx + row * nx +
                                   (end_x - 1 - column)] = target_velocity;
                    /*!for values from y = ny-HALF_LENGTH TO y =
                     * ny-HALF_LENGTH-BOUND_LENGTH*/
                    /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH - BOUND_LENGTH
                     */
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx +
                                   (end_x - 1 - column)] = target_velocity;
                }
            }
        }
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
