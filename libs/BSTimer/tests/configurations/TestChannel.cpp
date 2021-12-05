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

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/timer/configurations/TimerChannel.hpp>
#include <bs/timer/configurations/TimerManager.hpp>
#include <bs/timer/core/timers/concrete/ElasticTimer.hpp>

using namespace bs::timer;
using namespace bs::timer::configurations;


TEST_CASE("TimerChannel - Class", "[Configuration]") {
    /* Pre-cleanup. */

    TimerManager::Kill();

    /* Channel test. */
    TimerChannel channel("FirstChannel", 1000, 1000, 9);
    TimerChannel::Pointer channel_pointer = std::make_shared<TimerChannel>(channel);
    configurations::TimerManager::GetInstance()->RegisterChannel(channel_pointer);
    REQUIRE(channel.GetName() == "FirstChannel");

    /* Timer test. */
    ElasticTimer timer(channel_pointer);
    REQUIRE(channel_pointer->GetTimers().size() == 1);
    REQUIRE(channel_pointer->GetTimers()[0] == &timer);

    /* Cleanup. */

    TimerManager::GetInstance()->Terminate(true);
}
