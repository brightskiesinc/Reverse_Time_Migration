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

#ifndef OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_HPP
#define OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_HPP

#include "operations/components/independents/interface/Component.hpp"

#include "operations/common/DataTypes.h"
#include "operations/data-units/concrete/migration/MigrationData.hpp"
#include <operations/components/independents/primitive/ComputationKernel.hpp>

#include <string>
#include <vector>
#include <map>

namespace operations {
    namespace components {

        /**
         * @brief Model Handler Interface. All concrete techniques for loading
         * or setting models should be implemented using this interface.
         */
        class ModelHandler : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~ModelHandler() {};

            /**
             * @brief This function should do the following:
             * <ul>
             *      <li>
             *      Create a new GridBox with all its frames
             *      (previous, current, next) allocated in memory.
             *      </li>
             *      <li>
             *      The models are loaded into the frame.
             *      </li>
             *      <li>
             *      This should account for the allocation of the boundaries around
             *      our domain(top, bottom, left, right).
             *      </li>
             *      <li>
             *      It should set all possible GridBox properties to be ready for
             *      actual computations.
             *      </li>
             * </ul>
             *
             * @param[in] files_names
             * The filenames vector of the files containing the model, the first filename
             * in the vector should be the velocity file, the second should be the density
             * file name.
             *
             * @param[in] apComputationKernel
             * The computation kernel to be used for first touch.
             *
             * @return[out]
             * GridBox object that was allocated, and setup appropriately.
             */
            virtual dataunits::GridBox *ReadModel(std::map<std::string, std::string> files_names) = 0;

            /**
             * @brief Setup the window properties if needed by copying the
             * needed window from the full model.
             */
            virtual void SetupWindow() = 0;

            /**
             * @brief
             * Prepares the output migration data to be returned to the user.
             * Eg: undoing everything the model handler has, from padding addtions
             * to resampling.
             *
             * @param[in] apMigrationData
             * The migration data generated to be post processed.
             */
            virtual void PostProcessMigration(dataunits::MigrationData *apMigrationData) = 0;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_MODEL_HANDLER_HPP
