//
// Created by zeyad-osama on 19/09/2020.
//

#ifndef OPERATIONS_LIB_DATA_UNITS_TRACES_HOLDER_HPP
#define OPERATIONS_LIB_DATA_UNITS_TRACES_HOLDER_HPP

#include "operations/data-units/interface/DataUnit.hpp"
#include "operations/common/DataTypes.h"

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
            float *Traces;

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
