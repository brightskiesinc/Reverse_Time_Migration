//
// Created by zeyad-osama on 17/09/2020.
//

#ifndef OPERATIONS_LIB_TRAVEL_DATA_UNITS_TIME_TABLE_HPP
#define OPERATIONS_LIB_TRAVEL_DATA_UNITS_TIME_TABLE_HPP

#include "operations/data-units/interface/DataUnit.hpp"
#include "operations/common/DataTypes.h"

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
