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

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void SeismicModelHandler::SetupWindow() {
    if (this->mpParameters->IsUsingWindow()) {
        int wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        int wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        int nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        int nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();


        uint sx = this->mpGridBox->GetWindowStart(X_AXIS);
        uint sz = this->mpGridBox->GetWindowStart(Z_AXIS);

        uint offset = this->mpParameters->GetHalfLength() +
                      this->mpParameters->GetBoundaryLength();
        uint start_x = offset;
        uint end_x = this->mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - offset;
        uint start_z = offset;
        uint end_z = this->mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - offset;

        for (uint iz = start_z; iz < end_z; iz++) {
            for (uint ix = start_x; ix < end_x; ix++) {
                uint offset_window = iz * wnx + ix;
                uint offset_full = (iz + sz) * nx + ix + sx;

                for (auto const &parameter : this->mpGridBox->GetParameters()) {
                    float *window_param = this->mpGridBox->Get(WIND | parameter.first)->GetNativePointer();
                    float *param_ptr = this->mpGridBox->Get(parameter.first)->GetNativePointer();
                    window_param[offset_window] = param_ptr[offset_full];
                }
            }
        }
    }
}

void SeismicModelHandler::SetupPadding() {

}
