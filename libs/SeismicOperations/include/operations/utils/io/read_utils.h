//
// Created by amr-nasr on 04/07/2020.
//

#ifndef OPERATIONS_LIB_UTILS_READ_UTILS_H
#define OPERATIONS_LIB_UTILS_READ_UTILS_H

#include <operations/common/DataTypes.h>
#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/data-units/concrete/holders/TracesHolder.hpp>
#include <thoth/api/thoth.hpp>

namespace operations {
    namespace utils {
        namespace io {
            void ParseGatherToTraces(thoth::dataunits::Gather *apGather, Point3D *apSource,
                                     dataunits::TracesHolder *apTraces,
                                     uint **x_position, uint **y_position,
                                     dataunits::GridBox *apGridBox,
                                     common::ComputationParameters *apParameters,
                                     float *total_time);
        } //namespace io
    } //namespace utils
} //namespace operations

#endif //OPERATIONS_LIB_UTILS_READ_UTILS_H
