//
// Created by mirna-moawad on 11/21/19.
//

#include "operations/components/independents/concrete/boundary-managers/SpongeBoundaryManager.hpp"

#include "operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp"

#include <iostream>
#include <cmath>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

/// Based on
/// https://pubs.geoscienceworld.org/geophysics/article-abstract/50/4/705/71992/A-nonreflecting-boundary-condition-for-discrete?redirectedFrom=fulltext

void SpongeBoundaryManager::ApplyBoundaryOnField(float *next) {
    /**
     * Sponge boundary implementation.
     **/

    int nx = this->mpGridBox->GetActualWindowSize(X_AXIS);
    int ny = this->mpGridBox->GetActualWindowSize(Y_AXIS);
    int nz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

    int lnx = this->mpGridBox->GetLogicalWindowSize(X_AXIS);
    int lny = this->mpGridBox->GetLogicalWindowSize(Y_AXIS);
    int lnz = this->mpGridBox->GetLogicalWindowSize(Z_AXIS);

    uint bound_length = mpParameters->GetBoundaryLength();
    uint half_length = mpParameters->GetHalfLength();

    int y_start = half_length + bound_length;
    int y_end = lny - half_length - bound_length;
    if (ny == 1) {
        y_start = 0;
        y_end = 1;
    }

    float *sponge_coefficients = mpSpongeCoefficients->GetNativePointer();
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int iy = y_start; iy < y_end; iy++) {
            for (int iz = half_length + bound_length - 1; iz >= half_length; iz--) {
#pragma ivdep
                for (int ix = half_length + bound_length; ix <= lnx - half_length - bound_length; ix++) {
                    next[iy * nx * nz + iz * nx + ix] *=
                            sponge_coefficients[iz - half_length];
                    next[iy * nx * nz + (iz + nz - 2 * iz - 1) * nx + ix] *=
                            sponge_coefficients[iz - half_length];
                }
            }
        }
    }
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int iy = y_start; iy < y_end; iy++) {
            for (int iz = half_length + bound_length; iz <= lnz - half_length - bound_length; iz++) {
#pragma ivdep
                for (int ix = half_length + bound_length - 1; ix >= half_length; ix--) {
                    next[iy * nx * nz + iz * nx + ix] *=
                            sponge_coefficients[ix - half_length];
                    next[iy * nx * nz + iz * nx + (ix + nx - 2 * ix - 1)] *=
                            sponge_coefficients[ix - half_length];
                }
            }
        }
    }
    if (ny > 1) {
#pragma omp parallel default(shared)
        {
#pragma omp for schedule(static, 1) collapse(2)
            for (int iy = half_length + bound_length - 1; iy >= half_length; iy--) {
                for (int iz = half_length + bound_length; iz <= lnz - half_length - bound_length; iz++) {
#pragma ivdep
                    for (int ix = half_length + bound_length; ix <= lnx - half_length - bound_length; ix++) {
                        next[iy * nx * nz + iz * nx + ix] *=
                                sponge_coefficients[iy - half_length];
                        next[(iy + ny - 2 * iy - 1) * nx * nz + iz * nx + ix] *=
                                sponge_coefficients[iy - half_length];
                    }
                }
            }
        }
    }
    int start_y = y_start;
    int end_y = y_end;
    int start_x = half_length;
    int end_x = lnx - half_length;
    int start_z = half_length;
    int end_z = lnz - half_length;
    int nz_nx = nz * nx;
    // Zero-Corners in the boundaries nx-nz boundary intersection--boundaries not
    // needed.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int depth = start_y; depth < end_y; depth++) {
            for (int row = 0; row < bound_length; row++) {
#pragma ivdep
                for (int column = 0; column < bound_length; column++) {
                    /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                    /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                    /*! Top left boundary in other words */
                    next[depth * nz_nx + (start_z + row) * nx + column + start_x] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = nz-half_length TO z =
                     * nz-half_length-BOUND_LENGTH*/
                    /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                    /*! Bottom left boundary in other words */
                    next[depth * nz_nx + (end_z - 1 - row) * nx + column + start_x] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                    /*! and for x = nx-half_length to x = nx-half_length - BOUND_LENGTH */
                    /*! Top right boundary in other words */
                    next[depth * nz_nx + (start_z + row) * nx + (end_x - 1 - column)] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = nz-half_length TO z =
                     * nz-half_length-BOUND_LENGTH*/
                    /*! and for x = nx-half_length to x = nx - half_length - BOUND_LENGTH */
                    /*! Bottom right boundary in other words */
                    next[depth * nz_nx + (end_z - 1 - row) * nx + (end_x - 1 - column)] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                }
            }
        }
    }
    // If 3-D, zero corners in the y-x and y-z plans.
    if (ny > 1) {
        // Zero-Corners in the boundaries ny-nz boundary intersection--boundaries
        // not needed.
#pragma omp parallel default(shared)
        {
#pragma omp for schedule(static, 1) collapse(2)
            for (int depth = 0; depth < bound_length; depth++) {
                for (int row = 0; row < bound_length; row++) {
#pragma ivdep
                    for (int column = start_x; column < end_x; column++) {
                        /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                        /*! and for y = half_length to y = half_length + BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + (start_z + row) * nx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = nz-half_length TO z =
                         * nz-half_length-BOUND_LENGTH*/
                        /*! and for y = half_length to y = half_length + BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + (end_z - 1 - row) * nx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                        /*! and for y = ny-half_length to y = ny-half_length - BOUND_LENGTH */
                        next[(end_y - 1 - depth) * nz_nx + (start_z + row) * nx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = nz-half_length TO z =
                         * nz-half_length-BOUND_LENGTH */
                        /*! and for y = ny-half_length to y = ny - half_length - BOUND_LENGTH
                         */
                        next[(end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * nx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                    }
                }
            }
        }
        // Zero-Corners in the boundaries nx-ny boundary intersection--boundaries
        // not needed.
#pragma omp parallel default(shared)
        {
#pragma omp for schedule(static, 1) collapse(2)
            for (int depth = 0; depth < bound_length; depth++) {
                for (int row = start_z; row < end_z; row++) {
#pragma ivdep
                    for (int column = 0; column < bound_length; column++) {
                        /*!for values from y = half_length TO y = half_length +BOUND_LENGTH */
                        /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + row * nx + column + start_x] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = ny-half_length TO y =
                         * ny-half_length-BOUND_LENGTH*/
                        /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                        next[(end_y - 1 - depth) * nz_nx + row * nx + column + start_x] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = half_length TO y = half_length +BOUND_LENGTH */
                        /*! and for x = nx-half_length to x = nx-half_length - BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + row * nx + (end_x - 1 - column)] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = ny-half_length TO y =
                         * ny-half_length-BOUND_LENGTH*/
                        /*! and for x = nx-half_length to x = nx - half_length - BOUND_LENGTH
                         */
                        next[(end_y - 1 - depth) * nz_nx + row * nx + (end_x - 1 - column)] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                    }
                }
            }
        }
    }
}
