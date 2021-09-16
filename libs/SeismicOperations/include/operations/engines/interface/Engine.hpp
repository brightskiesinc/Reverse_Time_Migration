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

#ifndef OPERATIONS_LIB_ENGINES_ENGINE_HPP
#define OPERATIONS_LIB_ENGINES_ENGINE_HPP

#include "operations/helpers/callbacks/primitive/CallbackCollection.hpp"
#include "operations/data-units/concrete/migration/MigrationData.hpp"


#include <vector>

namespace operations {
    namespace engines {
        /**
         * @note Each engine comes with it's own Timer and Logger. Logger
         * Channel should be initialized at each concrete implementation
         * and should be destructed at each destructor.
         */
        class Engine {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~Engine() {};

            /**
             * @brief Initializes domain model.
             */
            virtual dataunits::GridBox *Initialize() = 0;

            /**
             * @brief The function that filters and returns all possible
             * shot ID in a vector to be fed to the migrate method.
             *
             * @return[out]
             * A vector containing all unique shot IDs.
             */
            virtual std::vector<uint> GetValidShots() = 0;
            /// @todo GetValidGathers

            /**
             * @brief The migration function that will apply the
             * reverse time migration process and produce the results needed.
             *
             * @param[in] shot_list
             * A vector containing the shot IDs to be migrated.
             */
            virtual void MigrateShots(std::vector<uint> shot_numbers, dataunits::GridBox *apGridBox) = 0;
            /// @todo ProcessGathers

            /**
             * @brief Finalizes and terminates all processes
             *t();
        Logger->In
             * @return[out]
             * A float pointer to the array containing the final correlation result.
             */
            virtual dataunits::MigrationData *Finalize(dataunits::GridBox *apGridBox) = 0;

        protected:
            /// Callback collection to be called when not in release mode.
            helpers::callbacks::CallbackCollection *mpCallbacks;
            /// Computations parameters.
            common::ComputationParameters *mpParameters;
        };
    } //namespace engines
} //namespace operations

#endif //OPERATIONS_LIB_ENGINES_ENGINE_HPP
