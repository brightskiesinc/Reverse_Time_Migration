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

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>

#include <operations/configurations/MapKeys.h>
#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <algorithm>
#include <cmath>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

using namespace bs::base::configurations;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;


CPMLBoundaryManager::CPMLBoundaryManager(ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mReflectCoefficient = 0.1;
    this->mShiftRatio = 0.1;
    this->mRelaxCoefficient = 0.1;
    this->mUseTopLayer = true;
    this->mpExtension = nullptr;
    this->mpCoeffax = nullptr;
    this->mpCoeffbx = nullptr;
    this->mpCoeffaz = nullptr;
    this->mpCoeffbz = nullptr;
    this->mpCoeffay = nullptr;
    this->mpCoeffby = nullptr;
    this->mpAux1xup = nullptr;
    this->mpAux1xdown = nullptr;
    this->mpAux1zup = nullptr;
    this->mpAux1zdown = nullptr;
    this->mpAux1yup = nullptr;
    this->mpAux1ydown = nullptr;
    this->mpAux2xup = nullptr;
    this->mpAux2xdown = nullptr;
    this->mpAux2zup = nullptr;
    this->mpAux2zdown = nullptr;
    this->mpAux2yup = nullptr;
    this->mpAux2ydown = nullptr;
    this->mpFirstCoeffx = nullptr;
    this->mpFirstCoeffz = nullptr;
    this->mpFirstCoeffy = nullptr;
    this->mpSecondCoeffx = nullptr;
    this->mpSecondCoeffz = nullptr;
    this->mpSecondCoeffy = nullptr;
    this->mpDistanceDim1 = nullptr;
    this->mpDistanceDim2 = nullptr;
    this->mpDistanceDim3 = nullptr;
    this->mpParameters = nullptr;
    this->mpGridBox = nullptr;
    this->mMaxVel = 0;
}

void CPMLBoundaryManager::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mUseTopLayer = this->mpConfigurationMap->GetValue(
            OP_K_PROPRIETIES,
            OP_K_USE_TOP_LAYER, this->mUseTopLayer);
    if (this->mUseTopLayer) {
        Logger->Info() << "Using top boundary layer for forward modelling."
                          " To disable it set <boundary-manager.use-top-layer=false>" << '\n';
    } else {
        Logger->Info() << "Not using top boundary layer for forward modelling."
                          " To enable it set <boundary-manager.use-top-layer=true>" << '\n';
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES,
                                           OP_K_REFLECT_COEFFICIENT)) {
        Logger->Info() << "Parsing user defined reflect coefficient" << '\n';
        this->mReflectCoefficient = this->mpConfigurationMap->GetValue(
                OP_K_PROPRIETIES,
                OP_K_REFLECT_COEFFICIENT,
                this->mReflectCoefficient);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_SHIFT_RATIO)) {
        Logger->Info() << "Parsing user defined shift ratio" << '\n';
        this->mShiftRatio = this->mpConfigurationMap->GetValue(
                OP_K_PROPRIETIES,
                OP_K_SHIFT_RATIO, this->mShiftRatio);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_RELAX_COEFFICIENT)) {
        Logger->Info() << "Parsing user defined relax coefficient" << '\n';
        this->mRelaxCoefficient = this->mpConfigurationMap->GetValue(
                OP_K_PROPRIETIES,
                OP_K_RELAX_COEFFICIENT,
                this->mRelaxCoefficient);
    }

    this->mpExtension = new HomogenousExtension(this->mUseTopLayer);
    this->mpExtension->SetHalfLength(
            this->mpParameters->GetHalfLength());
    this->mpExtension->SetBoundaryLength(
            this->mpParameters->GetBoundaryLength());

}

template<int DIRECTION_>
void CPMLBoundaryManager::FillCPMLCoefficients() {
    float pml_reg_len = this->mpParameters->GetBoundaryLength();
    float dh = 0.0;
    float *coeff_a;
    float *coeff_b;
    if (pml_reg_len == 0) {
        pml_reg_len = 1;
    }
    float d0 = 0;
    // Case x :
    if (DIRECTION_ == X_AXIS) {
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        coeff_a = this->mpCoeffax->GetNativePointer();
        coeff_b = this->mpCoeffbx->GetNativePointer();
        // Case z :
    } else if (DIRECTION_ == Z_AXIS) {
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
        coeff_a = this->mpCoeffaz->GetNativePointer();
        coeff_b = this->mpCoeffbz->GetNativePointer();
        // Case y :
    } else {
        dh = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        coeff_a = this->mpCoeffay->GetNativePointer();
        coeff_b = this->mpCoeffby->GetNativePointer();
    }
    float dt = this->mpGridBox->GetDT();
    // Compute damping vector
    d0 = (-logf(this->mReflectCoefficient) * ((3 * mMaxVel) / (pml_reg_len * dh)) *
          this->mRelaxCoefficient) /
         pml_reg_len;
    uint bound_length = this->mpParameters->GetBoundaryLength();
    float coeff_a_temp[bound_length];
    float coeff_b_temp[bound_length];
    for (int i = this->mpParameters->GetBoundaryLength(); i > 0; i--) {
        float damping_ratio = PWR2(i) * d0;
        coeff_a_temp[i - 1] = expf(-dt * (damping_ratio + this->mShiftRatio));
        coeff_b_temp[i - 1] = (damping_ratio / (damping_ratio + this->mShiftRatio)) *
                              (coeff_a_temp[i - 1] - 1);
    }
    Device::MemCpy(coeff_a, coeff_a_temp, bound_length * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(coeff_b, coeff_b_temp, bound_length * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
}

template<int HALF_LENGTH_>
void CPMLBoundaryManager::ApplyAllCPML() {
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
    CalculateFirstAuxiliary<X_AXIS, true, HALF_LENGTH_>();
    CalculateFirstAuxiliary<Z_AXIS, true, HALF_LENGTH_>();
    CalculateFirstAuxiliary<X_AXIS, false, HALF_LENGTH_>();
    CalculateFirstAuxiliary<Z_AXIS, false, HALF_LENGTH_>();
    if (ny > 1) {
        CalculateFirstAuxiliary<Y_AXIS, true, HALF_LENGTH_>();
        CalculateFirstAuxiliary<Y_AXIS, false, HALF_LENGTH_>();
    }

    CalculateCPMLValue<X_AXIS, true, HALF_LENGTH_>();
    CalculateCPMLValue<Z_AXIS, true, HALF_LENGTH_>();
    CalculateCPMLValue<X_AXIS, false, HALF_LENGTH_>();
    CalculateCPMLValue<Z_AXIS, false, HALF_LENGTH_>();
    // 3D --> Add CPML for y-direction.
    if (ny > 1) {
        CalculateCPMLValue<Y_AXIS, true, HALF_LENGTH_>();
        CalculateCPMLValue<Y_AXIS, false, HALF_LENGTH_>();
    }
}

void CPMLBoundaryManager::InitializeVariables() {


    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

    int actual_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int actual_nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dy = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


    float dt = this->mpGridBox->GetDT();
    int bound_length = this->mpParameters->GetBoundaryLength();
    int half_length = this->mpParameters->GetHalfLength();
    float *host_velocity_base = this->mpGridBox->Get(PARM | GB_VEL)->GetHostPointer();
    float max_velocity = 0;

    for (int k = 0; k < ny; ++k) {
        for (int j = 0; j < nz; ++j) {
            for (int i = 0; i < nx; ++i) {
                int offset = i + actual_nx * j + k * actual_nx * actual_nz;
                float velocity_real = host_velocity_base[offset];
                if (velocity_real > max_velocity) {
                    max_velocity = velocity_real;
                }
            }
        }
    }
    max_velocity = max_velocity / (dt * dt);
    this->mMaxVel = sqrtf(max_velocity);

    this->mpCoeffax = new FrameBuffer<float>(bound_length);
    this->mpCoeffbx = new FrameBuffer<float>(bound_length);
    this->mpCoeffaz = new FrameBuffer<float>(bound_length);
    this->mpCoeffbz = new FrameBuffer<float>(bound_length);

    int width = bound_length + (2 * this->mpParameters->GetHalfLength());
    int y_size = width * wnx * wnz;
    int x_size = width * wny * wnz;
    int z_size = width * wnx * wny;

    this->mpAux1xup = new FrameBuffer<float>(x_size);
    this->mpAux1xdown = new FrameBuffer<float>(x_size);
    this->mpAux2xup = new FrameBuffer<float>(x_size);
    this->mpAux2xdown = new FrameBuffer<float>(x_size);

    Device::MemSet(mpAux1xup->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux1xdown->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux2xup->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux2xdown->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);

    this->mpAux1zup = new FrameBuffer<float>(z_size);
    this->mpAux1zdown = new FrameBuffer<float>(z_size);
    this->mpAux2zup = new FrameBuffer<float>(z_size);
    this->mpAux2zdown = new FrameBuffer<float>(z_size);

    Device::MemSet(mpAux1zup->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux1zdown->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux2zup->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux2zdown->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);

    FillCPMLCoefficients<X_AXIS>();
    FillCPMLCoefficients<Z_AXIS>();
    if (ny > 1) {
        this->mpCoeffay = new FrameBuffer<float>(bound_length);
        this->mpCoeffby = new FrameBuffer<float>(bound_length);

        this->mpAux1yup = new FrameBuffer<float>(y_size);
        this->mpAux1ydown = new FrameBuffer<float>(y_size);
        this->mpAux2yup = new FrameBuffer<float>(y_size);
        this->mpAux2ydown = new FrameBuffer<float>(y_size);

        Device::MemSet(mpAux1yup->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux1ydown->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux2yup->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux2ydown->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);

        FillCPMLCoefficients<Y_AXIS>();
    }
    int helper_array_len = half_length + 1;
    int distance_1[helper_array_len];
    int distance_2[helper_array_len];
    int distance_3[helper_array_len];
    float coeff_first_x[helper_array_len];
    float coeff_first_y[helper_array_len];
    float coeff_first_z[helper_array_len];
    float coeff_second_x[helper_array_len];
    float coeff_second_y[helper_array_len];
    float coeff_second_z[helper_array_len];
    if (dy == 0) {
        dy = 1;
    }
    float dx2 = dx * dx;
    float dz2 = dz * dz;
    float dy2 = dy * dy;
    for (int i = 0; i < helper_array_len; i++) {
        distance_1[i] = i;
        distance_2[i] = i * wnx;
        distance_3[i] = i * wnz * wnx;
        coeff_first_x[i] = this->mpParameters->GetFirstDerivativeFDCoefficient()[i] / dx;
        coeff_first_y[i] = this->mpParameters->GetFirstDerivativeFDCoefficient()[i] / dy;
        coeff_first_z[i] = this->mpParameters->GetFirstDerivativeFDCoefficient()[i] / dz;
        coeff_second_x[i] = this->mpParameters->GetSecondDerivativeFDCoefficient()[i] / dx2;
        coeff_second_y[i] = this->mpParameters->GetSecondDerivativeFDCoefficient()[i] / dy2;
        coeff_second_z[i] = this->mpParameters->GetSecondDerivativeFDCoefficient()[i] / dz2;
    }
    this->mpFirstCoeffx = new FrameBuffer<float>(helper_array_len);
    this->mpFirstCoeffy = new FrameBuffer<float>(helper_array_len);
    this->mpFirstCoeffz = new FrameBuffer<float>(helper_array_len);
    this->mpSecondCoeffx = new FrameBuffer<float>(helper_array_len);
    this->mpSecondCoeffy = new FrameBuffer<float>(helper_array_len);
    this->mpSecondCoeffz = new FrameBuffer<float>(helper_array_len);
    this->mpDistanceDim1 = new FrameBuffer<int>(helper_array_len);
    this->mpDistanceDim2 = new FrameBuffer<int>(helper_array_len);
    this->mpDistanceDim3 = new FrameBuffer<int>(helper_array_len);
    Device::MemCpy(this->mpDistanceDim1->GetNativePointer(),
                   distance_1, helper_array_len * sizeof(int),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpDistanceDim2->GetNativePointer(),
                   distance_2, helper_array_len * sizeof(int),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpDistanceDim3->GetNativePointer(),
                   distance_3, helper_array_len * sizeof(int),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpFirstCoeffx->GetNativePointer(),
                   coeff_first_x, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpFirstCoeffy->GetNativePointer(),
                   coeff_first_y, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpFirstCoeffz->GetNativePointer(),
                   coeff_first_z, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpSecondCoeffx->GetNativePointer(),
                   coeff_second_x, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpSecondCoeffy->GetNativePointer(),
                   coeff_second_y, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(this->mpSecondCoeffz->GetNativePointer(),
                   coeff_second_z, helper_array_len * sizeof(float),
                   Device::COPY_HOST_TO_DEVICE);
}

void CPMLBoundaryManager::ResetVariables() {

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    int width =
            this->mpParameters->GetBoundaryLength()
            + (2 * this->mpParameters->GetHalfLength());

    int y_size = width * wnx * wnz;
    int x_size = width * wny * wnz;
    int z_size = width * wnx * wny;

    Device::MemSet(mpAux1xup->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux1xdown->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux2xup->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);
    Device::MemSet(mpAux2xdown->GetNativePointer(), 0.0,
                   sizeof(float) * x_size);

    Device::MemSet(mpAux1zup->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux1zdown->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux2zup->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);
    Device::MemSet(mpAux2zdown->GetNativePointer(), 0.0,
                   sizeof(float) * z_size);

    if (this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() > 1) {
        Device::MemSet(mpAux1yup->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux1ydown->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux2yup->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
        Device::MemSet(mpAux2ydown->GetNativePointer(), 0.0,
                       sizeof(float) * y_size);
    }
}

void CPMLBoundaryManager::ExtendModel() {
    this->mpExtension->ExtendProperty();
    this->InitializeVariables();
}

void CPMLBoundaryManager::ReExtendModel() {
    this->mpExtension->ReExtendProperty();
    this->ResetVariables();
}

CPMLBoundaryManager::~CPMLBoundaryManager() {
    delete this->mpExtension;

    delete (mpCoeffax);
    delete (mpCoeffbx);

    delete (mpCoeffaz);
    delete (mpCoeffbz);

    delete (mpAux1xup);
    delete (mpAux1xdown);

    delete (mpAux1zup);
    delete (mpAux1zdown);

    delete (mpAux2xup);
    delete (mpAux2xdown);

    delete (mpAux2zup);
    delete (mpAux2zdown);

    if (mpAux1yup != nullptr) {
        delete (mpCoeffay);
        delete (mpCoeffby);

        delete (mpAux1yup);
        delete (mpAux1ydown);

        delete (mpAux2yup);
        delete (mpAux2ydown);
    }
    delete this->mpFirstCoeffx;
    delete this->mpFirstCoeffz;
    delete this->mpFirstCoeffy;
    delete this->mpSecondCoeffx;
    delete this->mpSecondCoeffz;
    delete this->mpSecondCoeffy;
    delete this->mpDistanceDim1;
    delete this->mpDistanceDim2;
    delete this->mpDistanceDim3;
}

void CPMLBoundaryManager::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void CPMLBoundaryManager::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    this->mpExtension->SetGridBox(this->mpGridBox);
    this->mpExtension->SetProperty(
            this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer(),
            this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer());
}

void CPMLBoundaryManager::ApplyBoundary(uint kernel_id) {
    if (kernel_id == 0) {
        switch (this->mpParameters->GetHalfLength()) {
            case O_2:
                ApplyAllCPML<O_2>();
                break;
            case O_4:
                ApplyAllCPML<O_4>();
                break;
            case O_8:
                ApplyAllCPML<O_8>();
                break;
            case O_12:
                ApplyAllCPML<O_12>();
                break;
            case O_16:
                ApplyAllCPML<O_16>();
                break;
        }
    }
}

void CPMLBoundaryManager::AdjustModelForBackward() {
    this->mpExtension->AdjustPropertyForBackward();
    this->ResetVariables();
}
