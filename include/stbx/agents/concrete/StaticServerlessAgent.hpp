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


#ifndef PIPELINE_AGENTS_STATIC_SERVERLESS_HPP
#define PIPELINE_AGENTS_STATIC_SERVERLESS_HPP

#if defined(USING_MPI)

#include <stbx/agents/interface/Agent.hpp>

#include <mpi.h>

namespace stbx {
    namespace agents {

        class StaticServerlessAgent : public Agent {
        public:
            StaticServerlessAgent();

            ~StaticServerlessAgent() override;

            operations::dataunits::GridBox *Initialize() override;

            void BeforeMigration() override;

            void AfterMigration() override;

            void BeforeFinalize() override;

            operations::dataunits::MigrationData *AfterFinalize(
                    operations::dataunits::MigrationData *apMigrationData) override;

            bool HasNextShot() override;

            std::vector<uint> GetNextShot() override;

        private:
            uint mCount;

            operations::dataunits::GridBox *mpGridBox;

            /// Current process rank is stored in self
            int self, mProcessCount;

            /// MPI communicator
            MPI_Comm mCommunication;

            /// Shots vector
            std::vector<uint> mPossibleShots;

            std::vector<float *> mStacks;
        };
    }//namespace agents
}//namespace stbx

#endif

#endif //PIPELINE_AGENTS_STATIC_SERVERLESS_HPP
