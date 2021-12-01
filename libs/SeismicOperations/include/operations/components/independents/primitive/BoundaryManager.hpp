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

#ifndef OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGER_HPP
#define OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGER_HPP

#include <operations/components/independents/interface/Component.hpp>

#include <operations/common/DataTypes.h>

namespace operations {
    namespace components {

        /**
         * @brief Boundary Manager Interface. All concrete techniques for absorbing
         * boundary conditions should be implemented using this interface.
         */
        class BoundaryManager : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~BoundaryManager() {};

            /**
             * @brief This function will be responsible of applying the absorbing boundary
             * condition on the current pressure frame.
             *
             * @param[in] kernel_id
             * When provided with a kernel_id, this would lead to applying the boundaries
             * on the designated kernel_id
             *      <ul>
             *      <li>kernel_id == 0 -> Pressure</li>
             *      <li>kernel_id == 1 -> Particle velocity</li>
             *      </ul>
             */
            virtual void ApplyBoundary(uint kernel_id = 0) = 0;

            /**
             * @brief Extends the velocities/densities to the added boundary parts to the
             * velocity/density of the model appropriately. This is only called once after
             * the initialization of the model.
             */
            virtual void ExtendModel() = 0;

            /**
             * @brief Extends the velocities/densities to the added boundary parts to the
             * velocity/density of the model appropriately. This is called repeatedly with
             * before the forward propagation of each shot.
             */
            virtual void ReExtendModel() = 0;

            /**
             * @brief Adjusts the velocity/density of the model appropriately for the
             * backward propagation. Normally, but not necessary, this means removing
             * the top absorbing boundary layer. This is called repeatedly with before
             * the backward propagation of each shot.
             */
            virtual void AdjustModelForBackward() = 0;

            /**
             * @brief
             * Sets the boundary to work in adjoint mode.
             *
             * @param[in] aAdjoint
             * Boolean indicating if adjoint mode is active.
             */
            void SetAdjoint(bool aAdjoint) {
                this->mAdjoint = aAdjoint;
            }

        protected:
            /// Whether this boundary is running on adjoint fields or not.
            bool mAdjoint;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_BOUNDARY_MANAGER_HPP
