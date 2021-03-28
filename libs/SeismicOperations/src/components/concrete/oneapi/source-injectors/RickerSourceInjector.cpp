//
// Created by amr-nasr on 13/11/2019.
//

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
 */
void RickerSourceInjector::ApplySource(uint time_step) {
    float dt = mpGridBox->GetDT();
    float freq = mpParameters->GetSourceFrequency();

    int location = this->GetInjectionLocation();

    if (time_step < this->GetCutOffTimeStep()) {
        {
            float temp = M_PI * M_PI * freq * freq *
                         (((time_step - 1) * dt) - 1 / freq) *
                         (((time_step - 1) * dt) - 1 / freq);
            float ricker = (2 * temp - 1) * exp(-temp);

            OneAPIBackend::GetInstance()->GetDeviceQueue()->submit([&](handler &cgh) {
                auto pressure = mpGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
                auto win_vel = mpGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer();
                cgh.single_task<class source_injector>([=]() {
                    pressure[location] += (ricker * win_vel[location]);
                });
            });
            OneAPIBackend::GetInstance()->GetDeviceQueue()->wait();
        }
    }
}
