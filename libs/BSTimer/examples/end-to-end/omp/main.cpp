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

#include <fstream>
#include <iostream>
#include <sys/stat.h>

#define SIZE 1000

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::reporter;

void
test_function();

int main() {
    mkdir(BS_TIMER_WRITE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    int grid_size = SIZE * SIZE;
    int operations = 4 * 6 + 4; /* Floating point operations in sqrt() = 6 */

    float a1[SIZE][SIZE];
    for (int t = 0; t < 80; t++) {
        ScopeTimer st("A_Kernel_OMP", grid_size, 1, true, operations);
        test_function();
    }

    float a2[SIZE][SIZE];
    for (int t = 0; t < 80; t++) {
        ScopeTimer st("B_Kernel_OMP");
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
#pragma omp parallel for
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            array[i][j] = sqrt(i) * sqrt(j) * sqrt(i) * sqrt(j);
        }
    }
}
