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
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

using namespace cl::sycl;
using namespace bs::base::backend;
using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void WaveFieldsMemoryHandler::FirstTouch(float *ptr, GridBox *apGridBox, bool enable_window) {
    if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        int nx, ny, nz, compute_nx, compute_nz;

        if (enable_window) {

            nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
            ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
            nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
            compute_nx = apGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();
            compute_nz = apGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();

        } else {

            nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
            ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
            nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();
            compute_nx = apGridBox->GetWindowAxis()->GetXAxis().GetComputationAxisSize();
            compute_nz = apGridBox->GetWindowAxis()->GetZAxis().GetComputationAxisSize();

        }
        ElasticTimer timer("ComputationKernel::FirstTouch");
        timer.Start();
        if (Backend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
            Backend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
                size_t num_groups = Backend::GetInstance()->GetWorkgroupNumber();
                size_t wgsize = Backend::GetInstance()->GetWorkgroupSize();
                size_t z_stride = compute_nz / num_groups;
                auto global_range = range<1>(num_groups);
                auto local_range = range<1>(wgsize);
                const size_t wnx = nx;
                const int hl = mpParameters->GetHalfLength();
                float *curr_base = ptr;
                cgh.parallel_for_work_group(global_range, local_range, [=](group<1> grp) {
                    size_t z_id = grp.get_id(0) * z_stride + hl;
                    size_t end_z =
                            (z_id + z_stride) < (compute_nz + hl) ? (z_id + z_stride) : (compute_nz + hl);
                    grp.parallel_for_work_item([&](h_item<1> it) {
                        for (size_t iz = z_id; iz < end_z; iz++) {
                            size_t offset = iz * wnx + it.get_local_id(0);
                            for (size_t ix = hl; ix < compute_nx; ix += wgsize) {
                                size_t idx = offset + ix;
                                curr_base[idx] = 0;
                            }
                        }
                    });
                });
            });
            Backend::GetInstance()->GetDeviceQueue()->wait();
        }
        Device::MemSet(ptr, 0, nx * nz * ny * sizeof(float));
        timer.Stop();
    }
}
