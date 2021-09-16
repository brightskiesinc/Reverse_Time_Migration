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

#include <operations/configurations/MapKeys.h>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <iostream>
#include <cmath>

using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;


RickerSourceInjector::RickerSourceInjector(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mMaxFrequencyAmplitudePercentage = 0.05;
}

RickerSourceInjector::~RickerSourceInjector() = default;

void RickerSourceInjector::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mMaxFrequencyAmplitudePercentage = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES,
                                                                                OP_K_MAX_FREQ_AMP_PERCENT,
                                                                                this->mMaxFrequencyAmplitudePercentage);
    if (this->mMaxFrequencyAmplitudePercentage < 0 || this->mMaxFrequencyAmplitudePercentage > 1) {
        throw std::runtime_error("Max Frequency Amplitude Percentage must be between 0(exclusive) and 1");
    }
    float peak_frequency = this->mpParameters->GetSourceFrequency();
    float amp_peak = (2.0f * expf(-1)) / (sqrtf(M_PI) * peak_frequency);
    float target_amp =
            mMaxFrequencyAmplitudePercentage * amp_peak; //Target the cutoff peak frequency, when less than 1% of peak
    float max_freq = peak_frequency;
    float cur_amp = (2 * powf(max_freq, 2) * expf(-powf(max_freq / peak_frequency, 2)))
                    / (sqrtf(M_PI) * powf(peak_frequency, 3));
    while (cur_amp > target_amp) {
        max_freq += 1;
        cur_amp = (2 * powf(max_freq, 2) * expf(-powf(max_freq / peak_frequency, 2)))
                  / (sqrtf(M_PI) * powf(peak_frequency, 3));
    }
    this->mMaxFrequency = max_freq;
    Logger->Info() << "Using Ricker with peak frequency " << peak_frequency
                   << " and max frequency " << mMaxFrequency << '\n';

}

void RickerSourceInjector::ApplyIsotropicField() {
    /// @todo To be implemented.

    uint location = this->GetInjectionLocation();
    uint isotropic_radius = this->mpParameters->GetIsotropicRadius();

    // Loop on circular field
    //      1. epsilon[location] = 0.0f
    //      2. delta[location] = 0.0f
}

void RickerSourceInjector::RevertIsotropicField() {
    /// @todo To be implemented.
}

int RickerSourceInjector::GetCutOffTimeStep() {
    // At which time does the source injection finish.
    float dt = this->mpGridBox->GetDT();
    float freq = this->mpParameters->GetSourceFrequency();
    return roundf((1.0f / freq) / dt);
}

int RickerSourceInjector::GetPrePropagationNT() {
    // What time does the source need before the zero time step.
    float dt = this->mpGridBox->GetDT();
    float freq = this->mpParameters->GetSourceFrequency();
    return roundf(((1.0f / freq) / dt));
}

float RickerSourceInjector::GetMaxFrequency() {
    return this->mMaxFrequency;
}

void RickerSourceInjector::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void RickerSourceInjector::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void RickerSourceInjector::SetSourcePoint(Point3D *apSourcePoint) {
    this->mpSourcePoint = apSourcePoint;
}

uint RickerSourceInjector::GetInjectionLocation() {

    uint x = this->mpSourcePoint->x;
    uint y = this->mpSourcePoint->y;
    uint z = this->mpSourcePoint->z;

    uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();


    uint location = y * wnx * wnz + z * wnx + x;
    return location;
}