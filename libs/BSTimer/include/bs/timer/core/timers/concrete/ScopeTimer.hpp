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

#ifndef BS_TIMER_CORE_SCOPE_TIMER_HPP
#define BS_TIMER_CORE_SCOPE_TIMER_HPP

#include <bs/timer/core/timers/concrete/ElasticTimer.hpp>

namespace bs {
    namespace timer {
        /**
         * @brief Traditional execution timer. Takes a channel or channel name to get
         * constructed. Supports starting ending and pausing the running timer.
         */
        class ScopeTimer : public Timer {
        public:
            /**
             * @brief Constructor that takes a channel object to construct a timer.
             * @param apChannel
             * @param aSnapshotTarget
             */
            explicit
            ScopeTimer(const TimerChannel::Pointer &apChannel,
                       core::snapshots::SnapshotTarget aSnapshotTarget = core::snapshots::SnapshotTarget::HOST);

            /**
             * @brief Constructor that takes a channel name to construct a timer.
             * @param apChannelName
             * @param aSnapshotTarget
             */
            explicit
            ScopeTimer(const char *apChannelName,
                       core::snapshots::SnapshotTarget aSnapshotTarget = core::snapshots::SnapshotTarget::HOST);

            /**
             * @brief Constructor that takes a channel name and kernel data to construct a timer.
             * @param apChannelName
             * @param aGridSize
             * @param aArrays
             * @param aSinglePrecision
             * @param aOperations
             * @param aSnapshotTarget
             */
            ScopeTimer(const char *apChannelName,
                       int aGridSize,
                       int aArrays,
                       bool aSinglePrecision,
                       int aOperations,
                       core::snapshots::SnapshotTarget aSnapshotTarget = core::snapshots::SnapshotTarget::HOST);

            /**
             * @brief Destructor.
             */
            ~ScopeTimer() override;

        private:
            int
            Start() override;

            int
            Stop() override;

            void
            FlushSnapshot() override;

            bool
            IsActive() const override;

        private:
            /// Elastic timer instance for internal usage.
            ElasticTimer *mElasticTimer;
        };

    }//namespace timer

}

#endif // BS_TIMER_CORE_SCOPE_TIMER_HPP
