//
// Created by amr-nasr on 13/11/2019.
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

    if (time_step < this->GetCutOffTimeStep()) {
        float temp = M_PI * M_PI * freq * freq *
                     (((time_step - 1) * dt) - 1 / freq) *
                     (((time_step - 1) * dt) - 1 / freq);
        float ricker = (2 * temp - 1) * exp(-temp);
        uint location = this->GetInjectionLocation();
        ricker = ricker * this->mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer()[location];

        if (this->mpParameters->GetApproximation() == VTI ||
            this->mpParameters->GetApproximation() == TTI) {
            float *pressure_horizontal = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_X)->GetNativePointer();
            pressure_horizontal[location] += ricker;

            float *pressure_vertical = this->mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
            pressure_vertical[location] += ricker;
        } else {
            float *pressure = this->mpGridBox->Get(WAVE | GB_PRSS | CURR)->GetNativePointer();
            pressure[location] += ricker;
        }
    }
}
