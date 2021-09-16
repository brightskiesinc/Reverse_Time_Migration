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

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int lwnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lwny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lwnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    uint bound_length = mpParameters->GetBoundaryLength();
    uint half_length = mpParameters->GetHalfLength();

    int y_start = half_length + bound_length;
    int y_end = lwny - half_length - bound_length;
    if (wny == 1) {
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
                for (int ix = half_length + bound_length; ix <= lwnx - half_length - bound_length; ix++) {
                    next[iy * wnx * wnz + iz * wnx + ix] *=
                            sponge_coefficients[iz - half_length];
                    next[iy * wnx * wnz + (iz + wnz - 2 * iz - 1) * wnx + ix] *=
                            sponge_coefficients[iz - half_length];
                }
            }
        }
    }
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(2)
        for (int iy = y_start; iy < y_end; iy++) {
            for (int iz = half_length + bound_length; iz <= lwnz - half_length - bound_length; iz++) {
#pragma ivdep
                for (int ix = half_length + bound_length - 1; ix >= half_length; ix--) {
                    next[iy * wnx * wnz + iz * wnx + ix] *=
                            sponge_coefficients[ix - half_length];
                    next[iy * wnx * wnz + iz * wnx + (ix + wnx - 2 * ix - 1)] *=
                            sponge_coefficients[ix - half_length];
                }
            }
        }
    }
    if (wny > 1) {
#pragma omp parallel default(shared)
        {
#pragma omp for schedule(static, 1) collapse(2)
            for (int iy = half_length + bound_length - 1; iy >= half_length; iy--) {
                for (int iz = half_length + bound_length; iz <= lwnz - half_length - bound_length; iz++) {
#pragma ivdep
                    for (int ix = half_length + bound_length; ix <= lwnx - half_length - bound_length; ix++) {
                        next[iy * wnx * wnz + iz * wnx + ix] *=
                                sponge_coefficients[iy - half_length];
                        next[(iy + wny - 2 * iy - 1) * wnx * wnz + iz * wnx + ix] *=
                                sponge_coefficients[iy - half_length];
                    }
                }
            }
        }
    }
    int start_y = y_start;
    int end_y = y_end;
    int start_x = half_length;
    int end_x = lwnx - half_length;
    int start_z = half_length;
    int end_z = lwnz - half_length;
    int nz_nx = wnz * wnx;
    // Zero-Corners in the boundaries wnx-wnz boundary intersection--boundaries not
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
                    next[depth * nz_nx + (start_z + row) * wnx + column + start_x] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = wnz-half_length TO z =
                     * wnz-half_length-BOUND_LENGTH*/
                    /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                    /*! Bottom left boundary in other words */
                    next[depth * nz_nx + (end_z - 1 - row) * wnx + column + start_x] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                    /*! and for x = wnx-half_length to x = wnx-half_length - BOUND_LENGTH */
                    /*! Top right boundary in other words */
                    next[depth * nz_nx + (start_z + row) * wnx + (end_x - 1 - column)] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                    /*!for values from z = wnz-half_length TO z =
                     * wnz-half_length-BOUND_LENGTH*/
                    /*! and for x = wnx-half_length to x = wnx - half_length - BOUND_LENGTH */
                    /*! Bottom right boundary in other words */
                    next[depth * nz_nx + (end_z - 1 - row) * wnx + (end_x - 1 - column)] *=
                            min(sponge_coefficients[column], sponge_coefficients[row]);
                }
            }
        }
    }
    // If 3-D, zero corners in the y-x and y-z plans.
    if (wny > 1) {
        // Zero-Corners in the boundaries wny-wnz boundary intersection--boundaries
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
                        next[(depth + start_y) * nz_nx + (start_z + row) * wnx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = wnz-half_length TO z =
                         * wnz-half_length-BOUND_LENGTH*/
                        /*! and for y = half_length to y = half_length + BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + (end_z - 1 - row) * wnx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                        /*! and for y = wny-half_length to y = wny-half_length - BOUND_LENGTH */
                        next[(end_y - 1 - depth) * nz_nx + (start_z + row) * wnx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                        /*!for values from z = wnz-half_length TO z =
                         * wnz-half_length-BOUND_LENGTH */
                        /*! and for y = wny-half_length to y = wny - half_length - BOUND_LENGTH
                         */
                        next[(end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * wnx + column] *=
                                min(sponge_coefficients[depth], sponge_coefficients[row]);
                    }
                }
            }
        }
        // Zero-Corners in the boundaries wnx-wny boundary intersection--boundaries
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
                        next[(depth + start_y) * nz_nx + row * wnx + column + start_x] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = wny-half_length TO y =
                         * wny-half_length-BOUND_LENGTH*/
                        /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                        next[(end_y - 1 - depth) * nz_nx + row * wnx + column + start_x] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = half_length TO y = half_length +BOUND_LENGTH */
                        /*! and for x = wnx-half_length to x = wnx-half_length - BOUND_LENGTH */
                        next[(depth + start_y) * nz_nx + row * wnx + (end_x - 1 - column)] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                        /*!for values from y = wny-half_length TO y =
                         * wny-half_length-BOUND_LENGTH*/
                        /*! and for x = wnx-half_length to x = wnx - half_length - BOUND_LENGTH
                         */
                        next[(end_y - 1 - depth) * nz_nx + row * wnx + (end_x - 1 - column)] *=
                                min(sponge_coefficients[column], sponge_coefficients[depth]);
                    }
                }
            }
        }
    }
}
