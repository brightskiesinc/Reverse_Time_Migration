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

#include <operations/components/independents/concrete/migration-accommodators/CrossCorrelationKernel.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <operations/utils/sampling/Sampler.hpp>
#include <operations/configurations/MapKeys.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>

#define EPSILON 1e-20

using namespace std;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::utils::sampling;
using namespace bs::base::logger;


CrossCorrelationKernel::CrossCorrelationKernel(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mCompensationType = NO_COMPENSATION;
    this->mDipAngle = 90.0f;
}

CrossCorrelationKernel::~CrossCorrelationKernel() {
    delete this->mpShotCorrelation;
    delete this->mpTotalCorrelation;
    delete this->mpSourceIllumination;
    delete this->mpReceiverIllumination;
}

void CrossCorrelationKernel::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mDipAngle = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_DIP_ANGLE, this->mDipAngle);

    Logger->Info() << "\tusing stack dip angle = " << this->mDipAngle << '\n';
    this->mDipAngle = this->mDipAngle * M_PI / 180.0f;

    string compensation = "no";
    compensation = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_COMPENSATION, compensation);

    if (compensation.empty()) {
        Logger->Error() << "No entry for migration-accommodator.compensation key : supported values [ "
                           "no | source | receiver | combined ]" << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    } else if (compensation == OP_K_COMPENSATION_NONE) {
        this->SetCompensation(NO_COMPENSATION);
        Logger->Info() << "No illumination compensation is requested" << '\n';
    } else if (compensation == OP_K_COMPENSATION_COMBINED) {
        this->SetCompensation(COMBINED_COMPENSATION);
        Logger->Info() << "Applying combined illumination compensation" << '\n';
    } else {
        Logger->Info() << "Invalid value for migration-accommodator.compensation key : supported values [ "
                          OP_K_COMPENSATION_NONE " | "
                          OP_K_COMPENSATION_COMBINED " ]" << '\n';
        Logger->Info() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void CrossCorrelationKernel::Correlate(dataunits::DataUnit *apDataUnit) {

    auto grid_box = (GridBox *) apDataUnit;

    uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();

    if (ny == 1) {
        switch (this->mCompensationType) {
            case NO_COMPENSATION:
                Correlation<true, NO_COMPENSATION>(grid_box);
                break;
            case COMBINED_COMPENSATION:
                Correlation<true, COMBINED_COMPENSATION>(grid_box);
                break;
        }
    } else {
        switch (this->mCompensationType) {
            case NO_COMPENSATION:
                Correlation<false, NO_COMPENSATION>(grid_box);
                break;
            case COMBINED_COMPENSATION:
                Correlation<false, COMBINED_COMPENSATION>(grid_box);
                break;
        }
    }
}

void CrossCorrelationKernel::Stack() {
    if (this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() == 1) {
        switch (this->mCompensationType) {
            case NO_COMPENSATION:
                Stack < true, NO_COMPENSATION > ();
                break;
            case COMBINED_COMPENSATION:
                Stack < true, COMBINED_COMPENSATION > ();
                break;
        }
    } else {
        switch (this->mCompensationType) {
            case NO_COMPENSATION:
                Stack < false, NO_COMPENSATION > ();
                break;
            case COMBINED_COMPENSATION:
                Stack < false, COMBINED_COMPENSATION > ();
                break;
        }
    }
}

void CrossCorrelationKernel::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void CrossCorrelationKernel::SetCompensation(COMPENSATION_TYPE aCOMPENSATION_TYPE) {
    mCompensationType = aCOMPENSATION_TYPE;
}

void CrossCorrelationKernel::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    InitializeInternalElements();
}

void CrossCorrelationKernel::InitializeInternalElements() {
    // Grid.
    uint nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    uint nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    uint grid_size = nx * nz;
    uint grid_bytes = grid_size * sizeof(float);

    /* Window initialization. */

    uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint window_size = wnx * wnz;
    uint window_bytes = window_size * sizeof(float);


    mpShotCorrelation = new FrameBuffer<float>();
    mpShotCorrelation->Allocate(window_size, mpParameters->GetHalfLength(), "shot_correlation");
    Device::MemSet(mpShotCorrelation->GetNativePointer(), 0, window_bytes);

    mpSourceIllumination = new FrameBuffer<float>();
    mpSourceIllumination->Allocate(window_size, mpParameters->GetHalfLength(), "source_illumination");
    Device::MemSet(mpSourceIllumination->GetNativePointer(), 0, window_bytes);

    mpReceiverIllumination = new FrameBuffer<float>();
    mpReceiverIllumination->Allocate(window_size, mpParameters->GetHalfLength(), "receiver_illumination");
    Device::MemSet(mpReceiverIllumination->GetNativePointer(), 0, window_bytes);

    mpTotalCorrelation = new FrameBuffer<float>();
    mpTotalCorrelation->Allocate(grid_size, mpParameters->GetHalfLength(), "stacked_shot_correlation");
    Device::MemSet(mpTotalCorrelation->GetNativePointer(), 0, grid_bytes);
}

void CrossCorrelationKernel::ResetShotCorrelation() {
    uint window_bytes = sizeof(float) *
                        this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize() *
                        this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    Device::MemSet(this->mpShotCorrelation->GetNativePointer(), 0, window_bytes);
    Device::MemSet(this->mpSourceIllumination->GetNativePointer(), 0, window_bytes);
    Device::MemSet(this->mpReceiverIllumination->GetNativePointer(), 0, window_bytes);
}

FrameBuffer<float> *CrossCorrelationKernel::GetShotCorrelation() {
    return this->mpShotCorrelation;
}

FrameBuffer<float> *CrossCorrelationKernel::GetStackedShotCorrelation() {
    return this->mpTotalCorrelation;
}

MigrationData *CrossCorrelationKernel::GetMigrationData() {
    vector<Result *> results;

    uint nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    uint nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    auto *result = new float[nx * ny * nz];
    memcpy(result, this->mpTotalCorrelation->GetHostPointer(),
           nx * nz * ny * sizeof(float));
    results.push_back(new Result(result));

    return new MigrationData(nx,
                             ny,
                             nz,
                             this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension(),
                             this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension(),
                             this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension(),
                             this->mpGridBox->GetParameterGatherHeader(),
                             results);

}

void CrossCorrelationKernel::SetSourcePoint(Point3D *apSourcePoint) {
    this->mSourcePoint = *apSourcePoint;
}

uint
CrossCorrelationKernel::CalculateDipAngleDepth(double aDipAngle, int aCurrentPositionX, int aCurrentPositionY) const {
    /* Opposite = Adjacent * tan(theta) */
    float theta = (M_PI / 2) - aDipAngle;
    return ceil(tan(theta) * fabs(sqrt(
            pow(((int) this->mSourcePoint.x) - aCurrentPositionX, 2)
            + pow(((int) this->mSourcePoint.y) - aCurrentPositionY, 2))));
}
