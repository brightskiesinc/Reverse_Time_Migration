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

#ifndef BS_TIMER_CORE_LAZY_TIMER
#define BS_TIMER_CORE_LAZY_TIMER

#include <bs/timer/common/Definitions.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>

namespace bs {
    namespace timer {
        namespace core {

            /**
             * @brief Function execution timer.
             */
            class LazyTimer {
            public:
                /**
                 * @brief Function execution timer. Takes a block of code as parameter and evaluate
                 * it's run time. Takes a flag that determines whether to print put the execution
                 * time or not.
                 */
                template<typename T>
                static double
                Evaluate(T f, bool aShowExecutionTime = false, double aTimeUnit = BS_TIMER_TU_MICRO) {
                    auto start = std::chrono::high_resolution_clock::now();
                    f();
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> duration = end - start;
                    double count = duration.count();
                    if (aShowExecutionTime) {
                        std::cout << std::left << std::setfill(' ') << std::setw(20) << "Execution Time: " << ": ";
                        std::cout << std::left << std::setfill(' ') << std::setw(20) << count << std::endl;
                    }
                    return count;
                }
            };
        }//namespace core
    }//namespace timer
}//namespace bs



#endif // BS_TIMER_CORE_LAZY_TIMER
