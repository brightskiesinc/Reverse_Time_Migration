//
// Created by zeyad-osama on 25/09/2020.
//

#ifndef OPERATIONS_LIB_DATA_UNITS_MIGRATION_RESULT_HPP
#define OPERATIONS_LIB_DATA_UNITS_MIGRATION_RESULT_HPP

namespace operations {
    namespace dataunits {

        class Result {
        public:
            /**
             * @brief Constructor
             */
            explicit Result(float *data) {
                this->mpData = data;
            };

            /**
             * @brief Destructor.
             */
            ~Result() = default;

            float *GetData() {
                return this->mpData;
            }

            void SetData(float *data) {
                mpData = data;
            }

        private:
            float *mpData;
        };
    } //namespace dataunits
} //namespace operations

#endif //OPERATIONS_LIB_DATA_UNITS_MIGRATION_RESULT_HPP
