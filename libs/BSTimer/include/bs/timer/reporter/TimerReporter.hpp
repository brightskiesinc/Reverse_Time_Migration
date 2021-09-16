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

#ifndef BS_TIMER_REPORTER_REPORTER_HPP
#define BS_TIMER_REPORTER_REPORTER_HPP

#include <bs/timer/configurations/TimerChannel.hpp>
#include <bs/timer/configurations/TimerManager.hpp>
#include <bs/timer/utils/stats/StatisticsHelper.hpp>

#include <string>

namespace bs {
    namespace timer {
        namespace reporter {

            /**
             * @brief Reporter interface to be implemented by all reporters.
             */
            class TimerReporter {
            public:
                /**
                 * @brief
                 *  Constructor.
                 */
                TimerReporter();

                /**
                 * @brief Default destructor.
                 */
                ~TimerReporter() = default;

                /**
                 * @brief
                 * Generates a report stream for certain channel.
                 * @param aChannelName
                 * @return String generated.
                 *
                 */
                std::string
                GenerateStream(std::ostream &aOutputStream, const std::string &aChannelName = " ");

                /**
                 * @brief Flushes report for all channels to be used by Python loader.
                 * @return
                 */
                int
                FlushReport(const std::string &aFilePath);

                /**
                 *
                 * @param aChannelName
                 * @return Report map of given channel.
                 *
                 */
                std::map<std::string, double>
                GetMap(const std::string &aChannelName);

                std::map<std::string, dataunits::ChannelStats>
                GetStats();

                /**
                 * Resolves all snapshots for reporting.
                 */
                void
                Resolve();

            private:
                static int
                HandleFilePath(const std::string &aFilePath);

            private:
                ///Map of channels and their accompanied stats.
                std::map<std::string, dataunits::ChannelStats> mDataMap;
            };
        } //namespace reporter
    }//namespace timer
}//namespace bs

#endif // BS_TIMER_REPORTER_REPORTER_HPP