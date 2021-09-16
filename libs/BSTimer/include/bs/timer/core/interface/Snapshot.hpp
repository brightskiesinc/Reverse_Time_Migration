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

#ifndef BS_TIMER_SNAPSHOT_HPP
#define BS_TIMER_SNAPSHOT_HPP

namespace bs {
    namespace timer {
        namespace core {
            namespace snapshot {

                enum SnapshotTarget {
                    HOST,
                    DEVICE
                };

                /**
                 * @brief Snapshot class used to hold start and end points of execution timer, until time of resolving and runtime
                 * calculation.
                 *
                 */
                class Snapshot {
                public:
                    /**
                     * @brief Default destructor.
                     */
                    virtual ~Snapshot() = default;

                    /**
                     * @brief Updates start state of snapshot at start time of execution timer.
                     *
                     */
                    virtual double Start() = 0;

                    /**
                     * @brief Updates end state of snapshot at end time of execution timer.
                     *
                     */
                    virtual double End() = 0;

                    /**
                     * @brief Calculates runtime using start and end points.
                     * @return Runtime
                     */
                    virtual double Resolve() = 0;
                };

            }//namespace snapshot
        }//namespace core
    }//namespace timer
}//namespace bs



#endif // BS_TIMER_SNAPSHOT_HPP
