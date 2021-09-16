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

#ifndef OPERATIONS_LIB_UTILS_INTERPOLATION_H
#define OPERATIONS_LIB_UTILS_INTERPOLATION_H

#include <operations/data-units/concrete/holders/TracesHolder.hpp>

namespace operations {
    namespace utils {
        namespace interpolation {

            class Interpolator {
            public:
                /**
                 * @brief Takes a Trace struct and modifies it's attributes to fit with the new
                 * actual_nt
                 * @param apTraceHolder            Traces struct
                 * @param actual_nt                To be interpolated
                 * @param total_time               Total time of forward propagation
                 * @param aInterpolation           Interpolation type
                 * @return Interpolated apTraceHolder in Traces struct [Not needed]
                 */
                static float *
                Interpolate(dataunits::TracesHolder *apTraceHolder, uint actual_nt, float total_time,
                            INTERPOLATION aInterpolation = NONE);

                static float *
                InterpolateLinear(dataunits::TracesHolder *apTraceHolder, uint actual_nt, float total_time);

                static void
                InterpolateTrilinear(float *old_grid, float *new_grid,
                                     int old_nx, int old_nz, int old_ny,
                                     int new_nx, int new_nz, int new_ny,
                                     int bound_length,
                                     int half_length);
            };

        } //namespace interpolation
    } //namespace utils
} //namespace operations

#endif // OPERATIONS_LIB_UTILS_INTERPOLATION_H
