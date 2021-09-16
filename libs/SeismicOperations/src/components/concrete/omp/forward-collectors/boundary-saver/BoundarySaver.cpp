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

#include <operations/components/independents/concrete/forward-collectors/boundary-saver/BoundarySaver.h>


using namespace operations::components::helpers;
using namespace operations::common;
using namespace operations::dataunits;

void BoundarySaver::SaveBoundaries(uint aStep) {
    uint index = 0;
    uint size_of_boundaries = this->mBoundarySize;
    uint time_step = aStep;
    uint half_length = this->mpComputationParameters->GetHalfLength();
    uint bound_length = this->mpComputationParameters->GetBoundaryLength();
    uint offset = half_length + bound_length;

    uint start_y = 0;
    uint end_y = 1;

    uint ny = this->mpMainGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();

    uint wnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    uint lnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    uint lny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    uint lnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

    uint start_z = offset;
    uint end_z = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;
    uint start_x = offset;
    uint end_x = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;

    uint wnznx = wnx * wnz;

    if (ny > 1) {
        start_y = offset;
        end_y = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - offset;

    }

    float *current_pressure = this->mpMainGridBox->Get(this->mKey)->GetNativePointer();
    float *backup_boundaries = this->mBackupBoundaries.GetNativePointer();
    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = start_z; iz < end_z; iz++) {
            for (int ix = 0; ix < half_length; ix++) {
                backup_boundaries[time_step * size_of_boundaries + index] =
                        current_pressure[iy * wnznx + iz * wnx + bound_length + ix];
                index++;
                backup_boundaries[time_step * size_of_boundaries + index] =
                        current_pressure[iy * wnznx + iz * wnx + (lnx - bound_length - 1) - ix];
                index++;
            }
        }
    }
    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = 0; iz < half_length; iz++) {
            for (int ix = start_x; ix < end_x; ix++) {
                backup_boundaries[time_step * size_of_boundaries + index] =
                        current_pressure[iy * wnznx + (bound_length + iz) * wnx + ix];
                index++;
                backup_boundaries[time_step * size_of_boundaries + index] =
                        current_pressure[iy * wnznx + (lnz - bound_length - 1 - iz) * wnx + ix];
                index++;
            }
        }
    }
    if (ny > 1) {
        for (int iy = 0; iy < half_length; iy++) {
            for (int iz = start_z; iz < end_z; iz++) {
                for (int ix = start_x; ix < end_x; ix++) {
                    backup_boundaries[time_step * size_of_boundaries + index] =
                            current_pressure[(bound_length + iy) * wnznx + iz * wnx + ix];
                    index++;
                    backup_boundaries[time_step * size_of_boundaries + index] =
                            current_pressure[(lny - bound_length - 1 - iy) * wnznx + iz * wnx + ix];
                    index++;
                }
            }
        }
    }
}

void BoundarySaver::RestoreBoundaries(uint aStep) {
    uint index = 0;
    uint size_of_boundaries = this->mBoundarySize;
    uint time_step = aStep;
    uint half_length = this->mpComputationParameters->GetHalfLength();
    uint bound_length = this->mpComputationParameters->GetBoundaryLength();
    uint offset = half_length + bound_length;

    uint start_y = 0;
    uint end_y = 1;

    ///WILL TEST
    uint ny = mpMainGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();

    uint wnx = mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    uint lnx = mpMainGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
    uint lny = mpMainGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
    uint lnz = mpMainGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

    uint start_z = offset;
    uint end_z = mpMainGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;
    uint start_x = offset;
    uint end_x = mpMainGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
    uint wnznx = wnx * wnz;
    if (ny > 1) {
        start_y = offset;
        end_y = mpMainGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - offset;
    }

    float *current_pressure = this->mpInternalGridBox->Get(this->mKey)->GetNativePointer();
    float *backup_boundaries = this->mBackupBoundaries.GetNativePointer();
    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = start_z; iz < end_z; iz++) {
            for (int ix = 0; ix < half_length; ix++) {
                current_pressure[iy * wnznx + iz * wnx + bound_length + ix] =
                        backup_boundaries[time_step * size_of_boundaries + index];
                index++;
                current_pressure[iy * wnznx + iz * wnx + (lnx - bound_length - 1) - ix] =
                        backup_boundaries[time_step * size_of_boundaries + index];
                index++;
            }
        }
    }
    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = 0; iz < half_length; iz++) {
            for (int ix = start_x; ix < end_x; ix++) {
                current_pressure[iy * wnznx + (bound_length + iz) * wnx + ix] =
                        backup_boundaries[time_step * size_of_boundaries + index];
                index++;
                current_pressure[iy * wnznx + (lnz - bound_length - 1 - iz) * wnx + ix] =
                        backup_boundaries[time_step * size_of_boundaries + index];
                index++;
            }
        }
    }
    if (ny > 1) {
        for (int iy = 0; iy < half_length; iy++) {
            for (int iz = start_z; iz < end_z; iz++) {
                for (int ix = start_x; ix < end_x; ix++) {
                    current_pressure[(bound_length + iy) * wnznx + iz * wnx + ix] =
                            backup_boundaries[time_step * size_of_boundaries + index];
                    index++;
                    current_pressure[(lny - bound_length - 1 - iy) * wnznx + iz * wnx + ix] =
                            backup_boundaries[time_step * size_of_boundaries + index];
                    index++;
                }
            }
        }
    }
}
