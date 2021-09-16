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
#ifndef OPERATIONS_LIB_COMPONENTS_MEMORY_HANDLERS_COMPUTATION_KERNELS_MEMORY_HANDLER_HPP
#define OPERATIONS_LIB_COMPONENTS_MEMORY_HANDLERS_COMPUTATION_KERNELS_MEMORY_HANDLER_HPP

#include <operations/components/dependents/primitive/MemoryHandler.hpp>

namespace operations {
    namespace components {

        class WaveFieldsMemoryHandler : public MemoryHandler {
        public:
            explicit WaveFieldsMemoryHandler(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~WaveFieldsMemoryHandler() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            /**
             * @brief Applies first touch initialization by accessing the given pointer in
             * the same way it will be accessed in the step function, and initializing the
             * values to 0.
             *
             * @param[in] ptr
             * The pointer that will be accessed for the first time.
             *
             * @param[in] GridBox
             * As it contains all the meta data needed
             * i.e. nx, ny and nz or wnx, wny and wnz
             *
             * @param[in] enable_window
             * Lets first touch know which sizes to compute upon.
             * i.e. Grid size or window size.
             *
             * @note Access elements in the same way used in the computation kernel step.
             */
            void FirstTouch(float *ptr, dataunits::GridBox *apGridBox, bool enable_window = false) override;

            /**
             * @brief Clone wave fields data from source GridBox to destination one.
             * Allocates the wave field in the destination GridBox.
             *
             * @param[in] _src
             * Grid Box to copy from.
             *
             * @param[in] _dst
             * Grid Box to copy to.
             *
             * @note Cloned wave fields should always be free for preventing
             * memory leakage.
             *
             * @see FreeWaveFields(dataunits:: GridBox *apGridBox)
             */
            void CloneWaveFields(dataunits::GridBox *_src, dataunits::GridBox *_dst);

            /**
             * @brief Copy wave fields data from source GridBox to destination one.
             *
             * @param[in] _src
             * Grid Box to copy from.
             *
             * @param[in] _dst
             * Grid Box to copy to.
             *
             * @note Both source and destination GridBoxes' wave fields should already
             * be allocated.
             */
            void CopyWaveFields(dataunits::GridBox *_src, dataunits::GridBox *_dst);

            /**
             * @param[in] apGridBox
             * Grid Box that has the wave fields to be freed.
             *
             * @see CloneWaveFields(dataunits:: GridBox *src, dataunits:: GridBox *dest)
             */
            void FreeWaveFields(dataunits::GridBox *apGridBox);

            void AcquireConfiguration() override;

        private:
            common::ComputationParameters *mpParameters = nullptr;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_MEMORY_HANDLERS_COMPUTATION_KERNELS_MEMORY_HANDLER_HPP
