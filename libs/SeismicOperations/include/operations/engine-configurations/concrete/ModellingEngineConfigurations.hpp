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

#ifndef OPERATIONS_LIB_MODELLING_ENGINE_CONFIGURATION_HPP
#define OPERATIONS_LIB_MODELLING_ENGINE_CONFIGURATION_HPP

#include <operations/engine-configurations/interface/EngineConfigurations.hpp>

#include <operations/components/independents/primitive/BoundaryManager.hpp>
#include <operations/components/independents/primitive/ComputationKernel.hpp>
#include <operations/components/independents/primitive/ModelHandler.hpp>
#include <operations/components/independents/primitive/TraceWriter.hpp>
#include <operations/components/independents/primitive/SourceInjector.hpp>
#include <operations/components/independents/primitive/TraceManager.hpp>

#include <map>

namespace operations {
    namespace configurations {

        /**
         * @brief Class that contains pointers to concrete
         * implementations of some of the components
         * to be used in modelling engine
         *
         * @note ModellingEngineConfiguration is only used for modelling
         * and it doesn't have a forward collector
         * Because we don't need to store the forward propagation.
         * If we are modeling then we only do forward and store the traces while
         * propagating using the (TraceWriter) component we store the traces for each
         * shot in the trace file.(each shot's traces  in a different file)
         */
        class ModellingEngineConfigurations : public EngineConfigurations {
        public:
            /**
             * @brief Constructor
             **/
            ModellingEngineConfigurations() {
                this->mpModelHandler = nullptr;
                this->mpBoundaryManager = nullptr;
                this->mpSourceInjector = nullptr;
                this->mpComputationKernel = nullptr;
                this->mpTraceManager = nullptr;
                this->mpMemoryHandler = nullptr;
                this->mpTraceWriter = nullptr;
                this->mSortMin = -1;
                this->mSortMax = -1;
            }

            /**
             * @brief Destructor for correct destroying of the pointers
             **/
            ~ModellingEngineConfigurations() override {
                delete mpModelHandler;
                delete mpBoundaryManager;
                delete mpSourceInjector;
                delete mpComputationKernel;
                delete mpTraceWriter;
                delete mpTraceManager;
                delete mpMemoryHandler;
            }

            inline components::ModelHandler *GetModelHandler() const {
                return this->mpModelHandler;
            }

            void SetModelHandler(components::ModelHandler *apModelHandler) {
                this->mpModelHandler = apModelHandler;
                this->mpComponentsMap->Set(MODEL_HANDLER, this->mpModelHandler);
            }

            inline components::BoundaryManager *GetBoundaryManager() const {
                return this->mpBoundaryManager;
            }

            void SetBoundaryManager(components::BoundaryManager *apBoundaryManager) {
                this->mpBoundaryManager = apBoundaryManager;
                this->mpComponentsMap->Set(BOUNDARY_MANAGER, this->mpBoundaryManager);
            }

            inline components::SourceInjector *GetSourceInjector() const {
                return this->mpSourceInjector;
            }

            void SetSourceInjector(components::SourceInjector *apSourceInjector) {
                this->mpSourceInjector = apSourceInjector;
                this->mpComponentsMap->Set(SOURCE_INJECTOR, this->mpSourceInjector);
            }

            inline components::ComputationKernel *GetComputationKernel() const {
                return this->mpComputationKernel;
            }

            void SetComputationKernel(components::ComputationKernel *apComputationKernel) {
                this->mpComputationKernel = apComputationKernel;
                this->mpComponentsMap->Set(COMPUTATION_KERNEL, this->mpComputationKernel);

                /// Set Memory Handler accordingly.
                this->SetMemoryHandler(this->mpComputationKernel->GetMemoryHandler());
            }

            inline components::TraceManager *GetTraceManager() const {
                return this->mpTraceManager;
            }

            void SetTraceManager(components::TraceManager *apTraceManager) {
                this->mpTraceManager = apTraceManager;
                this->mpComponentsMap->Set(TRACE_MANAGER, this->mpTraceManager);
            }

            inline components::TraceWriter *GetTraceWriter() const {
                return this->mpTraceWriter;
            }

            inline void SetTraceWriter(components::TraceWriter *apTraceWriter) {
                this->mpTraceWriter = apTraceWriter;
                this->mpComponentsMap->Set(TRACE_WRITER, this->mpTraceWriter);
            }

            inline const std::map<std::string, std::string> &GetModelFiles() const {
                return this->mModelFiles;
            }

            inline void SetModelFiles(const std::map<std::string, std::string> &aModelFiles) {
                this->mModelFiles = aModelFiles;
            }

            inline const std::vector<std::string> &GetTraceFiles() const {
                return this->mTraceFiles;
            }

            inline void SetTraceFiles(const std::vector<std::string> &aTraceFiles) {
                this->mTraceFiles = aTraceFiles;
            }

            inline uint GetSortMin() const {
                return this->mSortMin;
            }

            inline void SetSortMin(uint aSortMin) {
                this->mSortMin = aSortMin;
            }

            inline uint GetSortMax() const {
                return this->mSortMax;
            }

            inline void SetSortMax(uint aSortMax) {
                this->mSortMax = aSortMax;
            }

            inline const std::string &GetSortKey() const {
                return this->mSortKey;
            }

            inline void SetSortKey(const std::string &aSortKey) {
                this->mSortKey = aSortKey;
            }

        private:
            void SetMemoryHandler(components::MemoryHandler *apMemoryHandler) {
                this->mpMemoryHandler = apMemoryHandler;
                this->mpDependentComponentsMap->Set(MEMORY_HANDLER, this->mpMemoryHandler);
            }

        private:
            /* Independent Components */

            components::ModelHandler *mpModelHandler;
            components::BoundaryManager *mpBoundaryManager;
            components::SourceInjector *mpSourceInjector;
            components::ComputationKernel *mpComputationKernel;
            components::TraceWriter *mpTraceWriter;
            components::TraceManager *mpTraceManager;

            /* Dependent Components */

            components::MemoryHandler *mpMemoryHandler;

            /// All model (i.e. parameters) files.
            /// @example Velocity, density, epsilon and delta phi and theta
            std::map<std::string, std::string> mModelFiles;

            /// Traces files are different files each file contains
            /// the traces that the modeling engine would use as meta
            /// data to generate shots.
            std::vector<std::string> mTraceFiles;

            /**
             * @brief shot_start_id and shot_end_id are to support the different formats,
             *
             * @example In .segy, you might have a file that has shots 0 to 200
             * while you only want to work on shots between 100-150 so in this case:
             *      -- shot_start_id = 100
             *      -- shot_end_id = 150
             * so those just specify the starting shot id (inclusive) and the ending shot
             * id (exclusive)
             */
            uint mSortMin;
            uint mSortMax;
            std::string mSortKey;
        };
    } //namespace configuration
} //namespace operations

#endif //OPERATIONS_LIB_MODELLING_ENGINE_CONFIGURATION_HPP
