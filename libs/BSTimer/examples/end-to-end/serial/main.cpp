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

#include <bs/timer/api/cpp/BSTimer.hpp>
#include <bs/timer/common/Definitions.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <fstream>
#include <iostream>
#include <sys/stat.h>

#define SIZE 1000

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::reporter;
using namespace bs::base::configurations;


void
test_function();


int main() {
    mkdir(BS_TIMER_WRITE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    nlohmann::json map;
    map[BS_TIMER_K_PROPERTIES][BS_TIMER_K_TIME_UNIT] = 1e-3;
    auto configuration_map = new JSONConfigurationMap(map);
    TimerManager::GetInstance()->Configure(configuration_map);
    int grid_size = SIZE * SIZE;
    int operations = 4 * 6 + 4; /* Floating point operations in sqrt() = 6*/

    cout << "Timing using lazy timer: " << endl;
    core::LazyTimer::Evaluate([&]() {
        test_function();
    }, true);

    for (int i = 0; i < 80; i++) {
        ScopeTimer t("A_Kernel_Serial", grid_size, 1, true, operations);
        test_function();
    }

    for (int i = 0; i < 10; i++) {
        ScopeTimer st("B_Kernel_Serial");
        test_function();
    }

    std::vector<std::ostream *> streams;
    streams.emplace_back(&std::cout);
    std::ofstream ofs;
    ofs.open(BS_TIMER_WRITE_PATH
             "/timing_results.txt");
    streams.emplace_back(&ofs);
    TimerManager::GetInstance()->Report(streams);
    ofs.close();

    TimerManager::GetInstance()->Terminate(true);

    return EXIT_SUCCESS;
}

inline void
test_function() {
    float array[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            array[i][j] = sqrt(i) * sqrt(j) * sqrt(i) * sqrt(j);
        }
    }
}
