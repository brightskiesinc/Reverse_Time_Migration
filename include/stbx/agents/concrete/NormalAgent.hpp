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


#ifndef PIPELINE_AGENTS_NORMAL_AGENT_HPP
#define PIPELINE_AGENTS_NORMAL_AGENT_HPP

#include <stbx/agents/interface/Agent.hpp>

namespace stbx {
    namespace agents {

/**
 * @brief
 */
        class NormalAgent : public Agent {

        public:
            /**
            * @brief Constructor.
            */
            NormalAgent() = default;

            /**
             * @brief Destructor.
             */
            ~NormalAgent() override;

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
            operations::dataunits::GridBox *Initialize() override;

            /**
             * @brief Preform all tasks need by the engine before migration.
             */
            void BeforeMigration() override;

            /**
             * @brief Preform all tasks need by the engine after migration.
             */
            void AfterMigration() override;

            /**
             * @brief Preform all tasks need by the engine
             * before finalizing migration.
             */
            void BeforeFinalize() override;

            /**
             * @brief Preform all tasks need by the engine
             * after finalizing migration.
             * @param[in] aMigrationData : MigrationData
             */
            operations::dataunits::MigrationData *AfterFinalize(
                    operations::dataunits::MigrationData *aMigrationData) override;

            bool HasNextShot() override;

            std::vector<uint> GetNextShot() override;

        private:
            uint mCount = 0;
        };
    }//namespace agents
}//namespace stbx

#endif //PIPELINE_AGENTS_NORMAL_AGENT_HPP
