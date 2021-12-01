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

#ifndef OPERATIONS_LIB_ENGINE_CONFIGURATIONS_PSTM_ENGINE_CONFIGURATION_HPP
#define OPERATIONS_LIB_ENGINE_CONFIGURATIONS_PSTM_ENGINE_CONFIGURATION_HPP

#include <operations/engine-configurations/interface/EngineConfigurations.hpp>
#include <operations/components/independents/primitive/MigrationAccommodator.hpp>
#include <operations/components/independents/primitive/ModelHandler.hpp>
#include <operations/components/independents/primitive/TraceManager.hpp>
#include <operations/components/dependents/primitive/MemoryHandler.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

namespace operations {
    namespace configurations {

        class PSTMEngineConfigurations : public EngineConfigurations {
        public:
            /**
             * @brief Constructor
             **/
            PSTMEngineConfigurations() {
                this->mpModelHandler = nullptr;
                this->mpMigrationAccommodator = nullptr;
                this->mpTraceManager = nullptr;
                this->mpMemoryHandler = nullptr;
                this->mSortMin = -1;
                this->mSortMax = -1;
            }

            /**
             * @brief Destructor for correct destroying of the pointers
             **/
            ~PSTMEngineConfigurations() override {
                delete mpModelHandler;
                delete mpMigrationAccommodator;
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

            inline components::MigrationAccommodator *GetMigrationAccommodator() const {
                return this->mpMigrationAccommodator;
            }

            void SetMigrationAccommodator(components::MigrationAccommodator *apMigrationAccommodator) {
                this->mpMigrationAccommodator = apMigrationAccommodator;
                this->mpComponentsMap->Set(MIGRATION_ACCOMMODATOR, this->mpMigrationAccommodator);

                this->SetMemoryHandler(new operations::components::WaveFieldsMemoryHandler(nullptr));
            }

            inline components::TraceManager *GetTraceManager() const {
                return this->mpTraceManager;
            }

            void SetTraceManager(components::TraceManager *apTraceManager) {
                this->mpTraceManager = apTraceManager;
                this->mpComponentsMap->Set(TRACE_MANAGER, this->mpTraceManager);
            }

            inline components::MemoryHandler *GetMemoryHandler() const {
                return this->mpMemoryHandler;
            }

            inline const std::map<std::string, std::string> &GetModelFiles() const {
                return mModelFiles;
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
            components::MigrationAccommodator *mpMigrationAccommodator;
            components::TraceManager *mpTraceManager;

            /* Dependent Components */

            components::MemoryHandler *mpMemoryHandler;

            /// All model (i.e. parameters) files.
            /// @example Velocity, density, epsilon and delta phi and theta
            std::map<std::string, std::string> mModelFiles;

            /// Traces files are different files each file contains
            /// the traces for one shot that may be output from the
            /// modeling engine
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

#endif //OPERATIONS_LIB_ENGINE_CONFIGURATIONS_PSTM_ENGINE_CONFIGURATION_HPP