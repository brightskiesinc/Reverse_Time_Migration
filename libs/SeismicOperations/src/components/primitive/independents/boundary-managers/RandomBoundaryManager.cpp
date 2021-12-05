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

#include <cstdlib>
#include <ctime>

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/boundary-managers/RandomBoundaryManager.hpp>
#include <operations/components/independents/concrete/boundary-managers/extensions/MinExtension.hpp>
#include <operations/components/independents/concrete/boundary-managers/extensions/RandomExtension.hpp>
#include <operations/configurations/MapKeys.h>

using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;


RandomBoundaryManager::RandomBoundaryManager(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    srand(time(NULL));
    this->mpConfigurationMap = apConfigurationMap;
    this->mGrainSideLength = 0;
}

void RandomBoundaryManager::AcquireConfiguration() {
    this->mGrainSideLength = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_GRAIN_SIDE_LENGTH,
                                                                this->mGrainSideLength);

    LoggerSystem *Logger = LoggerSystem::GetInstance();
    if (this->mGrainSideLength <= 0) {
        Logger->Error() << "No valid value provided for key 'grain_side_length'..." << '\n';
        Logger->Info() << "Using default value for grain_side_length 0f 200" << '\n';
        this->mGrainSideLength = 200;
    } else {
        Logger->Info() << "Random Boundary manager will use grains of side length = " << this->mGrainSideLength
                       << " meters." << '\n';
    }


}

RandomBoundaryManager::~RandomBoundaryManager() {
    for (auto const &extension : this->mvExtensions) {
        delete extension;
    }
    this->mvExtensions.clear();
}

void
RandomBoundaryManager::ExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ExtendProperty();
    }
}

void
RandomBoundaryManager::ReExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ExtendProperty();
        extension->ReExtendProperty();
    }
}

void
RandomBoundaryManager::ApplyBoundary(uint kernel_id) {
    // Do nothing for random boundaries.
}

void
RandomBoundaryManager::SetComputationParameters(ComputationParameters *apParameters) {
    auto logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void
RandomBoundaryManager::SetGridBox(GridBox *apGridBox) {
    auto logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    InitializeExtensions();
}

void
RandomBoundaryManager::InitializeExtensions() {
    this->mvExtensions.push_back(new RandomExtension(this->mGrainSideLength));

    uint params_size = this->mpGridBox->GetParameters().size();
    for (int i = 0; i < params_size - 1; ++i) {
        this->mvExtensions.push_back(new MinExtension());
    }
    for (auto const &extension : this->mvExtensions) {
        extension->SetHalfLength(this->mpParameters->GetHalfLength());
        extension->SetBoundaryLength(this->mpParameters->GetBoundaryLength());
    }

    uint index = 0;
    for (auto const &parameter :  this->mpGridBox->GetParameters()) {
        this->mvExtensions[index]->SetGridBox(this->mpGridBox);
        this->mvExtensions[index]->SetProperty(parameter.second->GetNativePointer(),
                                               this->mpGridBox->Get(WIND | parameter.first)->GetNativePointer());
        index++;
    }
}

void
RandomBoundaryManager::AdjustModelForBackward() {
    for (auto const &extension : this->mvExtensions) {
        extension->AdjustPropertyForBackward();
    }
}
