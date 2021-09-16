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


#include <operations/test-utils/NumberHelpers.hpp>

#include <limits>

namespace operations {
    namespace testutils {

        float calculate_norm(const float *mat, uint nx, uint nz, uint ny) {
            float sum = 0;
            uint nx_nz = nx * nz;
            for (int iy = 0; iy < ny; iy++) {
                for (int iz = 0; iz < nz; iz++) {
                    for (int ix = 0; ix < nx; ix++) {
                        float value = mat[iy * nx_nz + nx * iz + ix];
                        sum = sum + (value * value);
                    }
                }
            }
            return sqrtf(sum);
        }

        bool approximately_equal(float a, float b, float tolerance) {
            float epsilon = std::numeric_limits<float>::epsilon() + tolerance;
            return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon) ||
                   fabs(b - a) <= ((fabs(b) < fabs(a) ? fabs(a) : fabs(b)) * epsilon);
        }

        bool essentially_equal(float a, float b, float tolerance) {
            float epsilon = std::numeric_limits<float>::epsilon();
            return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
        }

        bool definitely_greater_than(double a, double b, double tolerance) {
            float epsilon = std::numeric_limits<double>::epsilon();
            return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
        }

        bool definitely_less_than(float a, float b, float tolerance) {
            float epsilon = std::numeric_limits<float>::epsilon();
            return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
        }

    } //namespace testutils
} //namespace operations