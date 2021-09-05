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

#ifndef PIPELINE_AGENTS_DYNAMIC_SERVERLESS_HPP
#define PIPELINE_AGENTS_DYNAMIC_SERVERLESS_HPP

#if defined(USING_MPI)

#include <stbx/agents/interface/Agent.hpp>

#include <mpi.h>

namespace stbx {
    namespace agents {

        class DynamicServerlessAgent : public Agent {
        public:
            DynamicServerlessAgent() = default;

            ~DynamicServerlessAgent() override;

            operations::dataunits::GridBox *Initialize() override;

            void BeforeMigration() override;

            void AfterMigration() override;

            void BeforeFinalize() override;

            operations::dataunits::MigrationData *AfterFinalize(
                    operations::dataunits::MigrationData *apMigrationData) override;

            bool HasNextShot() override;

            std::vector<uint> GetNextShot() override;

        private:
            uint mCount = 0;

            /// Current process rank is stored in self
            int self, mProcessCount;

            /// MPI communicator
            MPI_Comm mCommunication;

            /// Reception operation status
            MPI_Status mStatus;

            /// Original shot size
            uint mShotsSize;

            uint mMasterCurrentShotID;

            /// Current shot ID to be processed
            int mShotTracker = 0;

            /// Indicate the first shots iterators
            int mShotsCount = 0;

            /// All shots vector
            std::vector<uint> mPossibleShots;

            /// Shots per each process
            std::vector<uint> mProcessShots;

            std::vector<float *> mStacks;

            operations::dataunits::GridBox *mpGridBox;

            bool mHasShotDynamic = false;

            /**
             * ==============================================
             * Flags used for the master slave communication.
             * ==============================================
             *
             * flag[0] -> Shot ID (from server to clients).
             *
             * flag[1] -> Shot flag (from server to clients):
             *     - 0 = There are shots available
             *     - 1 = All shots are migrated
             *
             * flag[2] -> Availability flag (from clients to server):
             *     - 0 = Still working
             *     - 1 = Available now
             *
             * flag[3] -> Rank flag (from clients to server)
             *
             * flag[4] -> Migration flag (from server to clients):
             *     - 0 = For a process if (process rank < # of shots) -> It will be working on a shot
             *     - 1 = If (process rank >= Number of shots) -> There is no enough shots for this process
             */
            int flag[5] = {0, 0, 0, 0, 0};
        };
    }//namespace agents
}//namespace stbx

#endif

#endif //PIPELINE_AGENTS_DYNAMIC_SERVERLESS_HPP
