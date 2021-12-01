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

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>

using namespace bs::timer;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


void
WaveFieldsMemoryHandler::FirstTouch(float *ptr, GridBox *apGridBox, bool enable_window) {
    int nx, ny, nz;
    if (enable_window) {
        nx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        ny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        nz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();
    } else {
        nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
        ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
        nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();
    }

    {
        ScopeTimer t("ComputationKernel::FirstTouch");
        /* Set the device arrays to zero. */
        Device::MemSet(ptr, 0.0f, nx * ny * nz * sizeof(float));
    }
}
