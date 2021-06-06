//
// Created by ingy-mounir.
//

#include <operations/components/independents/concrete/model-handlers/SeismicModelHandler.hpp>

#include <timer/Timer.h>
#include <seismic-io-framework/datatypes.h>
#include <omp.h>
#include <iostream>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

//optimized version 
void SeismicModelHandler::PreprocessModel() {
    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int ny = this->mpGridBox->GetActualGridSize(Y_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

    float dt = this->mpGridBox->GetDT();
    float dt2 = dt * dt;

    float *velocity_values = this->mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();

    int full_nx = nx;
    int full_nx_nz = nx * nz;

    int device_num = omp_get_default_device();

    if (this->mpParameters->GetEquationOrder() == FIRST) {
        float *density_values = this->mpGridBox->Get(PARM | GB_DEN)->GetNativePointer();
        /// Preprocess the velocity model by calculating the
        /// dt * c2 * density component of the wave equation.

#pragma omp target is_device_ptr(velocity_values, density_values) device(device_num)
#pragma omp teams distribute parallel for collapse(2) schedule(static, 1)
        for (int z = 0; z < nz; ++z) {
            for (int x = 0; x < nx; ++x) {
                float value = velocity_values[z * full_nx + x];
                int offset = full_nx + x;
                velocity_values[offset] =
                        value * value * dt * density_values[offset];
                if (density_values[offset] != 0) {
                    density_values[offset] = dt / density_values[offset];
                }
            }
        }
    } else {
        /// Preprocess the velocity model by calculating the
        /// dt2 * c2 component of the wave equation.

#pragma omp target is_device_ptr(velocity_values) device(device_num)
#pragma omp teams distribute parallel for collapse(3) schedule(static, 1)
        for (int y = 0; y < ny; ++y) {
            for (int z = 0; z < nz; ++z) {
                for (int x = 0; x < nx; ++x) {
                    float value = velocity_values[y * full_nx_nz + z * full_nx + x];
                    velocity_values[y * full_nx_nz + z * full_nx + x] =
                            value * value * dt2;
                }
            }
        }
    }
}

void SeismicModelHandler::SetupWindow() {
    if (this->mpParameters->IsUsingWindow()) {
        uint wnx = this->mpGridBox->GetActualWindowSize(X_AXIS);
        uint wny = this->mpGridBox->GetActualWindowSize(Y_AXIS);
        uint wnz = this->mpGridBox->GetActualWindowSize(Z_AXIS);

        uint nx = this->mpGridBox->GetActualGridSize(X_AXIS);
        uint ny = this->mpGridBox->GetActualGridSize(Y_AXIS);
        uint nz = this->mpGridBox->GetActualGridSize(Z_AXIS);

        uint sx = this->mpGridBox->GetWindowStart(X_AXIS);
        uint sy = this->mpGridBox->GetWindowStart(Y_AXIS);
        uint sz = this->mpGridBox->GetWindowStart(Z_AXIS);

        uint offset = this->mpParameters->GetHalfLength() +
                      this->mpParameters->GetBoundaryLength();
        uint start_x = offset;
        uint end_x = this->mpGridBox->GetLogicalWindowSize(X_AXIS) - offset;
        uint start_z = offset;
        uint end_z = this->mpGridBox->GetLogicalWindowSize(Z_AXIS) - offset;
        uint start_y = 0;
        uint end_y = 1;

        if (ny != 1) {
            start_y = offset;
            end_y = this->mpGridBox->GetLogicalWindowSize(Y_AXIS) - offset;
        }

        int device_num = omp_get_default_device();

        for (auto const &parameter : this->mpGridBox->GetParameters()) {
            float *window_param = this->mpGridBox->Get(WIND | parameter.first)->GetNativePointer();
            float *param_ptr = this->mpGridBox->Get(parameter.first)->GetNativePointer();


#pragma omp target is_device_ptr(window_param, param_ptr) device(device_num)
#pragma omp teams distribute parallel for collapse(3) schedule(static, 1)
            for (uint iy = start_y; iy < end_y; iy++) {
                for (uint iz = start_z; iz < end_z; iz++) {
                    for (uint ix = start_x; ix < end_x; ix++) {
                        uint offset_window = iy * wnx * wnz + iz * wnx + ix;
                        uint offset_full = (iy + sy) * nx * nz + (iz + sz) * nx + ix + sx;
                        window_param[offset_window] = param_ptr[offset_full];
                    }
                }
            }

        }
    }
}

void SeismicModelHandler::SetupPadding() {
    this->mpGridBox->SetActualGridSize(X_AXIS, this->mpGridBox->GetLogicalGridSize(X_AXIS));
    this->mpGridBox->SetActualGridSize(Y_AXIS, this->mpGridBox->GetLogicalGridSize(Y_AXIS));
    this->mpGridBox->SetActualGridSize(Z_AXIS, this->mpGridBox->GetLogicalGridSize(Z_AXIS));
    this->mpGridBox->SetActualWindowSize(X_AXIS, this->mpGridBox->GetLogicalWindowSize(X_AXIS));
    this->mpGridBox->SetActualWindowSize(Y_AXIS, this->mpGridBox->GetLogicalWindowSize(Y_AXIS));
    this->mpGridBox->SetActualWindowSize(Z_AXIS, this->mpGridBox->GetLogicalWindowSize(Z_AXIS));
    this->mpGridBox->SetComputationGridSize(X_AXIS,
                                            this->mpGridBox->GetLogicalWindowSize(X_AXIS) -
                                            2 * this->mpParameters->GetHalfLength());
    this->mpGridBox->SetComputationGridSize(Z_AXIS,
                                            this->mpGridBox->GetLogicalWindowSize(Z_AXIS) -
                                            2 * this->mpParameters->GetHalfLength());
    if (this->mpGridBox->GetLogicalWindowSize(Y_AXIS) > 1) {
        this->mpGridBox->SetComputationGridSize(Y_AXIS,
                                                this->mpGridBox->GetLogicalWindowSize(Y_AXIS) -
                                                2 * this->mpParameters->GetHalfLength());
    } else {
        this->mpGridBox->SetComputationGridSize(Y_AXIS, 1);
    }
}

