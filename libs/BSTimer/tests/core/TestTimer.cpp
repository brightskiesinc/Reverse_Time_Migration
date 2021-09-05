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

#include <bs/timer/core/concrete/ElasticTimer.hpp>

#include <bs/timer/configurations/TimerManager.hpp>
#include <bs/base/common/ExitCodes.hpp>
#include <bs/timer/test-utils/FunctionsGenerator.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

#include <chrono>

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::testutils;


TEST_CASE("Timer - Class", "[Core]") {
    /* Pre-cleanup. */

    TimerManager::Kill();

    /* Meta-data. */

    int grid_size = 1000 * 1000;
    int data_size = grid_size * sizeof(float);
    int operations = 4 * 6 + 4; /* Floating point operations in sqrt() = 6*/
    TimerChannel channel("First Kernel", grid_size, data_size, operations);

    TimerChannel::Pointer channel_pointer = std::make_shared<TimerChannel>(channel);
    TimerManager::GetInstance()->RegisterChannel(channel_pointer);

    ElasticTimer timer("First Kernel", grid_size, 1, true, operations);
    REQUIRE(timer.Start() == BS_BASE_RC_SUCCESS);
    REQUIRE(timer.Start() == BS_BASE_RC_FAILURE);
    REQUIRE(timer.IsActive() == true);

    timer.Start();
    timer.Stop();
    REQUIRE(timer.IsActive() == false);

    auto start = chrono::high_resolution_clock::now();
    timer.Start();
    target_technology_test_function();
    auto end = chrono::high_resolution_clock::now();
    timer.Stop();
    chrono::duration<double> elapsed_seconds = end - start;
    channel_pointer->GetChannelStats().Resolve();
    REQUIRE(channel_pointer->GetChannelStats().GetRuntimes()[0] == Approx(0).margin(0.1));
    REQUIRE(channel_pointer->GetChannelStats().GetRuntimes()[1] == Approx(elapsed_seconds.count()).margin(0.00001));

    /* Cleanup. */

    TimerManager::GetInstance()->Terminate(true);
}
