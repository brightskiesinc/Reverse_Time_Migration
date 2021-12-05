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

#include <chrono>
#include <iostream>
#include <iomanip>

#include <bs/timer/core/snapshots/helpers/GenericSnapshot.hpp>
#include <bs/timer/reporter/TimerReporter.hpp>
#include <bs/timer/common/Definitions.hpp>

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
                Evaluate(T f,
                         bool aShowExecutionTime = false,
                         double aPrecision = BS_TIMER_TU_MICRO,
                         core::snapshots::SnapshotTarget aSnapshotTarget = core::snapshots::SnapshotTarget::HOST) {
                    core::snapshots::GenericSnapshot s(aSnapshotTarget);
                    s.Start();
                    f();
                    s.End();
                    auto duration = s.Resolve();
                    if (aShowExecutionTime) {
                        std::stringstream os;
                        os.precision(5);
                        os << std::left << std::setfill(' ') << std::setw(20) << "Lazy Timer Runtime" << ": "
                           << std::left << std::setfill(' ') << std::setw(12)
                           << std::scientific << duration / aPrecision
                           << reporter::TimerReporter::PrecisionToUnit(aPrecision)
                           << std::endl;
                        std::cout << os.str();
                    }
                    return duration;
                }
            };
        }//namespace core
    }//namespace timer
}//namespace bs



#endif // BS_TIMER_CORE_LAZY_TIMER
