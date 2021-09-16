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

#ifndef OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H
#define OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H

#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/components/independents/interface/Component.hpp>

namespace operations {
    namespace components {
        namespace helpers {
            /**
             * @brief
             * Boundary saving helper class
             * Used for the storage, and restoration of
             * boundaries.
             */
            class BoundarySaver {
            public:
                /**
                 * @brief
                 * Default constructor.
                 */
                BoundarySaver();

                /**
                 * @brief
                 * Initialize the boundary saver to target a specific wave field.
                 *
                 * @param[in] aActiveKey
                 * The key of the wave field to store.
                 *
                 * @param[in] apInternalGridBox
                 * The internal gridbox used for the restoration process.
                 *
                 * @param[in] apMainGridBox
                 * The main gridbox of the system, used for saving process.
                 *
                 * @param[in] apParameters
                 * The computation parameters.
                 */
                void Initialize(dataunits::GridBox::Key aActiveKey,
                                dataunits::GridBox *apInternalGridBox,
                                dataunits::GridBox *apMainGridBox,
                                common::ComputationParameters *apParameters);

                /**
                 * @brief
                 * The current time step to be stored.
                 *
                 * @param[in] aStep
                 * The time step to be stored.
                 */
                void SaveBoundaries(uint aStep);

                /**
                 * @brief
                 * The current time step to be restored.
                 *
                 * @param[in] aStep
                 * The time step to be restored.
                 */
                void RestoreBoundaries(uint aStep);

                /**
                 * @brief
                 * Default destructor.
                 */
                ~BoundarySaver();

            private:
                /// The key to save on the boundaries.
                dataunits::GridBox::Key mKey;
                /// The saved boundaries.
                dataunits::FrameBuffer<float> mBackupBoundaries;
                /// The boundary size saved in a single time step.
                uint mBoundarySize;
                /// Internal Gridbox used for the restoration process.
                dataunits::GridBox *mpInternalGridBox;
                /// Main Gridbox used for the restoration process.
                dataunits::GridBox *mpMainGridBox;
                /// The computation parameters used for calculations.
                common::ComputationParameters *mpComputationParameters;
            };
        }//namespace helpers
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H
