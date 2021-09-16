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


#ifndef PIPELINE_AGENTS_AGENT_HPP
#define PIPELINE_AGENTS_AGENT_HPP

#include <operations/engines/interface/Engine.hpp>

namespace stbx {
    namespace agents {
        /**
         * @brief After specifying which running approach is currently
         * being used (i.e. MPI/No MPI), Agent is responsible to preform
         * RTMEngine tasks accordingly.
         *
         * @note Should be inherited whenever a new approach is used
         */
        class Agent {
        public:
            /**
             * @brief Constructor should be overridden to
             * initialize needed  member variables.
             */
            Agent() = default;

            /**
             * @brief Destructor should be overridden to
             * ensure correct memory management.
             */
            virtual ~Agent() = default;

            /**
             * @brief Assign initialized engine to agent to use in
             * all functions.
             * @param aEngine : RTMEngine
             */
            inline void AssignEngine(operations::engines::Engine *aEngine) {
                mpEngine = aEngine;
            }

            /**
             * @brief Assign CLI arguments to agent to use in
             * all functions.
             * @param _argc
             * @param _argv
             */
            inline void AssignArgs(int _argc, char **_argv) {
                argc = _argc;
                argv = _argv;
            }

            /**
             * @brief Initialize member variables assigned to
             * each derived class.
             * <br>
             * Should always be called in each derived class as follows:-
             *      GridBox *gb = Agent::Initialize();
             * and return gb at the end of each derived Initialize function.
             *
             * @return GridBox*
             */
            virtual operations::dataunits::GridBox *Initialize() {
                return mpEngine->Initialize();
            }

            /**
             * @brief Preform all tasks need by the engine before migration.
             */
            virtual void BeforeMigration() = 0;

            /**
             * @brief Preform all tasks need by the engine after migration.
             */
            virtual void AfterMigration() = 0;

            /**
             * @brief Preform all tasks need by the engine
             * before finalizing migration.
             */
            virtual void BeforeFinalize() = 0;

            /**
             * @brief Preform all tasks need by the engine
             * after finalizing migration.
             * @param[in] apMigrationData : MigrationData *
             */
            virtual operations::dataunits::MigrationData *AfterFinalize(
                    operations::dataunits::MigrationData *apMigrationData) = 0;

            virtual bool HasNextShot() = 0;

            virtual std::vector<uint> GetNextShot() = 0;

            /**
             * @brief Preform migration full cycle.
             * @return aMigrationData : MigrationData
             */
            operations::dataunits::MigrationData *Execute() {
                operations::dataunits::GridBox *gb = Initialize();
                BeforeMigration();
                while (HasNextShot()) {
                    mpEngine->MigrateShots(GetNextShot(), gb);
                    AfterMigration();
                }
                BeforeFinalize();
                operations::dataunits::MigrationData *finalized_data = AfterFinalize(mpEngine->Finalize(gb));
                return finalized_data;
            }

        protected:
            /// Engine instance needed by agent to preform task upon
            operations::engines::Engine *mpEngine{};

            /// CLI arguments
            int argc{};
            char **argv{};
        };
    }//namespace agents
}//namespace stbx

#endif //PIPELINE_AGENTS_AGENT_HPP
