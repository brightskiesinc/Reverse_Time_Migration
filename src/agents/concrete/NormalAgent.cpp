//
// Created by zeyad-osama on 01/09/2020.
//

#include <stbx/agents/concrete/NormalAgent.hpp>

using namespace std;
using namespace stbx::agents;
using namespace operations::dataunits;

NormalAgent::~NormalAgent() = default;

GridBox *NormalAgent::Initialize() {
    return Agent::Initialize();
}

void NormalAgent::BeforeMigration() {}

void NormalAgent::AfterMigration() {}

void NormalAgent::BeforeFinalize() {}

MigrationData *NormalAgent::AfterFinalize(MigrationData *aMigrationData) {
    return aMigrationData;
}

bool NormalAgent::HasNextShot() {
    this->mCount++;
    return this->mCount < 2;
}

vector<uint> NormalAgent::GetNextShot() {
    return this->mpEngine->GetValidShots();
}
