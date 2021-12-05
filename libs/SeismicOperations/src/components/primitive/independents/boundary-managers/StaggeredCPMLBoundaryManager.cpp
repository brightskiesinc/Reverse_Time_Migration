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

#include <algorithm>
#include <cmath>

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp>
#include <operations/configurations/MapKeys.h>
#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

using namespace bs::base::logger;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::common;
using namespace operations::dataunits;


StaggeredCPMLBoundaryManager::StaggeredCPMLBoundaryManager(
        bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mReflectCoefficient = 0.1;
    this->mShiftRatio = 0.1;
    this->mRelaxCoefficient = 1;
    this->mUseTopLayer = true;
    this->mMaxVelocity = 0;
    this->mpCoeff = 0;
}

void
StaggeredCPMLBoundaryManager::AcquireConfiguration() {
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
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_REFLECT_COEFFICIENT)) {
        Logger->Info() << "Parsing user defined reflect coefficient" << '\n';
        this->mReflectCoefficient = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_REFLECT_COEFFICIENT,
                                                                       this->mReflectCoefficient);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_SHIFT_RATIO)) {
        Logger->Info() << "Parsing user defined shift ratio" << '\n';
        this->mShiftRatio = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_SHIFT_RATIO, this->mShiftRatio);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_RELAX_COEFFICIENT)) {
        Logger->Info() << "Parsing user defined relax coefficient" << '\n';
        this->mRelaxCoefficient = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_RELAX_COEFFICIENT,
                                                                     this->mRelaxCoefficient);
    }
}

void
StaggeredCPMLBoundaryManager::ExtendModel() {
    for (auto const &extension: this->mvExtensions) {
        extension->ExtendProperty();
    }
}

void
StaggeredCPMLBoundaryManager::ReExtendModel() {
    for (auto const &extension: this->mvExtensions) {
        extension->ReExtendProperty();
    }
    ZeroAuxiliaryVariables();
}

StaggeredCPMLBoundaryManager::~StaggeredCPMLBoundaryManager() {
    for (auto const &extension: this->mvExtensions) {
        delete extension;
    }
    this->mvExtensions.clear();
    delete mpSmall_a_x;
    delete mpSmall_a_y;
    delete mpSmall_a_z;
    delete mpSmall_b_x;
    delete mpSmall_b_y;
    delete mpSmall_b_z;
    delete mpAuxiliary_vel_x_left;
    delete mpAuxiliary_vel_x_right;
    delete mpAuxiliary_ptr_x_left;
    delete mpAuxiliary_ptr_x_right;
    delete mpAuxiliary_vel_z_up;
    delete mpAuxiliary_vel_z_down;
    delete mpAuxiliary_ptr_z_up;
    delete mpAuxiliary_ptr_z_down;
    delete mpAuxiliary_vel_y_up;
    delete mpAuxiliary_vel_y_down;
    delete mpAuxiliary_ptr_y_up;
    delete mpAuxiliary_ptr_y_down;
    delete mpCoeff;
}

void
StaggeredCPMLBoundaryManager::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void
StaggeredCPMLBoundaryManager::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    InitializeExtensions();

    int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
    int ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
    int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

    int actual_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int actual_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int actual_nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    int b_l = mpParameters->GetBoundaryLength();
    HALF_LENGTH h_l = mpParameters->GetHalfLength();

    // get the size of grid
    int grid_total_size = wnx * wnz * wny;
    // the size of the boundary in x without half_length is x=b_l and z=nz-2*h_l
    int bound_size_x = b_l * (wnz - 2 * h_l);
    // the size of the boundary in z without half_length is x=nx-2*h_l and z=b_l
    int bound_size_z = b_l * (wnx - 2 * h_l);

    int bound_size_y = 0;
    bool is_2d;
    int y_start;
    int y_end;

    if (wny == 1) {
        is_2d = 1;
        y_start = 0;
        y_end = 1;
    } else {
        y_start = h_l;
        y_end = wny - h_l;
        is_2d = 0;
        // size of boundary in y without half_length x=nx-2*h_l z=nz-2*h_l y= b_l
        bound_size_y = b_l * (wnx - 2 * h_l) * (wnz - 2 * h_l);

        // if 3d multiply by y size without half_length which is ny-2*h_l
        bound_size_x = bound_size_x * (wny - 2 * h_l);
        bound_size_z = bound_size_z * (wny - 2 * h_l);
    }

    // allocate the small arrays for coefficients

    mpSmall_a_x = new FrameBuffer<float>(b_l);
    mpSmall_a_y = new FrameBuffer<float>(b_l);
    mpSmall_a_z = new FrameBuffer<float>(b_l);
    mpSmall_b_x = new FrameBuffer<float>(b_l);
    mpSmall_b_y = new FrameBuffer<float>(b_l);
    mpSmall_b_z = new FrameBuffer<float>(b_l);


    // allocate the auxiliary variables for the boundary length for velocity in x
    // direction
    mpAuxiliary_vel_x_left = new FrameBuffer<float>(bound_size_x);
    mpAuxiliary_vel_x_right = new FrameBuffer<float>(bound_size_x);


    // allocate the auxiliary variables for the boundary length for velocity in z
    // direction
    mpAuxiliary_vel_z_up = new FrameBuffer<float>(bound_size_z);
    mpAuxiliary_vel_z_down = new FrameBuffer<float>(bound_size_z);

    // allocate the auxiliary variables for the boundary length for pressure in x
    // direction
    mpAuxiliary_ptr_x_left = new FrameBuffer<float>(bound_size_x);
    mpAuxiliary_ptr_x_right = new FrameBuffer<float>(bound_size_x);

    // allocate the auxiliary variables for the boundary length for pressure in z
    // direction
    mpAuxiliary_ptr_z_up = new FrameBuffer<float>(bound_size_z);
    mpAuxiliary_ptr_z_down = new FrameBuffer<float>(bound_size_z);


    // get the maximum velocity

    float *velocity_base = this->mpGridBox->Get(PARM | GB_VEL)->GetHostPointer();
    for (int k = 0; k < ny; ++k) {
        for (int j = 0; j < nz; ++j) {
            for (int i = 0; i < nx; ++i) {
                int offset = i + actual_nx * j + k * actual_nx * actual_nz;
                float velocity_real = velocity_base[offset];
                if (velocity_real > this->mMaxVelocity) {
                    this->mMaxVelocity = velocity_real;
                }
            }
        }
    }

    /// Put values for the small arrays

    StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
            mpSmall_a_x->GetNativePointer(), mpSmall_b_x->GetNativePointer(), b_l, h_l,
            this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension(), this->mpGridBox->GetDT(),
            this->mMaxVelocity, this->mShiftRatio, this->mReflectCoefficient, this->mRelaxCoefficient);


    StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
            mpSmall_a_z->GetNativePointer(), mpSmall_b_z->GetNativePointer(), b_l, h_l,
            this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension(), this->mpGridBox->GetDT(),
            this->mMaxVelocity, this->mShiftRatio, this->mReflectCoefficient, this->mRelaxCoefficient);


    if (ny > 1) {

        // allocate the auxiliary variables for the boundary length for velocity in y
        // direction
        mpAuxiliary_vel_y_up = new FrameBuffer<float>(bound_size_y);
        mpAuxiliary_vel_y_down = new FrameBuffer<float>(bound_size_y);

        // allocate the auxiliary variables for the boundary length for pressure in y
        // direction
        mpAuxiliary_ptr_y_up = new FrameBuffer<float>(bound_size_y);
        mpAuxiliary_ptr_y_down = new FrameBuffer<float>(bound_size_y);

        StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
                mpSmall_a_y->GetNativePointer(), mpSmall_b_y->GetNativePointer(), b_l, h_l,
                this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension(), this->mpGridBox->GetDT(),
                this->mMaxVelocity, this->mShiftRatio, this->mReflectCoefficient, this->mRelaxCoefficient);
    }

}


void
StaggeredCPMLBoundaryManager::InitializeExtensions() {
    uint params_size = this->mpGridBox->GetParameters().size();
    for (int i = 0; i < params_size; ++i) {
        this->mvExtensions.push_back(new HomogenousExtension(this->mUseTopLayer));
    }

    for (auto const &extension: this->mvExtensions) {
        extension->SetHalfLength(this->mpParameters->GetHalfLength());
        extension->SetBoundaryLength(this->mpParameters->GetBoundaryLength());
    }

    uint index = 0;
    for (auto const &parameter: this->mpGridBox->GetParameters()) {
        this->mvExtensions[index]->SetGridBox(this->mpGridBox);
        this->mvExtensions[index]->SetProperty(parameter.second->GetNativePointer(),
                                               this->mpGridBox->Get(WIND | parameter.first)->GetNativePointer());
        index++;
    }
}

void
StaggeredCPMLBoundaryManager::AdjustModelForBackward() {
    for (auto const &extension: this->mvExtensions) {
        extension->AdjustPropertyForBackward();
    }
    ZeroAuxiliaryVariables();
}

// this function used to reset the auxiliary variables to zero
void
StaggeredCPMLBoundaryManager::ZeroAuxiliaryVariables() {

    int wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();

    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();


    HALF_LENGTH half_length = mpParameters->GetHalfLength();
    int b_l = mpParameters->GetBoundaryLength();
    int index_2d = 0;
    if (wny != 1) {
        index_2d = half_length;
    }
    int bound_x = (lny - 2 * index_2d) * (lnz - 2 * half_length) * b_l;
    int bound_z = (lny - 2 * index_2d) * b_l * (lnx - 2 * half_length);
    int bound_y = b_l * (lnx - 2 * half_length) * (lnz - 2 * half_length);

    // X-Variables.
    Device::MemSet(this->mpAuxiliary_vel_x_left->GetNativePointer(), 0,
                   bound_x * sizeof(float));
    Device::MemSet(this->mpAuxiliary_vel_x_right->GetNativePointer(), 0,
                   bound_x * sizeof(float));
    Device::MemSet(this->mpAuxiliary_ptr_x_left->GetNativePointer(), 0,
                   bound_x * sizeof(float));
    Device::MemSet(this->mpAuxiliary_ptr_x_right->GetNativePointer(), 0,
                   bound_x * sizeof(float));
    // Z-Variables
    Device::MemSet(this->mpAuxiliary_vel_z_up->GetNativePointer(), 0,
                   bound_z * sizeof(float));
    Device::MemSet(this->mpAuxiliary_vel_z_down->GetNativePointer(), 0,
                   bound_z * sizeof(float));
    Device::MemSet(this->mpAuxiliary_ptr_z_up->GetNativePointer(), 0,
                   bound_z * sizeof(float));
    Device::MemSet(this->mpAuxiliary_ptr_z_down->GetNativePointer(), 0,
                   bound_z * sizeof(float));

    // for the auxiliaries in the z boundaries for all x and y
    if (wny != 1) {
        Device::MemSet(this->mpAuxiliary_vel_y_up->GetNativePointer(), 0,
                       bound_y * sizeof(float));
        Device::MemSet(this->mpAuxiliary_vel_y_down->GetNativePointer(), 0,
                       bound_y * sizeof(float));
        Device::MemSet(this->mpAuxiliary_ptr_y_up->GetNativePointer(), 0,
                       bound_y * sizeof(float));
        Device::MemSet(this->mpAuxiliary_ptr_y_down->GetNativePointer(), 0,
                       bound_y * sizeof(float));
    }
}

void
StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
        float *apCoeff_a, float *apCoeff_b, int aBoundaryLength, int aHalfLength, float aDh, float aDt,
        float aMaxVel, float aShiftRatio, float aReflectCoeff, float aRelaxCp) {

    float pml_reg_len = aBoundaryLength;
    float coeff_a_temp[aBoundaryLength];
    float coeff_b_temp[aBoundaryLength];

    if (pml_reg_len == 0) {
        pml_reg_len = 1;
    }

    float d0 = 0;
    // compute damping vector ...
    d0 =
            (-logf(aReflectCoeff) * ((3 * aMaxVel) / (pml_reg_len * aDh)) * aRelaxCp) /
            pml_reg_len;
    for (int i = aBoundaryLength; i > 0; i--) {
        float damping_ratio = i * i * d0;
        coeff_a_temp[i - 1] = expf(-aDt * (damping_ratio + aShiftRatio));
        coeff_b_temp[i - 1] =
                (damping_ratio / (damping_ratio + aShiftRatio)) * (coeff_a_temp[i - 1] - 1);
    }
    Device::MemCpy(apCoeff_a, coeff_a_temp, aBoundaryLength * sizeof(float),
                   dataunits::Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(apCoeff_b, coeff_b_temp, aBoundaryLength * sizeof(float),
                   dataunits::Device::COPY_HOST_TO_DEVICE);

    mpCoeff = new FrameBuffer<float>(aHalfLength);

    Device::MemCpy(mpCoeff->GetNativePointer(), mpParameters->GetFirstDerivativeStaggeredFDCoefficient(),
                   aHalfLength * sizeof(float),
                   dataunits::Device::COPY_HOST_TO_DEVICE);
}

void
StaggeredCPMLBoundaryManager::ApplyBoundary(uint aKernelId) {
    if (aKernelId == 0) {
        if (this->mAdjoint) {
            this->ApplyPressureCPML<true>();
        } else {
            this->ApplyPressureCPML<false>();
        }
    } else {
        if (this->mAdjoint) {
            this->ApplyVelocityCPML<true>();
        } else {
            this->ApplyVelocityCPML<false>();
        }
    }
}

template<bool ADJOINT_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::ApplyVelocityCPML() {

    int ny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();

    this->CalculatePressureFirstAuxiliary<ADJOINT_, X_AXIS, false, HALF_LENGTH_>();
    this->CalculatePressureFirstAuxiliary<ADJOINT_, X_AXIS, true, HALF_LENGTH_>();
    this->CalculatePressureFirstAuxiliary<ADJOINT_, Z_AXIS, false, HALF_LENGTH_>();
    this->CalculatePressureFirstAuxiliary<ADJOINT_, Z_AXIS, true, HALF_LENGTH_>();
    if (ny > 1) {
        this->CalculatePressureFirstAuxiliary<ADJOINT_, Y_AXIS, false, HALF_LENGTH_>();
        this->CalculatePressureFirstAuxiliary<ADJOINT_, Y_AXIS, true, HALF_LENGTH_>();
    }
    this->CalculateVelocityCPMLValue<ADJOINT_, X_AXIS, false, HALF_LENGTH_>();
    this->CalculateVelocityCPMLValue<ADJOINT_, X_AXIS, true, HALF_LENGTH_>();
    this->CalculateVelocityCPMLValue<ADJOINT_, Z_AXIS, false, HALF_LENGTH_>();
    this->CalculateVelocityCPMLValue<ADJOINT_, Z_AXIS, true, HALF_LENGTH_>();
    if (ny > 1) {
        this->CalculateVelocityCPMLValue<ADJOINT_, Y_AXIS, false, HALF_LENGTH_>();
        this->CalculateVelocityCPMLValue<ADJOINT_, Y_AXIS, true, HALF_LENGTH_>();
    }
}

template<bool ADJOINT_, int HALF_LENGTH_>
void
StaggeredCPMLBoundaryManager::ApplyPressureCPML() {

    int ny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();

    this->CalculateVelocityFirstAuxiliary<ADJOINT_, X_AXIS, false, HALF_LENGTH_>();
    this->CalculateVelocityFirstAuxiliary<ADJOINT_, X_AXIS, true, HALF_LENGTH_>();
    this->CalculateVelocityFirstAuxiliary<ADJOINT_, Z_AXIS, false, HALF_LENGTH_>();
    this->CalculateVelocityFirstAuxiliary<ADJOINT_, Z_AXIS, true, HALF_LENGTH_>();
    if (ny > 1) {
        this->CalculateVelocityFirstAuxiliary<ADJOINT_, Y_AXIS, false, HALF_LENGTH_>();
        this->CalculateVelocityFirstAuxiliary<ADJOINT_, Y_AXIS, true, HALF_LENGTH_>();
    }
    this->CalculatePressureCPMLValue<ADJOINT_, X_AXIS, false, HALF_LENGTH_>();
    this->CalculatePressureCPMLValue<ADJOINT_, X_AXIS, true, HALF_LENGTH_>();
    this->CalculatePressureCPMLValue<ADJOINT_, Z_AXIS, false, HALF_LENGTH_>();
    this->CalculatePressureCPMLValue<ADJOINT_, Z_AXIS, true, HALF_LENGTH_>();
    if (ny > 1) {
        this->CalculatePressureCPMLValue<ADJOINT_, Y_AXIS, false, HALF_LENGTH_>();
        this->CalculatePressureCPMLValue<ADJOINT_, Y_AXIS, true, HALF_LENGTH_>();
    }
}

template<bool ADJOINT_>
void
StaggeredCPMLBoundaryManager::ApplyVelocityCPML() {
    switch (this->mpParameters->GetHalfLength()) {
        case O_2:
            ApplyVelocityCPML < ADJOINT_, O_2 > ();
            break;
        case O_4:
            ApplyVelocityCPML < ADJOINT_, O_4 > ();
            break;
        case O_8:
            ApplyVelocityCPML < ADJOINT_, O_8 > ();
            break;
        case O_12:
            ApplyVelocityCPML < ADJOINT_, O_12 > ();
            break;
        case O_16:
            ApplyVelocityCPML < ADJOINT_, O_16 > ();
            break;
    }
}

template<bool ADJOINT_>
void
StaggeredCPMLBoundaryManager::ApplyPressureCPML() {
    switch (this->mpParameters->GetHalfLength()) {
        case O_2:
            ApplyPressureCPML < ADJOINT_, O_2 > ();
            break;
        case O_4:
            ApplyPressureCPML < ADJOINT_, O_4 > ();
            break;
        case O_8:
            ApplyPressureCPML < ADJOINT_, O_8 > ();
            break;
        case O_12:
            ApplyPressureCPML < ADJOINT_, O_12 > ();
            break;
        case O_16:
            ApplyPressureCPML < ADJOINT_, O_16 > ();
            break;
    }
}
