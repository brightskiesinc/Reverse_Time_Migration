/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms            out<<x_end - x_start<<" " <<y_end - y_start <<" "<< z_end - z_start<<sycl::endl;
 of the GNU Lesser General Public License as published
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

#include <iostream>
#include <cmath>

#include <bs/base/api/cpp/BSBase.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

using namespace std;
using namespace bs::base::logger;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;

StaggeredComputationKernel::StaggeredComputationKernel(
        bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mpMemoryHandler = new WaveFieldsMemoryHandler(this->mpConfigurationMap);
    this->mpBoundaryManager = nullptr;
    this->mpCoeff = nullptr;
}

StaggeredComputationKernel::StaggeredComputationKernel(const StaggeredComputationKernel &aStaggeredComputationKernel) {
    this->mpConfigurationMap = aStaggeredComputationKernel.mpConfigurationMap;
    this->mpMemoryHandler = new WaveFieldsMemoryHandler(this->mpConfigurationMap);
    this->mpBoundaryManager = nullptr;
    this->mpCoeff = nullptr;
}

StaggeredComputationKernel::~StaggeredComputationKernel() {
    delete this->mpCoeff;
}

void StaggeredComputationKernel::AcquireConfiguration() {}

ComputationKernel *StaggeredComputationKernel::Clone() {
    return new StaggeredComputationKernel(*this);
}

template<KERNEL_MODE KERNEL_MODE_>
void StaggeredComputationKernel::ComputeAll() {
    int logical_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();

    if (logical_ny == 1) {
        this->ComputeAll<KERNEL_MODE_, true>();
    } else {
        this->ComputeAll<KERNEL_MODE_, false>();
    }
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_>
void StaggeredComputationKernel::ComputeAll() {
    switch (mpParameters->GetHalfLength()) {
        case O_2:
            ComputeAll < KERNEL_MODE_, IS_2D_, O_2 > ();
            break;
        case O_4:
            ComputeAll < KERNEL_MODE_, IS_2D_, O_4 > ();
            break;
        case O_8:
            ComputeAll < KERNEL_MODE_, IS_2D_, O_8 > ();
            break;
        case O_12:
            ComputeAll < KERNEL_MODE_, IS_2D_, O_12 > ();
            break;
        case O_16:
            ComputeAll < KERNEL_MODE_, IS_2D_, O_16 > ();
            break;
    }
}

template<KERNEL_MODE KERNEL_MODE_, bool IS_2D_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::ComputeAll() {
    this->ComputeVelocity<KERNEL_MODE_, IS_2D_, HALF_LENGTH_>();

    ElasticTimer timer("BoundaryManager::ApplyBoundary(Velocity)");
    timer.Start();
    if (this->mpBoundaryManager != nullptr) {
        this->mpBoundaryManager->ApplyBoundary(1);
    }
    timer.Stop();
    this->ComputePressure<KERNEL_MODE_, IS_2D_, HALF_LENGTH_>();

}

void StaggeredComputationKernel::Step() {
    if (this->mpCoeff == nullptr) {
        InitializeVariables();
    }
    // Take a step in time.
    if (this->mMode == KERNEL_MODE::FORWARD) {
        this->ComputeAll<KERNEL_MODE::FORWARD>();
    } else if (this->mMode == KERNEL_MODE::INVERSE) {
        this->ComputeAll<KERNEL_MODE::INVERSE>();
    } else if (this->mMode == KERNEL_MODE::ADJOINT) {
        this->ComputeAll<KERNEL_MODE::ADJOINT>();
    } else {
        throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
    }
    this->mpGridBox->Swap(WAVE | GB_PRSS | NEXT | DIR_Z, WAVE | GB_PRSS | CURR | DIR_Z);

    {
        ScopeTimer t("BoundaryManager::ApplyBoundary(Pressure)");
        if (this->mpBoundaryManager != nullptr) {
            this->mpBoundaryManager->ApplyBoundary(0);
        }
    }
}

void StaggeredComputationKernel::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void StaggeredComputationKernel::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

MemoryHandler *StaggeredComputationKernel::GetMemoryHandler() {
    return this->mpMemoryHandler;
}

void StaggeredComputationKernel::InitializeVariables() {

    int wnx = mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    float *coeff = mpParameters->GetFirstDerivativeStaggeredFDCoefficient();

    int hl = mpParameters->GetHalfLength();
    int array_length = hl;
    float coeff_local[hl];
    int vertical[hl];

    for (int i = 0; i < hl; i++) {
        coeff_local[i] = coeff[i + 1];
        vertical[i] = (i + 1) * (wnx);

    }
    mpCoeff = new FrameBuffer<float>(array_length);
    mpVerticalIdx = new FrameBuffer<int>(array_length);

    Device::MemCpy(mpCoeff->GetNativePointer(), coeff_local,
                   array_length * sizeof(float));
    Device::MemCpy(mpVerticalIdx->GetNativePointer(), vertical, array_length * sizeof(int));

}
