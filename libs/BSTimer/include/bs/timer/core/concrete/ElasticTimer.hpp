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

#ifndef BS_TIMER_CORE_ELASTIC_TIMER_HPP
#define BS_TIMER_CORE_ELASTIC_TIMER_HPP

#include <bs/timer/core/interface/Timer.hpp>
#include <bs/timer/configurations/TimerChannel.hpp>
#include <bs/timer/core/interface/Snapshot.hpp>


namespace bs {
    namespace timer {

        /**
         * @brief Traditional execution timer. Takes a channel or channel name to get
         * constructed. Supports starting ending and pausing the running timer.
         */
        class ElasticTimer : public Timer {
        public:
            /**
             * @brief Constructor that takes a channel object to construct a timer.
             */
            explicit
            ElasticTimer(const TimerChannel::Pointer &apChannel,
                         core::snapshot::SnapshotTarget aSnapshotTarget = core::snapshot::SnapshotTarget::HOST);

            /**
             * @brief Constructor that takes a channel name to construct a timer.
             */
            explicit
            ElasticTimer(const char *apChannelName,
                         core::snapshot::SnapshotTarget aSnapshotTarget = core::snapshot::SnapshotTarget::HOST);

            ElasticTimer(const char *apChannelName,
                         int aGridSize,
                         int aArrays,
                         bool aSinglePrecision,
                         int aOperations,
                         core::snapshot::SnapshotTarget aSnapshotTarget = core::snapshot::SnapshotTarget::HOST);

            /**
             * @brief Destructor.
             */
            ~ElasticTimer() override;

            /**
             * @brief Starts the timer for the corresponding channel accompanied with this timer.
             * @return Status flag.
             */
            int
            Start() override;

            /**
             * @brief Ends the timer for the corresponding channel accompanied with this timer.
             * @return Status flag.
             */
            int
            Stop() override;

            /**
             * @brief
             * Flushes data object of this timer object to the accompanied TimerChannel object.
             */
            void
            FlushSnapshot() override;

            /**
             *
             * @return
             * Flag to indicate if this timer object is currently active.
             */
            bool
            IsActive() const override;

        private:
            /// The corresponding channel accompanied with this timer.
            TimerChannel::Pointer mpChannel;
            /// Activation flag
            bool mIsActive;
            ///Snapshot handler object.
            core::snapshot::Snapshot *mSnapshot;
        };
    }//namespace timer
}//namespace bs


#endif // BS_TIMER_CORE_ELASTIC_TIMER_HPP
