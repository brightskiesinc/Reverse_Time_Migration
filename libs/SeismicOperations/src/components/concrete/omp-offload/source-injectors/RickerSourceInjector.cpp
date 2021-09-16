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

#include <operations/utils/checks/Checks.hpp>

#include <iostream>
#include <math.h>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;
using namespace operations::utils::checks;
using namespace bs::base::exceptions;


/**
 * Implementation based on
 * https://tel.archives-ouvertes.fr/tel-00954506v2/document .
 */

// Add the ricker value for the pressure array
#pragma omp declare target

void ApplySourceOnPressure(float aTemp, int aLocation, float *aPressure, float *aVelocity) {
    float ricker = aTemp * aVelocity[aLocation];
    aPressure[aLocation] += ricker;
}

#pragma omp end declare target

void RickerSourceInjector::ApplySource(int time_step) {

    float dt = this->mpGridBox->GetDT();
    float freq = this->mpParameters->GetSourceFrequency();


    if (time_step < this->GetCutOffTimeStep()) {


        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }


        float a = M_PI * freq;
        float a2 = a * a;
        float t = time_step * dt;
        float t2 = t * t;

        float temp = (1 - a2 * 2 * t2) * exp(-a2 * t2);

        int location = this->GetInjectionLocation();

        /* Finding gpu device. */
        int device_num = omp_get_default_device();

        float *vel = this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();

        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {

            float *pressure_horizontal = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
#pragma omp target is_device_ptr(pressure_horizontal, vel) device(device_num)
            ApplySourceOnPressure(temp, location, pressure_horizontal, vel);

            float *pressure_vertical = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
#pragma omp target is_device_ptr(pressure_vertical, vel) device(device_num)
            ApplySourceOnPressure(temp, location, pressure_vertical, vel);
        } else {
            float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR)->GetNativePointer();
#pragma omp target is_device_ptr(pressure, vel) device(device_num)
            ApplySourceOnPressure(temp, location, pressure, vel);
        }
    }
}
