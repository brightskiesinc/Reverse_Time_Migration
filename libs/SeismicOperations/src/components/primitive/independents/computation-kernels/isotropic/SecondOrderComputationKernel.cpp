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

#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

#include <bs/base/exceptions/concrete/IllogicalException.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <iostream>
#include <cmath>
#include <cstring>

#define fma(a, b, c) (a) * (b) + (c)

using namespace std;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;

SecondOrderComputationKernel::SecondOrderComputationKernel(
        bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mpMemoryHandler = new WaveFieldsMemoryHandler(apConfigurationMap);
    this->mpBoundaryManager = nullptr;
}

SecondOrderComputationKernel::SecondOrderComputationKernel(
        const SecondOrderComputationKernel &aSecondOrderComputationKernel) {
    this->mpConfigurationMap = aSecondOrderComputationKernel.mpConfigurationMap;
    this->mpMemoryHandler = new WaveFieldsMemoryHandler(this->mpConfigurationMap);
    this->mpBoundaryManager = nullptr;
}

SecondOrderComputationKernel::~SecondOrderComputationKernel() = default;

void SecondOrderComputationKernel::AcquireConfiguration() {}

ComputationKernel *SecondOrderComputationKernel::Clone() {
    return new SecondOrderComputationKernel(*this);
}

template<KERNEL_MODE KERNEL_MODE_>
void SecondOrderComputationKernel::Compute() {

    int logical_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();

    if (logical_ny == 1) {
        this->Compute<KERNEL_MODE_, true>();
    } else {
        this->Compute<KERNEL_MODE_, false>();
    }
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_>
void SecondOrderComputationKernel::Compute() {
    switch (mpParameters->GetHalfLength()) {
        case O_2:
            Compute < KERNEL_MODE_, IS_2D_, O_2 > ();
            break;
        case O_4:
            Compute < KERNEL_MODE_, IS_2D_, O_4 > ();
            break;
        case O_8:
            Compute < KERNEL_MODE_, IS_2D_, O_8 > ();
            break;
        case O_12:
            Compute < KERNEL_MODE_, IS_2D_, O_12 > ();
            break;
        case O_16:
            Compute < KERNEL_MODE_, IS_2D_, O_16 > ();
            break;
    }
}

void SecondOrderComputationKernel::Step() {
    // Take a step in time.
    if (mpCoeffX == nullptr) {
        InitializeVariables();
    }
    if (this->mMode == KERNEL_MODE::FORWARD) {
        this->Compute<KERNEL_MODE::FORWARD>();
    } else if (this->mMode == KERNEL_MODE::INVERSE) {
        this->Compute<KERNEL_MODE::INVERSE>();
    } else if (this->mMode == KERNEL_MODE::ADJOINT) {
        this->Compute<KERNEL_MODE::ADJOINT>();
    } else {
        throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
    }
    // Swap pointers : Next to current, current to prev and unwanted prev to next
    // to be overwritten.
    if (this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z) ==
        this->mpGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)) {
        // two pointers case : curr becomes both next and prev, while next becomes
        // current.
        this->mpGridBox->Swap(WAVE | GB_PRSS | PREV | DIR_Z, WAVE | GB_PRSS | CURR | DIR_Z);
        this->mpGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z,
                             this->mpGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z));
    } else {
        // three pointers : normal swapping between the three pointers.
        this->mpGridBox->Swap(WAVE | GB_PRSS | PREV | DIR_Z, WAVE | GB_PRSS | CURR | DIR_Z);
        this->mpGridBox->Swap(WAVE | GB_PRSS | CURR | DIR_Z, WAVE | GB_PRSS | NEXT | DIR_Z);
    }

    {
        ScopeTimer t("BoundaryManager::ApplyBoundary");
        if (this->mpBoundaryManager != nullptr) {
            this->mpBoundaryManager->ApplyBoundary();
        }
    }
}

void SecondOrderComputationKernel::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SecondOrderComputationKernel::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

MemoryHandler *SecondOrderComputationKernel::GetMemoryHandler() {
    return this->mpMemoryHandler;
}

void SecondOrderComputationKernel::InitializeVariables() {
    int wnx = mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wnz = mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx2 = 1 / (mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension() *
                     mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension());
    float dz2 = 1 / (mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension() *
                     mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension());
    float dy2 = 1;
    float *coeff = mpParameters->GetSecondDerivativeFDCoefficient();

    int hl = mpParameters->GetHalfLength();
    int array_length = hl;
    float coeff_x[hl];
    float coeff_y[hl];
    float coeff_z[hl];
    int vertical[hl];
    int front[hl];

    for (int i = 0; i < hl; i++) {
        coeff_x[i] = coeff[i + 1] * dx2;
        coeff_z[i] = coeff[i + 1] * dz2;

        vertical[i] = (i + 1) * (wnx);
    }

    mpCoeffX = new FrameBuffer<float>(array_length);
    mpCoeffZ = new FrameBuffer<float>(array_length);
    mpFrontalIdx = new FrameBuffer<int>(array_length);
    mpVerticalIdx = new FrameBuffer<int>(array_length);

    mCoeffXYZ = coeff[0] * (dx2 + dz2);

    Device::MemCpy(mpCoeffX->GetNativePointer(), coeff_x, array_length * sizeof(float));
    Device::MemCpy(mpCoeffZ->GetNativePointer(), coeff_z, array_length * sizeof(float));
    Device::MemCpy(mpVerticalIdx->GetNativePointer(), vertical, array_length * sizeof(int));
}
