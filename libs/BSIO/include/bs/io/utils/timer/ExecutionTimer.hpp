/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_IO_UTILS_TIMER_EXECUTION_TIMER_HPP
#define BS_IO_UTILS_TIMER_EXECUTION_TIMER_HPP

#include <chrono>
#include <iostream>

namespace bs {
    namespace io {
        namespace utils {
            namespace timer {

                /**
                 * @brief Function execution timer.
                 */
                class ExecutionTimer {
                public:
                    /**
                     * @brief Function execution timer. Takes a block of code as parameter and evaluate
                     * it's run time. Takes a flag that determines whether to print put the execution
                     * time or not.
                     *
                     * @note Time of execution is return as microseconds.
                     */
                    template<typename T>
                    static long
                    Evaluate(T f, bool aShowExecutionTime = false) {
                        auto start = std::chrono::high_resolution_clock::now();
                        f();
                        auto stop = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                        auto count = duration.count();
                        if (aShowExecutionTime) {
                            ExecutionTimer::ShowResult(count);
                        }
                        return count;
                    }

                    /**
                     * @brief Takes execution time in micro seconds and shows it in seconds,
                     */
                    static int
                    ShowResult(long aTime) {
                        return printf("Execution Time: %.4f SEC\n", aTime / (1.0 * 1e6));
                    }
                };
            }
        }
    }
}

#endif //BS_IO_UTILS_TIMER_EXECUTION_TIMER_HPP
