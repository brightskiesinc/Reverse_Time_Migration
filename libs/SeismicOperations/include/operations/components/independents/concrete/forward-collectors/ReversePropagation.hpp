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

#ifndef OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_REVERSE_PROPAGATION_HPP
#define OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_REVERSE_PROPAGATION_HPP

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>
#include <operations/components/independents/primitive/ForwardCollector.hpp>
#include <operations/components/dependency/concrete/HasDependents.hpp>

#include <operations/components/independents/primitive/ComputationKernel.hpp>
#include <bs/base/memory/MemoryManager.hpp>

#include <cstdlib>
#include <cstring>
#include <operations/components/independents/concrete/forward-collectors/boundary-saver/BoundarySaver.h>

namespace operations {
    namespace components {

        class ReversePropagation : public ForwardCollector,
                                   public dependency::HasDependents {
        public:
            explicit ReversePropagation(bs::base::configurations::ConfigurationMap *apConfigurationMap);

            ~ReversePropagation() override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void SetDependentComponents(
                    operations::helpers::ComponentsMap<DependentComponent> *apDependentComponentsMap) override;

            void FetchForward() override;

            void SaveForward() override;

            void ResetGrid(bool is_forward_run) override;

            dataunits::GridBox *GetForwardGrid() override;

            void AcquireConfiguration() override;

        private:
            /**
             * @brief Used by reverse injection propagation mode.
             */
            void Inject();

        private:
            common::ComputationParameters *mpParameters = nullptr;

            ComputationKernel *mpComputationKernel = nullptr;

            WaveFieldsMemoryHandler *mpWaveFieldsMemoryHandler = nullptr;

            /*
             * Propagation Properties.
             */

            dataunits::GridBox *mpMainGridBox = nullptr;

            dataunits::GridBox *mpInternalGridBox = nullptr;

            /*
             * Injection Properties.
             */

            bool mInjectionEnabled;

            std::vector<components::helpers::BoundarySaver *> mBoundarySavers;

            uint mTimeStep;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_REVERSE_PROPAGATION_HPP
