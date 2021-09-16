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

#ifndef OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP
#define OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP

#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::dataunits::axis;

namespace operations {
    namespace utils {
        namespace sampling {

            class Sampler {
            public:

                static void
                Resize(float *input, float *output,
                       Axis3D<unsigned int> *apInputGridBox, Axis3D<unsigned int> *apOutputGridBox,
                       ComputationParameters *apParameters);

                static void
                CalculateAdaptiveCellDimensions(GridBox *apGridBox,
                                                ComputationParameters *apParameters,
                                                int aMinimum_velocity, float aMaxFrequency);
            };

        } //namespace sampling
    } //namespace utils
} //namespace operations


#endif /* OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP */
