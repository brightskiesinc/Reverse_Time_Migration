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

#include "operations/components/independents/concrete/boundary-managers/SpongeBoundaryManager.hpp"
#include <operations/backend/OneAPIBackend.hpp>

#include <iostream>
#include <cmath>

using namespace cl::sycl;
using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::backend;

/// Based on
/// https://pubs.geoscienceworld.org/geophysics/article-abstract/50/4/705/71992/A-nonreflecting-boundary-condition-for-discrete?redirectedFrom=fulltext

void SpongeBoundaryManager::ApplyBoundaryOnField(float *next) {
    /**
     * Sponge boundary implementation.
     **/


    int nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int ny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    int lnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    int lny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    int lnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();
    int original_nx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetAxisSize();
    int original_nz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetAxisSize();


    uint bound_length = mpParameters->GetBoundaryLength();
    uint half_length = mpParameters->GetHalfLength();

    int y_start = half_length + bound_length;

    int y_end = lny - half_length - bound_length;

    float *dev_sponge_coeffs = this->mpSpongeCoefficients->GetNativePointer();

    if (ny == 1) {
        y_start = 0;
        y_end = 1;
    }

    OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(original_nx,
                                            y_end - y_start,
                                            (half_length + bound_length - 1) - (half_length) + 1),
                                   [=](id<3> i) {
                                       int ix = i[0] + (half_length + bound_length);
                                       int iy = i[1] + y_start;
                                       int iz = (half_length + bound_length - 1) - 1 - i[2] + 1;

                                       next[iy * nx * nz + iz * nx + ix] *= dev_sponge_coeffs[iz - half_length];
                                       next[iy * nx * nz + (iz + lnz - 2 * iz - 1) * nx +
                                            ix] *= dev_sponge_coeffs[iz - half_length];
                                   });
    });

    OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>((half_length + bound_length - 1) - (half_length) + 1,
                                            y_end - y_start,
                                            original_nz),
                                   [=](id<3> i) {
                                       int ix = (half_length + bound_length - 1) - 1 - i[0] + 1;
                                       int iy = i[1] + y_start;
                                       int iz = i[2] + (half_length + bound_length);

                                       next[iy * nx * nz + iz * nx + ix] *= dev_sponge_coeffs[ix - half_length];
                                       next[iy * nx * nz + iz * nx +
                                            (ix + lnx - 2 * ix - 1)] *= dev_sponge_coeffs[ix - half_length];
                                   });
    });


    if (ny > 1) {
        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.parallel_for(
                    range<3>(original_nx,
                             (half_length + bound_length - 1) - (half_length) + 1,
                             original_nz), [=](id<3> i) {
                        int ix = i[0] + (half_length + bound_length);
                        int iy = (half_length + bound_length - 1) - 1 - i[1] + 1;
                        int iz = i[2] + (half_length + bound_length);

                        next[iy * nx * nz + iz * nx + ix] *= dev_sponge_coeffs[iy - half_length];
                        next[(iy + lny - 2 * iy - 1) * nx * nz + iz * nx + ix] *= dev_sponge_coeffs[iy -
                                                                                                    half_length];
                    });
        });

    }

    int start_y = y_start;
    int start_x = half_length;
    int start_z = half_length;
    int end_y = y_end;
    int end_x = lnx - half_length;
    int end_z = lnz - half_length;
    int nz_nx = nx * nz;

    // Zero-Corners in the boundaries nx-nz boundary intersection--boundaries not needed.
    OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
        cgh.parallel_for(range<3>(end_y - start_y,
                                                         bound_length,
                                                         bound_length), [=](id<3> i) {
            int depth = i[0] + start_y;
            int row = i[1];
            int column = i[2];

            next[depth * nz_nx + (start_z + row) * nx + column + start_x] *= std::min(dev_sponge_coeffs[column],
                                                                                      dev_sponge_coeffs[row]);
            next[depth * nz_nx + (end_z - 1 - row) * nx + column + start_x] *= std::min(dev_sponge_coeffs[column],
                                                                                        dev_sponge_coeffs[row]);
            next[depth * nz_nx + (start_z + row) * nx + (end_x - 1 - column)] *= std::min(dev_sponge_coeffs[column],
                                                                                          dev_sponge_coeffs[row]);
            next[depth * nz_nx + (end_z - 1 - row) * nx + (end_x - 1 - column)] *= std::min(dev_sponge_coeffs[column],
                                                                                            dev_sponge_coeffs[row]);
        });
    });


    // If 3-D, zero corners in the y-x and y-z plans.
    if (ny > 1) {
        // Zero-Corners in the boundaries ny-nz boundary intersection--boundaries not needed.
        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.parallel_for(range<3>(bound_length,
                                                             bound_length,
                                                             end_x - start_x), [=](id<3> i) {
                int depth = i[0];
                int row = i[1];
                int column = i[2] + start_x;

                next[(depth + start_y) * nz_nx + (start_z + row) * nx + column] *= std::min(dev_sponge_coeffs[depth],
                                                                                            dev_sponge_coeffs[row]);
                next[(depth + start_y) * nz_nx + (end_z - 1 - row) * nx + column] *= std::min(dev_sponge_coeffs[depth],
                                                                                              dev_sponge_coeffs[row]);
                next[(end_y - 1 - depth) * nz_nx + (start_z + row) * nx + column] *= std::min(dev_sponge_coeffs[depth],
                                                                                              dev_sponge_coeffs[row]);
                next[(end_y - 1 - depth) * nz_nx + (end_z - 1 - row) * nx + column] *= std::min(
                        dev_sponge_coeffs[depth],
                        dev_sponge_coeffs[row]);
            });
        });

        OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
            cgh.parallel_for(range<3>(bound_length,
                                                             end_z - start_z,
                                                             bound_length), [=](id<3> i) {
                int depth = i[0];
                int row = i[1] + start_z;
                int column = i[2];

                next[(depth + start_y) * nz_nx + row * nx + column + start_x] *= std::min(dev_sponge_coeffs[column],
                                                                                          dev_sponge_coeffs[depth]);
                next[(end_y - 1 - depth) * nz_nx + row * nx + column + start_x] *= std::min(dev_sponge_coeffs[column],
                                                                                            dev_sponge_coeffs[depth]);
                next[(depth + start_y) * nz_nx + row * nx + (end_x - 1 - column)] *= std::min(dev_sponge_coeffs[column],
                                                                                              dev_sponge_coeffs[depth]);
                next[(end_y - 1 - depth) * nz_nx + row * nx + (end_x - 1 - column)] *= std::min(
                        dev_sponge_coeffs[column],
                        dev_sponge_coeffs[depth]);
            });
        });
    }
    OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
}
