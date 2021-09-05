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


#include <bs/timer/configurations/TimerManager.hpp>
#include <bs/timer/configurations/TimerChannel.hpp>
#include <bs/timer/core/concrete/ElasticTimer.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace bs::timer;
using namespace bs::timer::configurations;


TEST_CASE("TimerManager - Class", "[Configuration]") {
    /* Pre-cleanup. */

    TimerManager::Kill();

    TimerChannel channel("FirstChannel", 1000, 1000, 9);
    TimerChannel::Pointer channel_pointer = std::make_shared<TimerChannel>(channel);
    TimerManager::GetInstance()->RegisterChannel(channel_pointer);

    ElasticTimer timer1(channel_pointer);
    ElasticTimer timer2(channel_pointer);

    timer1.Start();
    timer2.Start();
    REQUIRE(TimerManager::GetInstance()->Get("FirstChannel") == channel_pointer);
    REQUIRE(TimerManager::GetInstance()->GetActiveTimers().size() == 2);

    timer2.Stop();
    REQUIRE(TimerManager::GetInstance()->GetActiveTimers().size() == 1);

    TimerManager::GetInstance()->Terminate(true);
    REQUIRE(TimerManager::GetInstance()->GetActiveTimers().empty());
    REQUIRE(TimerManager::GetInstance()->Get("FirstChannel") == nullptr);

    /* Cleanup. */

    TimerManager::GetInstance()->Terminate(true);
}
