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
