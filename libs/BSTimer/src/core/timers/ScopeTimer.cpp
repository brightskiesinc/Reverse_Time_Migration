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

#include <bs/timer/core/timers/concrete/ScopeTimer.hpp>

using namespace std;
using namespace bs::timer;
using namespace bs::timer::core::snapshots;


ScopeTimer::ScopeTimer(const TimerChannel::Pointer &apChannel, SnapshotTarget aSnapshotTarget) {
    this->mElasticTimer = new ElasticTimer(apChannel, aSnapshotTarget);
    this->Start();
}

ScopeTimer::ScopeTimer(const char *apChannelName, SnapshotTarget aSnapshotTarget) {
    this->mElasticTimer = new ElasticTimer(apChannelName, aSnapshotTarget);
    this->Start();
}

ScopeTimer::ScopeTimer(const char *apChannelName,
                       int aGridSize,
                       int aArrays,
                       bool aSinglePrecision,
                       int aOperations,
                       SnapshotTarget aSnapshotTarget) {
    this->mElasticTimer = new ElasticTimer(apChannelName,
                                           aGridSize,
                                           aArrays,
                                           aSinglePrecision,
                                           aOperations,
                                           aSnapshotTarget);
    this->Start();
}

ScopeTimer::~ScopeTimer() {
    this->mElasticTimer->Stop();
    delete this->mElasticTimer;
}

int
ScopeTimer::Start() {
    return this->mElasticTimer->Start();
}

int
ScopeTimer::Stop() {
    return this->mElasticTimer->Stop();
}

void
ScopeTimer::FlushSnapshot() {
    this->mElasticTimer->FlushSnapshot();
}

bool
ScopeTimer::IsActive() const {
    return this->mElasticTimer->IsActive();
}