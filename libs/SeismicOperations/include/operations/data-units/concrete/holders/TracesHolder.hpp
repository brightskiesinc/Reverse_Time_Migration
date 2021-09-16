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

#ifndef OPERATIONS_LIB_DATA_UNITS_TRACES_HOLDER_HPP
#define OPERATIONS_LIB_DATA_UNITS_TRACES_HOLDER_HPP

#include <operations/data-units/interface/DataUnit.hpp>
#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

namespace operations {
    namespace dataunits {
        /**
         * @brief Class containing the available traces information.
         */
        class TracesHolder : public DataUnit {
        public:
            /**
             * @brief Constructor.
             */
            TracesHolder() {
                Traces = nullptr;
                PositionsX = nullptr;
                PositionsY = nullptr;
            }

            /**
             * @brief Destructor.
             */
            ~TracesHolder() override = default;

        public:
            dataunits::FrameBuffer<float> *Traces;

            uint *PositionsX;
            uint *PositionsY;

            uint ReceiversCountX;
            uint ReceiversCountY;

            uint TraceSizePerTimeStep;

            uint SampleNT;
            float SampleDT;
        };
    } //namespace dataunits
} //namespace operations

#endif //OPERATIONS_LIB_DATA_UNITS_TRACES_HOLDER_HPP
