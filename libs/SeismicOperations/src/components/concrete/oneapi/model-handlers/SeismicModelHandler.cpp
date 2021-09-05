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

#include <operations/components/independents/concrete/model-handlers/SeismicModelHandler.hpp>
#include <operations/backend/OneAPIBackend.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#define make_divisible(v, d) (v + (d - (v % d)))

using namespace cl::sycl;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::backend;
using namespace bs::base::logger;

void SeismicModelHandler::SetupWindow() {
    if (mpParameters->IsUsingWindow()) {
        uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
        uint nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

        uint sx = mpGridBox->GetWindowStart(X_AXIS);
        uint sz = mpGridBox->GetWindowStart(Z_AXIS);
        uint sy = mpGridBox->GetWindowStart(Y_AXIS);


        uint offset = mpParameters->GetHalfLength() + mpParameters->GetBoundaryLength();
        uint start_x = offset;
        uint end_x = mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
        uint start_z = offset;
        uint end_z = mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

        uint start_y = 0;
        uint end_y = 1;
        if (ny != 1) {
            start_y = offset;
            end_y = mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - offset;

        }
        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](sycl::handler &cgh) {
            auto global_range = range<3>(end_x - start_x, end_y - start_y, end_z - start_z);

            auto local_range = range<3>(1, 1, 1);
            auto global_nd_range = nd_range<3>(global_range, local_range);
            float *vel = mpGridBox->Get(PARM | GB_VEL)->GetNativePointer();
            float *w_vel = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
            cgh.parallel_for<class model_handler>(global_nd_range, [=](sycl::nd_item<3> it) {
                int x = it.get_global_id(0) + start_x;
                int y = it.get_global_id(1) + start_y;
                int z = it.get_global_id(2) + start_z;
                uint offset_window = y * wnx * wnz + z * wnx + x;
                uint offset_full = (y + sy) * nx * nz + (z + sz) * nx + x + sx;
                w_vel[offset_window] = vel[offset_full];
            });
        });
        OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
    }
}

void SeismicModelHandler::SetupPadding() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto grid = mpGridBox;
    auto parameters = mpParameters;
    uint block_x = parameters->GetBlockX();
    uint block_z = parameters->GetBlockZ();

    uint nx = grid->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    uint ny = grid->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    uint nz = grid->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();
    uint inx = nx - 2 * parameters->GetHalfLength();
    uint inz = nz - 2 * parameters->GetHalfLength();

    uint rear_x_padding = 0, rear_z_padding = 0;
    uint aux_front_x_padding = 0, aux_rear_x_padding = 0;

    // Store old values of nx,nz,ny to use in boundaries/etc....

    if (block_x > inx) {
        block_x = inx;
        Logger->Info() << "Block Factor x > domain size... Reduced to domain size" << '\n';
    }
    if (block_z > inz) {
        block_z = inz;
        Logger->Info() << "Block Factor z > domain size... Reduced to domain size" << '\n';
    }
    if (block_x % 16 != 0 && block_x != 1) {
        block_x = make_divisible(
                block_x,
                16); // Ensure block in x is divisible by 16(biggest vector length).
        Logger->Info() << "Adjusting block factor in x to make it divisible by "
                          "16(Possible Vector Length)..." << '\n';
    }
    if (inx % block_x != 0) {
        Logger->Info() << "Adding padding to make domain divisible by block size in  x-axis" << '\n';

        uint inx_axis_dev = make_divisible(inx, block_x);

        rear_x_padding = inx_axis_dev - inx;

        grid->GetWindowAxis()->GetXAxis().AddComputationalPadding(OP_DIREC_REAR, rear_x_padding);

    }
    if (inz % block_z != 0) {
        Logger->Info()
                << "Adding padding to make domain divisible by block size in  z-axis" << '\n';

        uint inz_axis_dev = make_divisible(inz, block_z);

        rear_z_padding = inz_axis_dev - inz;

        grid->GetWindowAxis()->GetZAxis().AddComputationalPadding(OP_DIREC_REAR, rear_z_padding);

    }

    int actual_nx = grid->GetWindowAxis()->GetXAxis().GetActualAxisSize();

    if (actual_nx % 16 != 0) {
        //TODO this needs to be non-computational padding.
//        Logger->Info() << "Adding padding to ensure alignment of each row" << '\n';
//        uint actual_nx_dev = make_divisible(actual_nx, 16);
//
//        aux_front_x_padding = (actual_nx_dev - actual_nx) / 2;
//        aux_rear_x_padding = (actual_nx_dev - actual_nx) - aux_front_x_padding;
//
//        grid->GetWindowAxis()->GetXAxis().AddComputationalPadding(OP_DIREC_FRONT,
//                                                                  front_x_padding + aux_front_x_padding);
//        grid->GetWindowAxis()->GetXAxis().AddComputationalPadding(OP_DIREC_REAR, rear_x_padding + aux_rear_x_padding);

    }

    parameters->SetBlockX(block_x);
    parameters->SetBlockZ(block_z);

    if (!parameters->IsUsingWindow()) {
        this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().AddComputationalPadding(OP_DIREC_REAR, rear_x_padding +
                                                                                                   aux_rear_x_padding);

        this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().AddComputationalPadding(OP_DIREC_REAR, rear_z_padding);
    }

}
