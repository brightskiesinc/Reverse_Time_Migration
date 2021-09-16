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

#include <bs/timer/reporter/TimerReporter.hpp>

#include <bs/timer/core/concrete/ElasticTimer.hpp>
#include <bs/timer/utils/stats/StatisticsHelper.hpp>
#include <bs/timer/test-utils/FunctionsGenerator.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <prerequisites/libraries/catch/catch.hpp>

#include <iostream>

#define TOLERANCE 0.00001 /* Based on trial and error */

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::reporter;
using namespace bs::timer::utils::stats;
using namespace bs::timer::testutils;
using namespace bs::base::configurations;

TEST_CASE("Reporter - Class", "[reporter]") {
    /* Pre-cleanup. */

    TimerManager::Kill();

    /* Meta-data. */

    nlohmann::json map;
    map[BS_TIMER_K_TIME_UNIT] = "sec";
    auto configuration_map = new JSONConfigurationMap(map);
    TimerManager::GetInstance()->Configure(configuration_map);
    double grid_size = 1000 * 1000;
    double data_size = grid_size * sizeof(float);
    int operations = 4 * 6 + 4; /* Floating point operations in sqrt() = 6*/

    /* Timers' instantiation. */

    TimerChannel channel("First Kernel", grid_size, data_size, operations);
    TimerChannel::Pointer channel_pointer = std::make_shared<TimerChannel>(channel);
    TimerManager::GetInstance()->RegisterChannel(channel_pointer);

    ElasticTimer timer1(channel_pointer);
    ElasticTimer timer2(channel_pointer);

    /* Runtime storage vector. */

    vector<double> runtimes;

    /* Timer object test. */

    timer1.Start();
    timer1.Stop();
    runtimes.push_back(0);

    /* Chrono timer ground truth test. */

    auto start = chrono::high_resolution_clock::now();
    timer2.Start();
    target_technology_test_function();
    auto end = chrono::high_resolution_clock::now();
    timer2.Stop();
    chrono::duration<double> elapsed_seconds = end - start;
    runtimes.push_back(elapsed_seconds.count());

    /* Results. */

    auto average = StatisticsHelper::GetAverage(runtimes);
    auto total = StatisticsHelper::GetTotal(runtimes);

    /* Reporter test. */

    TimerReporter r;
    r.Resolve();

    REQUIRE(r.GetMap("First Kernel")[BS_TIMER_K_MIN_RUNTIME]
            == Approx(0).margin(TOLERANCE));
    REQUIRE(r.GetMap("First Kernel")[BS_TIMER_K_MAX_RUNTIME]
            == Approx(elapsed_seconds.count()).margin(TOLERANCE));
    REQUIRE(r.GetMap("First Kernel")[BS_TIMER_K_AVERAGE_RUNTIME]
            == Approx(average).margin(TOLERANCE));
    REQUIRE(r.GetMap("First Kernel")[BS_TIMER_K_TOTAL]
            == Approx(total).margin(TOLERANCE));
    REQUIRE(r.GetStats()["First Kernel"].GetNumberOfCalls()
            == 2);

    /* Cleanup. */

    TimerManager::GetInstance()->Terminate(true);
}
