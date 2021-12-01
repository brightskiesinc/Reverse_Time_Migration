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

#include <bs/base/common/ExitCodes.hpp>

#include <bs/timer/core/timers/concrete/ElasticTimer.hpp>
#include <bs/timer/core/snapshots/helpers/GenericSnapshot.hpp>
#include <bs/timer/configurations/TimerManager.hpp>

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::core::snapshots;


ElasticTimer::ElasticTimer(const TimerChannel::Pointer &apChannel, SnapshotTarget aSnapshotTarget) {
    TimerManager::GetInstance()->RegisterChannel(make_shared<TimerChannel>(*new TimerChannel(apChannel->GetName())));
    this->mpSnapshot = new GenericSnapshot(aSnapshotTarget);
    this->mpChannel = apChannel;
    apChannel.get()->AddTimer(this);
    this->mIsActive = false;
}

ElasticTimer::ElasticTimer(const char *apChannelName, SnapshotTarget aSnapshotTarget) {
    TimerManager::GetInstance()->RegisterChannel(make_shared<TimerChannel>(*new TimerChannel(apChannelName)));
    this->mpSnapshot = new GenericSnapshot(aSnapshotTarget);
    this->mpChannel = configurations::TimerManager::GetInstance()->Get(apChannelName);
    configurations::TimerManager::GetInstance()->Get(apChannelName).get()->AddTimer(this);
    this->mIsActive = false;
}

ElasticTimer::ElasticTimer(const char *apChannelName,
                           int aGridSize,
                           int aArrays,
                           bool aSinglePrecision,
                           int aOperations,
                           SnapshotTarget aSnapshotTarget) {
    int data_size;
    if (aSinglePrecision) {
        data_size = aArrays * aGridSize * 4;
    } else {
        data_size = aArrays * aGridSize * 8;
    }
    TimerManager::GetInstance()->RegisterChannel(
            make_shared<TimerChannel>(*new TimerChannel(apChannelName, aGridSize, data_size, aOperations)));
    this->mpSnapshot = new GenericSnapshot(aSnapshotTarget);
    this->mpChannel = configurations::TimerManager::GetInstance()->Get(apChannelName);
    configurations::TimerManager::GetInstance()->Get(apChannelName).get()->AddTimer(this);
    this->mIsActive = false;
}

ElasticTimer::~ElasticTimer() {
    this->mIsActive = false;
    this->mpChannel->RemoveTimer(this);
    this->mpSnapshot = nullptr;
}

int
ElasticTimer::Start() {
    int rc = BS_BASE_RC_FAILURE;
    if (!this->IsActive()) {
        this->mIsActive = true;
        this->mpSnapshot->Start();
        rc = BS_BASE_RC_SUCCESS;
    }
    return rc;
}

int
ElasticTimer::Stop() {
    int rc = BS_BASE_RC_FAILURE;
    if (this->IsActive()) {
        this->mpSnapshot->End();
        this->FlushSnapshot();
        this->mIsActive = false;
        rc = BS_BASE_RC_SUCCESS;
    }
    return rc;
}

void
ElasticTimer::FlushSnapshot() {
    this->mpChannel->AddSnapshot(this->mpSnapshot);
}

bool
ElasticTimer::IsActive() const {
    return mIsActive;
}
