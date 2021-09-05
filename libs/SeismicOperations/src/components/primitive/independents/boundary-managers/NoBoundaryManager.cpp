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

#include "operations/components/independents/concrete/boundary-managers/NoBoundaryManager.hpp"

#include "operations/components/independents/concrete/boundary-managers/extensions/ZeroExtension.hpp"
#include <bs/base/logger/concrete/LoggerSystem.hpp>

using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;

NoBoundaryManager::NoBoundaryManager(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

void NoBoundaryManager::AcquireConfiguration() {}

NoBoundaryManager::~NoBoundaryManager() {
    for (auto const &extension : this->mvExtensions) {
        delete extension;
    }
    this->mvExtensions.clear();
}

void NoBoundaryManager::ExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ExtendProperty();
    }
}

void NoBoundaryManager::ReExtendModel() {
    for (auto const &extension : this->mvExtensions) {
        extension->ReExtendProperty();
    }
}

void NoBoundaryManager::ApplyBoundary(uint kernel_id) {
    // Do nothing for perfect reflection.
}

void NoBoundaryManager::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

}

void NoBoundaryManager::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    InitializeExtensions();
}

void NoBoundaryManager::InitializeExtensions() {
    uint params_size = this->mpGridBox->GetParameters().size();
    for (int i = 0; i < params_size; ++i) {
        this->mvExtensions.push_back(new ZeroExtension());
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

void NoBoundaryManager::AdjustModelForBackward() {
    for (auto const &extension : this->mvExtensions) {
        extension->AdjustPropertyForBackward();
    }
}
