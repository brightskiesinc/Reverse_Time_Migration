//
// Created by amr-nasr on 18/11/2019.
//

#include "operations/components/independents/concrete/boundary-managers/extensions/ZeroExtension.hpp"

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::dataunits;

void ZeroExtension::VelocityExtensionHelper(float *property_array,
                                            int start_x, int start_y, int start_z,
                                            int end_x, int end_y, int end_z,
                                            int nx, int ny, int nz,
                                            uint boundary_length) {
    /**
     * Change the values of velocities at boundaries (HALF_LENGTH excluded) to
     * zeros the start for x, y and z is at HALF_LENGTH and the end is at
     * (nx - HALF_LENGTH) or (ny - HALF_LENGTH) or (nz- HALF_LENGTH)
     */
    int nz_nx = nx * nz;

    // In case of 2D
    if (ny == 1) {
        start_y = 0;
        end_y = 1;
    } else {
        /// General case for 3D
        /**
         * Putting zero values for velocities at the boundaries for
         * y and with all x and z
         */
        for (int depth = 0; depth < boundary_length; depth++) {
            for (int row = start_z; row < end_z; row++) {
                for (int column = start_x; column < end_x; column++) {
                    /**
                     * For values from y = HALF_LENGTH
                     * TO y = HALF_LENGTH + BOUND_LENGTH
                     */
                    property_array[(depth + start_y) * nz_nx + row * nx + column] = 0;
                    /**
                     * For values from y = ny - HALF_LENGTH
                     * TO y = ny - HALF_LENGTH - BOUND_LENGTH
                     */
                    property_array[(end_y - 1 - depth) * nz_nx + row * nx + column] = 0;
                }
            }
        }
    }
    /**
     * Putting zero values for velocities at the boundaries for
     * X and with all Y and Z
     */
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = start_z; row < end_z; row++) {
            for (int column = 0; column < boundary_length; column++) {
                /**
                 * For values from x = HALF_LENGTH
                 * TO x= HALF_LENGTH + BOUND_LENGTH
                 */
                property_array[depth * nz_nx + row * nx + column + start_x] = 0;
                /**
                 * For values from x = nx - HALF_LENGTH
                 * TO x = nx - HALF_LENGTH - BOUND_LENGTH
                 */
                property_array[depth * nz_nx + row * nx + (end_x - 1 - column)] = 0;
            }
        }
    }
    /**
     * Putting zero values for velocities at the boundaries
     * for z and with all x and y
     */
    for (int depth = start_y; depth < end_y; depth++) {
        for (int row = 0; row < boundary_length; row++) {
            for (int column = start_x; column < end_x; column++) {
                /**
                 * For values from z = HALF_LENGTH
                 * TO z = HALF_LENGTH + BOUND_LENGTH
                 */
                property_array[depth * nz_nx + (start_z + row) * nx + column] = 0;
                /**
                 * For values from z = nz - HALF_LENGTH
                 * TO z = nz - HALF_LENGTH - BOUND_LENGTH
                 */
                property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] = 0;
            }
        }
    }
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
