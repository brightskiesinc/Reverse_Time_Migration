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

#if defined(USING_MPI)

#include <stbx/agents/concrete/StaticServerlessAgent.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <mpi.h>

using namespace std;
using namespace stbx::agents;
using namespace operations::dataunits;
using namespace bs::base::logger;

StaticServerlessAgent::StaticServerlessAgent() {
    this->mCount = 0;
}

StaticServerlessAgent::~StaticServerlessAgent() {
    for (auto it : this->mStacks) {
        delete[] it;
    }
}

GridBox *StaticServerlessAgent::Initialize() {
    LoggerSystem *Logger=LoggerSystem::GetInstance();
    this->mpGridBox = mpEngine->Initialize();

    int provided;
    MPI_Init_thread(&this->argc, &this->argv, MPI_THREAD_FUNNELED, &provided);

    if (provided != MPI_THREAD_FUNNELED) {
        Logger->Error() << "Warning MPI did not provide MPI_THREAD_FUNNELED..."<<'\n';
    }

    this->mCommunication = MPI_COMM_WORLD;
    MPI_Comm_rank(this->mCommunication, &this->self);
    MPI_Comm_size(this->mCommunication, &this->mProcessCount);

    return this->mpGridBox;
}

void StaticServerlessAgent::BeforeMigration() {
    this->mPossibleShots = mpEngine->GetValidShots();
}

void StaticServerlessAgent::AfterMigration() {}

void StaticServerlessAgent::BeforeFinalize() {}

MigrationData *StaticServerlessAgent::AfterFinalize(MigrationData *apMigrationData) {
    MigrationData *md = apMigrationData;

    uint size = md->GetGridSize(X_AXIS) *
                md->GetGridSize(Y_AXIS) *
                md->GetGridSize(Z_AXIS) *
                md->GetGatherDimension();

    for (int i = 0; i < md->GetResults().size(); ++i) {
        this->mStacks.push_back(new float[size]);
        MPI_Reduce(md->GetResultAt(i)->GetData(), this->mStacks[i],
                   size, MPI_FLOAT, MPI_SUM, 0, this->mCommunication);
    }

    if (this->self == 0) {
        for (int i = 0; i < md->GetResults().size(); ++i) {
            md->SetResults(i, new Result(this->mStacks[i]));
        }
    }

    MPI_Finalize();
    if (this->self != 0) {
        exit(0);
    }

    return md;
}

bool StaticServerlessAgent::HasNextShot() {
    this->mCount++;
    return this->mCount < 2;
}

vector<uint> StaticServerlessAgent::GetNextShot() {
    vector<uint> process_shots;
    uint size = this->mPossibleShots.size();
    for (int i = this->self; i < size; i = i + this->mProcessCount) {
        process_shots.push_back(this->mPossibleShots[i]);
    }
    return process_shots;
}

#endif
