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

#ifndef OPERATIONS_LIB_UTILS_WRITE_UTILS_H
#define OPERATIONS_LIB_UTILS_WRITE_UTILS_H

#include <operations/common/DataTypes.h>
#include <string>
#include <bs/io/api/cpp/BSIO.hpp>

namespace operations {
    namespace utils {
        namespace io {
            /**
             * @brief
             * Transform an array of data into a gather of traces for IO.
             *
             * @param[in] apData
             * The actual data array.
             *
             * @param[in] aNX
             * The number of traces in x-direction.
             *
             * @param[in] aNY
             * The number of traces in y-direction.
             *
             * @param[in] aNS
             * The number of samples.
             *
             * @param[in] aDX
             * Sampling in x-direction.
             *
             * @param[in] aDY
             * Sampling in y-direction.
             *
             * @param[in] aDS
             * Sampling of samples.
             *
             * @param[in] aSX
             * The starting x actual location.
             *
             * @param[in] aSY
             * The starting y actual location.
             *
             * @param[in] aOffsetX
             * The offset in x direction
             *
             * @param[in] aOffsetY
             * The offset in y direction.
             *
             * @param[in] aShots
             * The number of aShots.
             *
             * @param[in] aSpaceScale
             * The space sampling scaling for x and y.
             *
             * @param[in] aSampleScale
             * The sample sampling rate scaling.
             *
             * @return
             * A gather with the produced traces.
             */
            std::vector<bs::io::dataunits::Gather *> TransformToGather(const float *apData,
                                                                       uint aNX, uint aNY, uint aNS,
                                                                       float aDX, float aDY, float aDS,
                                                                       float aSX, float aSY,
                                                                       int aOffsetX, int aOffsetY,
                                                                       uint aShots,
                                                                       float aSpaceScale,
                                                                       float aSampleScale);

            /**
             * @brief
             * Transform an array of data into a gather of traces for IO.
             *
             * @param[in] apData
             * The actual data array.
             *
             * @param[in] aNX
             * The number of traces in x-direction.
             *
             * @param[in] aNY
             * The number of traces in y-direction.
             *
             * @param[in] aNS
             * The number of samples.
             *
             * @param[in] aDX
             * Sampling in x-direction.
             *
             * @param[in] aDY
             * Sampling in y-direction.
             *
             * @param[in] aDS
             * Sampling of samples.
             *
             * @param[in] apMetaDataGather
             * A gather containing the meta-data of each trace,
             * to be combined with the given data.
             *
             * @param[in] aShots
             * The number of aShots.
             *
             * @param[in] aSampleScale
             * The sample sampling rate scaling.
             *
             * @return
             * A gather with the produced traces.
             */
            std::vector<bs::io::dataunits::Gather *>
            TransformToGather(const float *apData,
                              uint aNX, uint aNY, uint aNS,
                              float aDS,
                              bs::io::dataunits::Gather *apMetaDataGather,
                              uint aShots,
                              float aSampleScale);
        } //namespace io
    } //namespace utils
} //namespace operations

#endif // OPERATIONS_LIB_UTILS_WRITE_UTILS_H
