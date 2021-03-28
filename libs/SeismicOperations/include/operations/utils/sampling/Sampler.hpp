//
// Created by ahmed-ayyad on 17/01/2021.
//

#ifndef OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP
#define OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP

#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

using namespace operations::dataunits;
using namespace operations::common;

namespace operations {
    namespace utils {
        namespace sampling {

            class Sampler {
            public:
                static void
                Resize(
                        float *input, float *output,
                        GridSize *apInputGridBox, GridSize *apOutputGridBox,
                        ComputationParameters *apParameters);

                static void
                CalculateAdaptiveCellDimensions(GridBox *apGridBox,
                                                ComputationParameters *apParameters,
                                                int minimum_velocity);
            };

        } //namespace sampling
    } //namespace utils
} //namespace operations


#endif /* OPERATIONS_LIB_UTILS_UTILS_SAMPLING_SAMPLER_HPP */
