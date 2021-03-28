//
// Created by mirna-moawad on 22/10/2019.
//

#include "operations/components/independents/concrete/model-handlers/SyntheticModelHandler.hpp"

#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

float SyntheticModelHandler::SetModelField(float *field, vector<float> &model_file,
                                           int nx, int nz, int ny,
                                           int logical_nx, int logical_nz, int logical_ny) {
    /// Extracting Velocity and size of each layer in terms of
    /// start(x,y,z) and end(x,y,z)
    float vel;
    int vsx, vsy, vsz, vex, vey, vez;
    float max = 0;
    for (int i = 0; i < model_file.size(); i += 7) {
        vel = model_file[i];
        if (vel > max) {
            max = vel;
        }
        /// Starting filling the velocity with required value
        /// value after shifting the padding of boundaries and half length
        int offset = this->mpParameters->GetBoundaryLength() + this->mpParameters->GetHalfLength();
        vsx = (int) model_file[i + 1] + offset;
        vsz = (int) model_file[i + 2] + offset;
        vsy = (int) model_file[i + 3];
        vex = (int) model_file[i + 4] + offset;
        vez = (int) model_file[i + 5] + offset;
        vey = (int) model_file[i + 6];

        if (ny > 1) {
            vsy = (int) vsy + offset;
            vey = (int) vey + offset;
        }
        if (vsx < 0 || vsy < 0 || vsz < 0) {
            cout << "Error at starting index values (x,y,z) (" << vsx << "," << vsy
                 << "," << vsz << ")" << endl;
            continue;
        }
        if (vex > logical_nx || vey > logical_ny || vez > logical_nz) {
            cout << "Error at ending index values (x,y,z) (" << vex << "," << vey
                 << "," << vez << ")" << endl;
            continue;
        }

        /// Initialize the layer with the extracted velocity value
#pragma omp parallel default(shared)
        {
#pragma omp for schedule(static) collapse(3)
            for (int y = vsy; y < vey; y++) {
                for (int z = vsz; z < vez; z++) {
                    for (int x = vsx; x < vex; x++) {
                        field[y * nx * nz + z * nx + x] = vel;
                    }
                }
            }
        }
    }
    return max;
}

void SyntheticModelHandler::PreprocessModel() {
    int nx = this->mpGridBox->GetActualGridSize(X_AXIS);
    int ny = this->mpGridBox->GetActualGridSize(Y_AXIS);
    int nz = this->mpGridBox->GetActualGridSize(Z_AXIS);
    int lnx = this->mpGridBox->GetLogicalGridSize(X_AXIS);
    int lny = this->mpGridBox->GetLogicalGridSize(Y_AXIS);
    int lnz = this->mpGridBox->GetLogicalGridSize(Z_AXIS);

    float dt = this->mpGridBox->GetDT();
    float dt2 = dt * dt;

    int full_nx = nx;
    int full_nx_nz = nx * nz;

    for (auto parameter : this->mpGridBox->GetParameters()) {
        if (GridBox::Includes(parameter.first, GB_VEL)) {
            float *parameter_ptr = parameter.second->GetNativePointer();
            /**
             * Preprocess the velocity model by calculating
             * dt^2 * c^2 component of the wave equation.
             */
#pragma omp parallel default(shared)
            {
#pragma omp for schedule(static) collapse(3)
                for (int y = 0; y < lny; ++y) {
                    for (int z = 0; z < lnz; ++z) {
                        for (int x = 0; x < lnx; ++x) {
                            float value = parameter_ptr[y * full_nx_nz + z * full_nx + x];
                            parameter_ptr[y * full_nx_nz + z * full_nx + x] =
                                    value * value * dt2;
                        }
                    }
                }
            }
        }
    }
}

void SyntheticModelHandler::SetupWindow() {
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

        uint offset = this->mpParameters->GetHalfLength() + this->mpParameters->GetBoundaryLength();
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
        for (uint iy = start_y; iy < end_y; iy++) {
            for (uint iz = start_z; iz < end_z; iz++) {
                for (uint ix = start_x; ix < end_x; ix++) {
                    uint offset_window = iy * wnx * wnz + iz * wnx + ix;
                    uint offset_full = (iy + sy) * nx * nz + (iz + sz) * nx + ix + sx;

                    for (auto const &parameter : this->mpGridBox->GetParameters()) {
                        float *param_window_ptr = this->mpGridBox->Get(WIND | parameter.first)->GetNativePointer();
                        float *param_ptr = parameter.second->GetNativePointer();
                        param_window_ptr[offset_window] = param_ptr[offset_full];
                    }
                }
            }
        }
    }
}

void SyntheticModelHandler::SetupPadding() {
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
    }
}
