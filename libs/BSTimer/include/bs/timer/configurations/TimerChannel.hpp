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

#ifndef BS_TIMER_CORE_CHANNEL_HPP
#define BS_TIMER_CORE_CHANNEL_HPP

#include <bs/timer/core/interface/Timer.hpp>
#include <bs/timer/data-units/ChannelStats.hpp>
#include <bs/timer/core/interface/Snapshot.hpp>

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <memory>

namespace bs {
    namespace timer {

        /**
         * @brief   TimerChannel shared by Timer objects using the same name
         * <br>
         * A TimerChannel is the underlying object used by one or many Timer objects to
         * associate a named prefix.
         * <br>
         */
        class Timer;

        class TimerChannel {
        public:
            typedef std::shared_ptr<TimerChannel> Pointer;
            typedef std::map<const std::string, TimerChannel::Pointer> Map;

        public:
            /**
             * @brief Default constructor.
             */
            TimerChannel() = default;

            /**
             * @brief Constructor that takes a name for this TimerChannel.
             */
            explicit TimerChannel(std::string apName,
                                  int aGridSize = -1,
                                  int aDataSize = -1,
                                  int aFLOPS = -1)
                    : mName(std::move(apName)) {
                this->mChannelStats.SetDataSize(aDataSize);
                this->mChannelStats.SetGridSize(aGridSize);
                this->mChannelStats.SetFLOPS(aFLOPS);
            }

            /**
             * Destructor.
             */
            ~TimerChannel();

            /**
             * @return TimerChannel name.
             */
            inline std::string
            GetName() const { return this->mName; }

            /**
             * @return Vector of pointers to Timer objects registered to this channel.
             */
            std::vector<Timer *>
            GetTimers() const { return this->mTimers; }

            /**
             * @brief
             * Registers a timer object to this channel.
             *
             * @param apTimer
             * Pointer to Timer object to add.
             */
            void
            AddTimer(Timer *apTimer);

            /**
             * @brief Removes a timer object from channel.
             * @param apTimer: Pointer to timer object to be removed.
             */
            void
            RemoveTimer(Timer *apTimer);

            /**
             *
             * @param apSnapshot
             * Pointer to Snapshot accompanied to this channel to be added.
             */
            void
            AddSnapshot(core::snapshot::Snapshot *apSnapshot);

            /**
             * @brief Returns a ChannelStats object holding all data regarding this channel.
             * @return ChannelStats
             */
            dataunits::ChannelStats &
            GetChannelStats();

        private:
            /// TimerChannel name.
            const std::string mName;
            /// Timer objects registered to this channel.
            std::vector<Timer *> mTimers;
            /// Data objects accompanied with this channel.
            dataunits::ChannelStats mChannelStats;
        };
    }//namespace timer
}//namespace bs

#endif // BS_TIMER_CORE_CHANNEL_HPP
