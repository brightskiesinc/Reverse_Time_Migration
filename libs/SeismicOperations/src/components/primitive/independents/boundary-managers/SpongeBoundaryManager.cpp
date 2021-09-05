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

#include <operations/components/independents/concrete/boundary-managers/SpongeBoundaryManager.hpp>

#include <operations/configurations/MapKeys.h>
#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <iostream>
#include <cmath>

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;


/// Based on
/// https://pubs.geoscienceworld.org/geophysics/article-abstract/50/4/705/71992/A-nonreflecting-boundary-condition-for-discrete?redirectedFrom=fulltext

SpongeBoundaryManager::SpongeBoundaryManager(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mUseTopLayer = true;
}

void SpongeBoundaryManager::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mUseTopLayer = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_USE_TOP_LAYER, this->mUseTopLayer);
    if (this->mUseTopLayer) {
        Logger->Info()
                << "Using top boundary layer for forward modelling. To disable it set <boundary-manager.use-top-layer=false>"
                << '\n';
    } else {
        Logger->Info()
                << "Not using top boundary layer for forward modelling. To enable it set <boundary-manager.use-top-layer=true>"
                << '\n';
    }
}

float SpongeBoundaryManager::Calculation(int index) {
    float value;
    uint bound_length = mpParameters->GetBoundaryLength();
    value = expf(-powf((0.1f / bound_length) * (bound_length - index), 2));
    return value;
}

void SpongeBoundaryManager::ApplyBoundary(uint kernel_id) {
    if (kernel_id == 0) {
        ApplyBoundaryOnField(this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer());
    }
}

void SpongeBoundaryManager::ExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ExtendProperty();
    }
}

void SpongeBoundaryManager::ReExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ReExtendProperty();
    }
}

SpongeBoundaryManager::~SpongeBoundaryManager() {
    for (auto const &extension : this->mvExtensions) {
        delete extension;
    }
    this->mvExtensions.clear();
    delete this->mpSpongeCoefficients;
}

void SpongeBoundaryManager::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SpongeBoundaryManager::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    InitializeExtensions();

    uint bound_length = this->mpParameters->GetBoundaryLength();

    auto temp_arr = new float[bound_length];

    for (int i = 0; i < bound_length; i++) {
        temp_arr[i] = Calculation(i);
    }

    mpSpongeCoefficients = new FrameBuffer<float>(bound_length);
    Device::MemCpy(mpSpongeCoefficients->GetNativePointer(), temp_arr, bound_length * sizeof(float));

    delete[] temp_arr;
}

void SpongeBoundaryManager::InitializeExtensions() {
    uint params_size = this->mpGridBox->GetParameters().size();
    for (int i = 0; i < params_size; ++i) {
        this->mvExtensions.push_back(new HomogenousExtension(mUseTopLayer));
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

void SpongeBoundaryManager::AdjustModelForBackward() {
    for (auto const &extension : this->mvExtensions) {
        extension->AdjustPropertyForBackward();
    }
}
