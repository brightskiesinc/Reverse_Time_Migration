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

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/forward-collectors/ReversePropagation.hpp>
#include <operations/configurations/MapKeys.h>
#include <operations/components/independents/concrete/forward-collectors/boundary-saver/BoundarySaver.h>

using namespace bs::base::logger;
using namespace operations::components;
using namespace operations::components::helpers;
using namespace operations::helpers;
using namespace operations::common;
using namespace operations::dataunits;


ReversePropagation::ReversePropagation(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mpInternalGridBox = new GridBox();
    this->mpComputationKernel = nullptr;
    this->mInjectionEnabled = false;
    this->mTimeStep = 0;
}

ReversePropagation::~ReversePropagation() {
    this->mpWaveFieldsMemoryHandler->FreeWaveFields(this->mpInternalGridBox);
    delete this->mpInternalGridBox;
    delete this->mpComputationKernel;
    for (auto boundary_saver : this->mBoundarySavers) {
        delete boundary_saver;
    }
}

void ReversePropagation::AcquireConfiguration() {
    this->mInjectionEnabled = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_BOUNDARY_SAVING,
                                                                 this->mInjectionEnabled);
    auto computation_kernel = (ComputationKernel *) (this->mpComponentsMap->Get(COMPUTATION_KERNEL));

    this->mpComputationKernel = (ComputationKernel *) computation_kernel->Clone();
    this->mpComputationKernel->SetDependentComponents(this->GetDependentComponentsMap());
    this->mpComputationKernel->SetMode(KERNEL_MODE::INVERSE);
    this->mpComputationKernel->SetComputationParameters(this->mpParameters);
    this->mpComputationKernel->SetGridBox(this->mpInternalGridBox);
    this->mpComputationKernel->SetDependentComponents(this->GetDependentComponentsMap());
}

void ReversePropagation::FetchForward() {
    this->mpComputationKernel->Step();

    if (this->mInjectionEnabled) {
        this->mTimeStep--;
        for (auto boundary_saver : this->mBoundarySavers) {
            boundary_saver->RestoreBoundaries(this->mTimeStep);
        }
    }
}

void ReversePropagation::ResetGrid(bool is_forward_run) {

    uint wnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;

    if (!is_forward_run) {

        this->mpMainGridBox->CloneMetaData(this->mpInternalGridBox);
        this->mpMainGridBox->CloneParameters(this->mpInternalGridBox);

        if (this->mpInternalGridBox->GetWaveFields().empty()) {
            this->mpWaveFieldsMemoryHandler->CloneWaveFields(this->mpMainGridBox,
                                                             this->mpInternalGridBox);
        } else {
            this->mpWaveFieldsMemoryHandler->CopyWaveFields(this->mpMainGridBox,
                                                            this->mpInternalGridBox);
        }

        /*
         * Swapping
         */

        if (this->mpParameters->GetApproximation() == ISOTROPIC) {
            if (this->mpParameters->GetEquationOrder() == FIRST) {
                // Swap next and current to reverse time.

                this->mpInternalGridBox->Swap(WAVE | GB_PRSS | NEXT | DIR_Z,
                                              WAVE | GB_PRSS | CURR | DIR_Z);
            } else if (this->mpParameters->GetEquationOrder() == SECOND) {

                // Swap previous and current to reverse time.
                this->mpInternalGridBox->Swap(WAVE | GB_PRSS | PREV | DIR_Z,
                                              WAVE | GB_PRSS | CURR | DIR_Z);
                // Only use two pointers, prev is same as next.

                this->mpInternalGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z,
                                             this->mpInternalGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z));
            }
        }

    } else {
        if (this->mInjectionEnabled) {
            this->Inject();
        }
    }

    for (auto const &wave_field : this->mpMainGridBox->GetWaveFields()) {
        Device::MemSet(wave_field.second->GetNativePointer(), 0.0f, window_size * sizeof(float));
    }
}

void ReversePropagation::SaveForward() {

    if (this->mInjectionEnabled) {
        for (auto boundary_saver : this->mBoundarySavers) {
            boundary_saver->SaveBoundaries(this->mTimeStep);
        }
        this->mTimeStep++;
    }
}

void ReversePropagation::Inject() {
    this->mTimeStep = 0;
    if (this->mBoundarySavers.empty()) {
        auto boundary_saver = new BoundarySaver();
        boundary_saver->Initialize(
                WAVE | GB_PRSS | CURR | DIR_Z,
                this->mpInternalGridBox,
                this->mpMainGridBox, this->mpParameters);
        this->mBoundarySavers.push_back(boundary_saver);
        if (this->mpParameters->GetApproximation() == ISOTROPIC) {
            if (this->mpParameters->GetEquationOrder() == FIRST) {
                auto boundary_saver_particle_x = new BoundarySaver();
                boundary_saver_particle_x->Initialize(
                        WAVE | GB_PRTC | CURR | DIR_X,
                        this->mpInternalGridBox,
                        this->mpMainGridBox, this->mpParameters);
                this->mBoundarySavers.push_back(boundary_saver_particle_x);
                auto boundary_saver_particle_z = new BoundarySaver();
                boundary_saver_particle_z->Initialize(
                        WAVE | GB_PRTC | CURR | DIR_Z,
                        this->mpInternalGridBox,
                        this->mpMainGridBox, this->mpParameters);
                this->mBoundarySavers.push_back(boundary_saver_particle_z);
                uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
                if (wny > 1) {
                    auto boundary_saver_particle_y = new BoundarySaver();
                    boundary_saver_particle_y->Initialize(
                            WAVE | GB_PRTC | CURR | DIR_Y,
                            this->mpInternalGridBox,
                            this->mpMainGridBox, this->mpParameters);
                    this->mBoundarySavers.push_back(boundary_saver_particle_y);
                }
            }
        }
    }
}

void ReversePropagation::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void ReversePropagation::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpMainGridBox = apGridBox;
    if (this->mpMainGridBox == nullptr) {
        Logger->Error() << "Not a compatible GridBox... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void ReversePropagation::SetDependentComponents(
        ComponentsMap<DependentComponent> *apDependentComponentsMap) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    HasDependents::SetDependentComponents(apDependentComponentsMap);

    this->mpWaveFieldsMemoryHandler =
            (WaveFieldsMemoryHandler *)
                    this->GetDependentComponentsMap()->Get(MEMORY_HANDLER);
    if (this->mpWaveFieldsMemoryHandler == nullptr) {
        Logger->Error() << "No Wave Fields Memory Handler provided... "
                        << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

GridBox *ReversePropagation::GetForwardGrid() {
    return this->mpInternalGridBox;
}
