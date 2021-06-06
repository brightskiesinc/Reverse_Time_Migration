//
// Created by ingy-mounir.
//


#include <operations/components/independents/concrete/source-injectors/RickerSourceInjector.hpp>
#include <iostream>
#include <math.h>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

/**
 * Implementation based on
 * https://tel.archives-ouvertes.fr/tel-00954506v2/document .
 */
void RickerSourceInjector::ApplySource(uint time_step) {
    float dt = this->mpGridBox->GetDT();
    float freq = this->mpParameters->GetSourceFrequency();
    int location = this->GetInjectionLocation();

    if (time_step < this->GetCutOffTimeStep()) {
        float temp = M_PI * M_PI * freq * freq *
                     (((time_step - 1) * dt) - 1 / freq) *
                     (((time_step - 1) * dt) - 1 / freq);
        float ricker = (2 * temp - 1) * exp(-temp);
        ricker = ricker * this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer()[location];

        int device_num = omp_get_default_device();

        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {

            float *pressure_horizontal = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();

#pragma omp target is_device_ptr(pressure_horizontal) device(device_num)
            pressure_horizontal[location] += ricker;

            float *pressure_vertical = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

#pragma omp target is_device_ptr(pressure_vertical) device(device_num)
            pressure_vertical[location] += ricker;
        } else {

            float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR)->GetNativePointer();
#pragma omp target is_device_ptr(pressure) device(device_num)

            pressure[location] += ricker;

        }
    }
}
