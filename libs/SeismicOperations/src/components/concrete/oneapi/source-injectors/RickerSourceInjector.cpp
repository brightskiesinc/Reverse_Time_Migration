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

#include <operations/components/independents/concrete/source-injectors/RickerSourceInjector.hpp>
#include <operations/backend/OneAPIBackend.hpp>
#include <iostream>
#include <cmath>

using namespace cl::sycl;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::backend;

/**
 * Implementation based on
 * https://tel.archives-ouvertes.fr/tel-00954506v2/document .
 * https://wiki.seg.org/wiki/Dictionary:Ricker_wavelet
 */
void RickerSourceInjector::ApplySource(int time_step) {
    float dt = mpGridBox->GetDT();
    float freq = mpParameters->GetSourceFrequency();

    int location = this->GetInjectionLocation();

    if (time_step < this->GetCutOffTimeStep()) {
        {
            float a = M_PI * freq;
            float a2 = a * a;
            float t = time_step * dt;
            float t2 = t * t;

            // ricker function required should have negative polarity
            float ricker = (1 - a2 * 2 * t2) * exp(-a2 * t2);

            OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
                auto pressure = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
                auto win_vel = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
                cgh.single_task([=]() {
                    pressure[location] += (ricker * win_vel[location]);
                });
            });
            OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
        }
    }
}
