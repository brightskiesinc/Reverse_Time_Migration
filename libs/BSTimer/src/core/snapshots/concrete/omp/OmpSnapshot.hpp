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

#ifndef BS_TIMER_CORE_SNAPSHOT_OMP_SNAPSHOT_HPP
#define BS_TIMER_CORE_SNAPSHOT_OMP_SNAPSHOT_HPP

#include <omp.h>

#include <bs/timer/core/snapshots/interface/Snapshot.hpp>


namespace bs {
    namespace timer {
        namespace core {
            namespace snapshots {

                /**
                 * @brief The snapshot class for timers timing OpenMp backends.
                 *
                 */
                class OmpSnapshot : public Snapshot {
                public:
                    explicit OmpSnapshot(SnapshotTarget aSnapshotTarget = SnapshotTarget::HOST);

                    ~OmpSnapshot() override;

                    double
                    Start() override;

                    double
                    End() override;

                    double
                    Resolve() override;

                private:
                    /// Starting time.
                    double mStart;
                    /// End time.
                    double mEnd;
                };
            }//namespace snapshot
        }//namespace core
    }//namespace timer
}//namespace bs

#endif // BS_TIMER_CORE_SNAPSHOT_OMP_SNAPSHOT_HPP
