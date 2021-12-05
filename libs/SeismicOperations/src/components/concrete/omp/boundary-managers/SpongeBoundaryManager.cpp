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

#include <iostream>
#include <cmath>

#include <operations/components/independents/concrete/boundary-managers/SpongeBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>

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
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int lwnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lwnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

    uint bound_length = mpParameters->GetBoundaryLength();
    uint half_length = mpParameters->GetHalfLength();

    float *sponge_coefficients = mpSpongeCoefficients->GetNativePointer();
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(1)
        for (int iz = half_length + bound_length - 1; iz >= half_length; iz--) {
#pragma ivdep
            for (int ix = half_length + bound_length; ix <= lwnx - half_length - bound_length; ix++) {
                next[iz * wnx + ix] *=
                        sponge_coefficients[iz - half_length];
                next[(iz + wnz - 2 * iz - 1) * wnx + ix] *=
                        sponge_coefficients[iz - half_length];
            }
        }
    }
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(1)
        for (int iz = half_length + bound_length; iz <= lwnz - half_length - bound_length; iz++) {
#pragma ivdep
            for (int ix = half_length + bound_length - 1; ix >= half_length; ix--) {
                next[iz * wnx + ix] *=
                        sponge_coefficients[ix - half_length];
                next[iz * wnx + (ix + wnx - 2 * ix - 1)] *=
                        sponge_coefficients[ix - half_length];
            }
        }
    }

    int start_x = half_length;
    int end_x = lwnx - half_length;
    int start_z = half_length;
    int end_z = lwnz - half_length;
    // Zero-Corners in the boundaries wnx-wnz boundary intersection--boundaries not
    // needed.
#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static, 1) collapse(1)
        for (int row = 0; row < bound_length; row++) {
#pragma ivdep
            for (int column = 0; column < bound_length; column++) {
                /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                /*! Top left boundary in other words */
                next[(start_z + row) * wnx + column + start_x] *=
                        min(sponge_coefficients[column], sponge_coefficients[row]);
                /*!for values from z = wnz-half_length TO z =
                     * wnz-half_length-BOUND_LENGTH*/
                /*! and for x = half_length to x = half_length + BOUND_LENGTH */
                /*! Bottom left boundary in other words */
                next[(end_z - 1 - row) * wnx + column + start_x] *=
                        min(sponge_coefficients[column], sponge_coefficients[row]);
                /*!for values from z = half_length TO z = half_length +BOUND_LENGTH */
                /*! and for x = wnx-half_length to x = wnx-half_length - BOUND_LENGTH */
                /*! Top right boundary in other words */
                next[(start_z + row) * wnx + (end_x - 1 - column)] *=
                        min(sponge_coefficients[column], sponge_coefficients[row]);
                /*!for values from z = wnz-half_length TO z =
                     * wnz-half_length-BOUND_LENGTH*/
                /*! and for x = wnx-half_length to x = wnx - half_length - BOUND_LENGTH */
                /*! Bottom right boundary in other words */
                next[(end_z - 1 - row) * wnx + (end_x - 1 - column)] *=
                        min(sponge_coefficients[column], sponge_coefficients[row]);
            }
        }
    }
}