//
// Created by mennatallah on 6/9/20.
//

#ifndef PIPELINE_AGENTS_STATIC_SERVER_HPP
#define PIPELINE_AGENTS_STATIC_SERVER_HPP

#if defined(USING_MPI)

#include <stbx/agents/interface/Agent.hpp>

#include <mpi.h>

namespace stbx {
    namespace agents {

        class StaticServerAgent : public Agent {
        public:
            StaticServerAgent();

            ~StaticServerAgent() override;

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

#endif //PIPELINE_AGENTS_STATIC_SERVER_HPP
