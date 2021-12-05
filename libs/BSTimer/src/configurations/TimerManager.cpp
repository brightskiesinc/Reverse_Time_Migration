/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer.
 *
 * BS Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <bs/base/common/ExitCodes.hpp>

#include <bs/timer/configurations/TimerManager.hpp>
#include <bs/timer/reporter/TimerReporter.hpp>

using namespace bs::timer;
using namespace bs::timer::core;
using namespace bs::timer::configurations;
using namespace bs::base::configurations;


void
TimerManager::Configure(ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->AcquireConfiguration();
}

void
TimerManager::Terminate(bool aCleanup) {
    for (const auto &channel : this->mChannelMap) {
        if (channel.second != nullptr) {
            for (auto timer : channel.second->GetTimers()) {
                if (timer != nullptr) {
                    if (timer->IsActive()) {
                        timer->Stop();
                    }
                }
            }
        }
    }
    if (aCleanup) {
        this->Cleanup();
    }
}

TimerChannel::Pointer
TimerManager::Get(const char *apChannelName) {
    return this->mChannelMap[apChannelName];
}

void
TimerManager::Cleanup() {
    this->mChannelMap.clear();
}

std::vector<Timer *>
TimerManager::GetActiveTimers() {
    std::vector<bs::timer::Timer *> vec;
    for (const auto &channel  : this->mChannelMap) {
        for (auto timer : channel.second->GetTimers()) {
            if (timer->IsActive()) {
                vec.push_back(timer);
            }
        }
    }
    return vec;
}

int
TimerManager::Report(const std::vector<std::ostream *> &aStreams, const std::string &aChannelName) {
    int rc = BS_BASE_RC_SUCCESS;
    for (auto stream : aStreams) {
        rc += (this->Report(*stream, aChannelName)).empty() ? BS_BASE_RC_FAILURE : BS_BASE_RC_SUCCESS;
    }
    reporter::TimerReporter r;
    rc += r.FlushReport(BS_TIMER_WRITE_PATH "/data" BS_TIMER_EXT);

    return rc;
}

std::string
TimerManager::Report(std::ostream &aOutputStream, const std::string &aChannelName) {
    reporter::TimerReporter r;
    return r.GenerateStream(aOutputStream, aChannelName);
}

void
TimerManager::RegisterChannel(const TimerChannel::Pointer &apChannel) {
    if (this->mChannelMap.count(apChannel->GetName()) == 0) {
        /* New channel being registered. */
        this->mChannelMap[apChannel->GetName()] = apChannel;
    }
}

TimerChannel::Map
TimerManager::GetMap() {
    return this->mChannelMap;
}

void
TimerManager::AcquireConfiguration() {
    this->mTimePrecision = this->mpConfigurationMap->GetValue(BS_TIMER_K_PROPERTIES, BS_TIMER_K_TIME_UNIT, 1.0);
}

double
TimerManager::GetPrecision() {
    return this->mTimePrecision;
}
