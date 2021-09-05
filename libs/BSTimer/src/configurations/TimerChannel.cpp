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

#include <bs/timer/configurations/TimerChannel.hpp>

using namespace bs::timer;
using namespace bs::timer::dataunits;


TimerChannel::~TimerChannel() {
    for (auto timer : this->mTimers) {
        if (timer) {
            timer = nullptr;
        }
    }
}

void
TimerChannel::AddTimer(Timer *aTimer) {
    this->mTimers.push_back(aTimer);
}

void
TimerChannel::AddSnapshot(core::snapshot::Snapshot *apSnapshot) {
    this->mChannelStats.AddSnapshot(apSnapshot);
}

ChannelStats &
TimerChannel::GetChannelStats() {
    return this->mChannelStats;
}

void
TimerChannel::RemoveTimer(Timer *apTimer) {
    auto it = this->mTimers.begin();
    for (; it != this->mTimers.end();) {
        if (*it == apTimer) {
            it = this->mTimers.erase(it);
        } else {
            ++it;
        }
    }
}
