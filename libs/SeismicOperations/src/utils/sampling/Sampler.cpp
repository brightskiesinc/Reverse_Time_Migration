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

#include <operations/utils/sampling/Sampler.hpp>

#include <operations/utils/interpolation/Interpolator.hpp>

#include <string>

using namespace operations::utils::sampling;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::utils::interpolation;

void Sampler::Resize(float *input, float *output, Axis3D<unsigned int> *apInputGridBox,
                     Axis3D<unsigned int> *apOutputGridBox, ComputationParameters *apParameters) {


    std::string name;

    int pre_x = apInputGridBox->GetXAxis().GetLogicalAxisSize();
    int pre_y = apInputGridBox->GetYAxis().GetLogicalAxisSize();
    int pre_z = apInputGridBox->GetZAxis().GetLogicalAxisSize();

    int post_x = apOutputGridBox->GetXAxis().GetLogicalAxisSize();
    int post_y = apOutputGridBox->GetYAxis().GetLogicalAxisSize();
    int post_z = apOutputGridBox->GetZAxis().GetLogicalAxisSize();


    if (pre_x != post_x || pre_z != post_z || pre_y != post_y) {
        Interpolator::InterpolateTrilinear(
                input,
                output,
                pre_x, pre_z, pre_y,
                post_x, post_z, post_y,
                apParameters->GetBoundaryLength(),
                apParameters->GetHalfLength());
    } else {
        memcpy(output, input, sizeof(float) * pre_x * pre_z * pre_y);
    }
}

void Sampler::CalculateAdaptiveCellDimensions(GridBox *apGridBox, ComputationParameters *apParameters,
                                              int aMinimumVelocity, float aMaxFrequency) {

    float old_nx = apGridBox->GetInitialAxis()->GetXAxis().GetAxisSize();
    float old_ny = apGridBox->GetInitialAxis()->GetYAxis().GetAxisSize();
    float old_nz = apGridBox->GetInitialAxis()->GetZAxis().GetAxisSize();

    float old_dx = apGridBox->GetInitialAxis()->GetXAxis().GetCellDimension();
    float old_dy = apGridBox->GetInitialAxis()->GetYAxis().GetCellDimension();
    float old_dz = apGridBox->GetInitialAxis()->GetZAxis().GetCellDimension();

    float maximum_frequency = aMaxFrequency;
    float minimum_wavelength = aMinimumVelocity / maximum_frequency;
    int stencil_order = apParameters->GetHalfLength();

    float minimum_wavelength_points = 4;

    switch (stencil_order) {
        case O_2:
            minimum_wavelength_points = 10;
            break;
        case O_4:
            minimum_wavelength_points = 5;
            break;
        case O_8:
            minimum_wavelength_points = 4;
            break;
        case O_12:
            minimum_wavelength_points = 3.5;
            break;
        case O_16:
            minimum_wavelength_points = 3.2;
            break;
        default:
            minimum_wavelength_points = 4;
    }

    float meters_per_cell = (minimum_wavelength / minimum_wavelength_points) * 0.9; // safety factor

    float new_dx = meters_per_cell;
    float new_dz = meters_per_cell;
    float new_dy = 1;
    if (old_ny > 1) {
        new_dy = meters_per_cell;
    }

    int new_domain_x = (old_nx * old_dx) / new_dx;
    int new_domain_z = (old_nz * old_dz) / new_dz;
    int new_domain_y = (old_ny * old_dy) / new_dy;


    apGridBox->GetAfterSamplingAxis()->SetXAxis(new_domain_x);
    apGridBox->GetAfterSamplingAxis()->SetZAxis(new_domain_z);

    apGridBox->GetAfterSamplingAxis()->GetXAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());
    apGridBox->GetAfterSamplingAxis()->GetZAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());

    apGridBox->GetAfterSamplingAxis()->GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH, apParameters->GetHalfLength());
    apGridBox->GetAfterSamplingAxis()->GetZAxis().AddHalfLengthPadding(OP_DIREC_BOTH, apParameters->GetHalfLength());

    if (old_ny == 1) {

        apGridBox->GetAfterSamplingAxis()->SetYAxis(1);
        apGridBox->GetAfterSamplingAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, 0);
        apGridBox->GetAfterSamplingAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, 0);

    } else {
        apGridBox->GetAfterSamplingAxis()->SetYAxis(new_domain_y);
        apGridBox->GetAfterSamplingAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());
        apGridBox->GetAfterSamplingAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                           apParameters->GetHalfLength());

    }

    apGridBox->GetAfterSamplingAxis()->GetXAxis().SetCellDimension(new_dx);
    apGridBox->GetAfterSamplingAxis()->GetYAxis().SetCellDimension(new_dy);
    apGridBox->GetAfterSamplingAxis()->GetZAxis().SetCellDimension(new_dz);

    unsigned int old_window_nx = apGridBox->GetWindowAxis()->GetXAxis().GetAxisSize();
    unsigned int old_window_nz = apGridBox->GetWindowAxis()->GetZAxis().GetAxisSize();
    unsigned int old_window_ny = apGridBox->GetWindowAxis()->GetYAxis().GetAxisSize();


    int new_window_nx = ((old_window_nx) * old_dx) / new_dx;
    int new_window_nz = ((old_window_nz) * old_dz) / new_dz;
    int new_window_ny = old_window_ny;


    if (old_window_ny > 1) {
        new_window_ny = ((old_window_ny) * old_dy) / new_dy;
    }

    apGridBox->SetWindowAxis(new Axis3D<unsigned int>(new_window_nx, new_window_ny, new_window_nz));

    if (old_window_ny > 1) {
        apGridBox->GetWindowAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());
        apGridBox->GetWindowAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, apParameters->GetHalfLength());
    } else {
        apGridBox->GetWindowAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, 0);
        apGridBox->GetWindowAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, 0);
    }

    apGridBox->GetWindowAxis()->GetXAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());
    apGridBox->GetWindowAxis()->GetZAxis().AddBoundary(OP_DIREC_BOTH, apParameters->GetBoundaryLength());
    apGridBox->GetWindowAxis()->GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH, apParameters->GetHalfLength());
    apGridBox->GetWindowAxis()->GetZAxis().AddHalfLengthPadding(OP_DIREC_BOTH, apParameters->GetHalfLength());

    int new_right_window = apParameters->GetRightWindow() * old_dx / new_dx;
    int new_left_window = apParameters->GetLeftWindow() * old_dx / new_dx;

    int new_front_window = apParameters->GetFrontWindow() * old_dy / new_dy;
    int new_back_window = apParameters->GetBackWindow() * old_dy / new_dy;

    int new_depth_window = apParameters->GetDepthWindow() * old_dz / new_dz;

    apParameters->SetRightWindow(new_right_window);
    apParameters->SetLeftWindow(new_left_window);
    apParameters->SetFrontWindow(new_front_window);
    apParameters->SetBackWindow(new_back_window);
    apParameters->SetDepthWindow(new_depth_window);

}
