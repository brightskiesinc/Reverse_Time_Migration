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

#ifndef OPERATIONS_LIB_UTILS_COMPRESSORS_COMPRESSOR_HPP
#define OPERATIONS_LIB_UTILS_COMPRESSORS_COMPRESSOR_HPP

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace operations {
    namespace utils {
        namespace compressors {
            class Compressor {
            public:
                Compressor() = default;

                ~Compressor() = default;

                /**
                 * @brief This is the main-entry point for all compression algorithms.
                 * Currently using a naive switch based differentiation of algorithm used
                 */
                static void Compress(float *array, int nx, int ny, int nz, int nt, double tolerance,
                                     unsigned int codecType, const char *filename,
                                     bool zfp_is_relative);

                /**
                 * @brief This is the main-entry point for all decompression algorithms.
                 * Currently using a naive switch based differentiation of algorithm used
                 */
                static void Decompress(float *array, int nx, int ny, int nz, int nt, double tolerance,
                                       unsigned int codecType, const char *filename,
                                       bool zfp_is_relative);
            };
        } //namespace compressors
    } //namespace utils
} //namespace operations

#endif //OPERATIONS_LIB_UTILS_COMPRESSORS_COMPRESSOR_HPP
