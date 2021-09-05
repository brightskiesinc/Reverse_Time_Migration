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

#ifndef OPERATIONS_LIB_UTILS_NOISE_FILTERING_H
#define OPERATIONS_LIB_UTILS_NOISE_FILTERING_H

namespace operations {
    namespace utils {
        namespace filters {

            typedef unsigned int uint;

            void filter_stacked_correlation(float *input_frame, float *output_frame,
                                            uint nx, uint ny, uint nz,
                                            float dx, float dz, float dy);

            void apply_laplace_filter(float *input_frame, float *output_frame,
                                      unsigned int nx, unsigned int ny, unsigned int nz);
        } //namespace filters
    } //namespace utils
} //namespace operations

#endif //OPERATIONS_LIB_UTILS_NOISE_FILTERING_H
