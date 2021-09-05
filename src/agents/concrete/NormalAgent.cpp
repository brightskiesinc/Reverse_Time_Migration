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
