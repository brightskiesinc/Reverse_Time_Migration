//
// Created by zeyad-osama on 19/09/2020.
//

#ifndef OPERATIONS_LIB_DATA_UNIT_HPP
#define OPERATIONS_LIB_DATA_UNIT_HPP

namespace operations {
    namespace dataunits {

        enum REPORT_LEVEL {
            SIMPLE, VERBOSE
        };

        class DataUnit {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~DataUnit() {};
        };
    } //namespace dataunits
} //namespace operations

#endif //OPERATIONS_LIB_DATA_UNIT_HPP