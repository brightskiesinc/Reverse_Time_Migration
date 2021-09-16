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

#include "iostream"
#include <bs/timer/core/concrete/ScopeTimer.hpp>

#include <bs/timer/configurations/TimerManager.hpp>

#include <bs/base/common/ExitCodes.hpp>

#include "snapshot/concrete/serial/SerialSnapshot.hpp"
#include "snapshot/concrete/omp/OmpSnapshot.hpp"

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::core::snapshot;


ScopeTimer::ScopeTimer(const TimerChannel::Pointer &apChannel, SnapshotTarget aSnapshotTarget)
        : mIsActive(true) {
    TimerManager::GetInstance()->RegisterChannel(make_shared<TimerChannel>(*new TimerChannel(apChannel->GetName())));

#ifdef USE_OMP
    this->mSnapshot = new ::OmpSnapshot(aSnapshotTarget);
#else
    this->mSnapshot = new ::SerialSnapshot(aSnapshotTarget);
#endif
    this->mpChannel = apChannel;
    apChannel.get()->AddTimer(this);
    this->Start();
}

ScopeTimer::ScopeTimer(const char *apChannelName, SnapshotTarget aSnapshotTarget)
        : mIsActive(true) {
    TimerManager::GetInstance()->RegisterChannel(make_shared<TimerChannel>(*new TimerChannel(apChannelName)));

#ifdef USE_OMP
    this->mSnapshot = new ::OmpSnapshot(aSnapshotTarget);
#else
    this->mSnapshot = new ::SerialSnapshot(aSnapshotTarget);
#endif

    this->mpChannel = TimerManager::GetInstance()->Get(apChannelName);
    TimerManager::GetInstance()->Get(apChannelName).get()->AddTimer(this);
    this->Start();
}

ScopeTimer::ScopeTimer(const char *apChannelName,
                       int aGridSize,
                       int aArrays,
                       bool aSinglePrecision,
                       int aOperations,
                       SnapshotTarget aSnapshotTarget) : mIsActive(true) {
    int data_size;
    if (aSinglePrecision) {
        data_size = aArrays * aGridSize * 4.0f;
    } else {
        data_size = aArrays * aGridSize * 8.0f;
    }
    TimerManager::GetInstance()->RegisterChannel(
            make_shared<TimerChannel>(*new TimerChannel(apChannelName, aGridSize, data_size, aOperations)));
#ifdef USE_OMP
    this->mSnapshot = new OmpSnapshot(aSnapshotTarget);
#else
    this->mSnapshot = new SerialSnapshot(aSnapshotTarget);
#endif

    this->mpChannel = TimerManager::GetInstance()->Get(apChannelName);
    TimerManager::GetInstance()->Get(apChannelName).get()->AddTimer(this);
    this->Start();
}

ScopeTimer::~ScopeTimer() {
    this->mIsActive = false;
    this->Stop();
    this->mpChannel->RemoveTimer(this);
    this->mSnapshot = nullptr;
}

int
ScopeTimer::Start() {
    this->mSnapshot->Start();
    return BS_BASE_RC_SUCCESS;
}

int
ScopeTimer::Stop() {
    this->mSnapshot->End();
    this->FlushSnapshot();
    return BS_BASE_RC_SUCCESS;
}

void
ScopeTimer::FlushSnapshot() {
    this->mpChannel->AddSnapshot(this->mSnapshot);
}

bool
ScopeTimer::IsActive() const {
    return mIsActive;
}