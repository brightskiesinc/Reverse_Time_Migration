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

#ifndef OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNEL_HPP
#define OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNEL_HPP

#include <operations/components/independents/interface/Component.hpp>
#include <operations/components/dependents/primitive/MemoryHandler.hpp>
#include <operations/common/DataTypes.h>

#include "BoundaryManager.hpp"

namespace operations {
    namespace components {
        /**
         * The different kernel modes supported by the computation kernels.
         */
        enum class KERNEL_MODE {
            FORWARD,
            INVERSE,
            ADJOINT
        };

        /**
         * @brief Computation Kernel Interface. All concrete techniques for
         * solving different wave equations should be implemented using this interface.
         */
        class ComputationKernel : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~ComputationKernel() {};

            /**
             * @brief This function should solve the wave equation.  It calculates the next
             * time step from previous and current frames.
             * <br>
             * It should also update the GridBox structure so that after the function call,
             * the GridBox structure current frame should point to the newly calculated result,
             * the previous frame should point to the current frame at the time of the
             * function call. The next frame should point to the previous frame at the time of
             * the function call.
             */
            virtual void Step() = 0;

            /**
             * @brief Set kernel boundary manager to be used and called internally.
             *
             * @param[in] apBoundaryManager
             * The boundary manager to be used.
             */
            void SetBoundaryManager(BoundaryManager *apBoundaryManager) {
                this->mpBoundaryManager = apBoundaryManager;
            }

            /**
             * @return[out] The memory handler used.
             */
            virtual MemoryHandler *GetMemoryHandler() {
                return this->mpMemoryHandler;
            }

            /**
             * @brief
             * The operating mode of the kernel.
             *
             * @param[in] aMode
             * The mode to run the kernel in.
             */
            virtual void SetMode(KERNEL_MODE aMode) {
                this->mMode = aMode;
                if (this->mpBoundaryManager != nullptr) {
                    if (this->mMode == KERNEL_MODE::ADJOINT) {
                        this->mpBoundaryManager->SetAdjoint(true);
                    } else {
                        this->mpBoundaryManager->SetAdjoint(false);
                    }
                }
            }

            /**
             * @brief All pre-processing needed to be done on the model before the
             * beginning of the computations, should be applied in this function.
             * It should do all pre-processing it needs to perform its operations.
             */
            virtual void PreprocessModel() = 0;

        protected:
            /// Boundary Manager instance to be used by the step function.
            BoundaryManager *mpBoundaryManager;

            /// Memory Handler instance to be used for all memory
            /// handling (i.e. First touch)
            MemoryHandler *mpMemoryHandler;

            /// The kernel operating mode.
            KERNEL_MODE mMode;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNEL_HPP
