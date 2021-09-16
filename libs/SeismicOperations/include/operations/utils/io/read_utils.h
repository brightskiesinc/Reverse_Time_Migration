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

#ifndef OPERATIONS_LIB_UTILS_READ_UTILS_H
#define OPERATIONS_LIB_UTILS_READ_UTILS_H

#include <operations/common/DataTypes.h>
#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/data-units/concrete/holders/TracesHolder.hpp>
#include <bs/io/api/cpp/BSIO.hpp>

namespace operations {
    namespace utils {
        namespace io {
            void ParseGatherToTraces(bs::io::dataunits::Gather *apGather, Point3D *apSource,
                                     dataunits::TracesHolder *apTraces,
                                     uint **x_position, uint **y_position,
                                     dataunits::GridBox *apGridBox,
                                     common::ComputationParameters *apParameters,
                                     float *total_time);

            bs::io::dataunits::Gather *CombineGather(std::vector<bs::io::dataunits::Gather *> &aGatherVector);

            void RemoveDuplicatesFromGather(bs::io::dataunits::Gather *apGather);

            bool IsLineGather(bs::io::dataunits::Gather *apGather);
        } //namespace io
    } //namespace utils
} //namespace operations

#endif //OPERATIONS_LIB_UTILS_READ_UTILS_H
