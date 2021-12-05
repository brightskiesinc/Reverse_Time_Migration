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

#ifndef OPERATIONS_LIB_TRAVEL_DATA_UNITS_TIME_TABLE_HPP
#define OPERATIONS_LIB_TRAVEL_DATA_UNITS_TIME_TABLE_HPP

#include <operations/data-units/interface/DataUnit.hpp>
#include <operations/common/DataTypes.h>

namespace operations {
    namespace dataunits {

        class TravelTimeTable : public DataUnit {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            ~TravelTimeTable() = default;
        };
    } //namespace dataunits
} //namespace operations

#endif //OPERATIONS_LIB_TRAVEL_DATA_UNITS_TIME_TABLE_HPP