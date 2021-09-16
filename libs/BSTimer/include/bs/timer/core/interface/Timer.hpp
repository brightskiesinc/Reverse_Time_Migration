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

#ifndef BS_TIMER_CORE_TIMER_HPP
#define BS_TIMER_CORE_TIMER_HPP


namespace bs {
    namespace timer {


        /**
         * @brief Traditional execution timer. Takes a channel or channel name to get
         * constructed. Supports starting ending and pausing the running timer.
         */
        class Timer {
        public:
            /**
             * @brief Destructor.
             */
            virtual ~Timer() = default;

            /**
             * @brief Starts the timer for the corresponding channel accompanied with this timer.
             * @return Status flag.
             */
            virtual int
            Start() = 0;

            /**
             * @brief Ends the timer for the corresponding channel accompanied with this timer.
             * @return Status flag.
             */
            virtual int
            Stop() = 0;

            /**
             * @brief
             * Flushes data object of this timer object to the accompanied TimerChannel object.
             */
            virtual void
            FlushSnapshot() = 0;

            /**
             *
             * @return
             * Flag to indicate if this timer object is currently active.
             */
            virtual bool
            IsActive() const = 0;
        };

    }//namespace timer
}//namespace bs

#endif // BS_TIMER_CORE_TIMER_HPP
