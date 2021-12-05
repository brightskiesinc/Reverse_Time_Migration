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

#ifndef BS_TIMER_CONFIGURATIONS_MANGER_HPP
#define BS_TIMER_CONFIGURATIONS_MANGER_HPP

#include <bs/base//common/Singleton.tpp>
#include <bs/base/configurations/interface/ConfigurationMap.hpp>
#include <bs/base/configurations/interface/Configurable.hpp>

#include <bs/timer/configurations/TimerChannel.hpp>
#include <bs/timer/core/timers/interface/Timer.hpp>
#include <bs/timer/reporter/TimerReporter.hpp>

namespace bs {
    namespace timer {
        namespace configurations {
            /**
            * @brief
            * The singleton class that manage the registered channels and outputs
            * <br>
            * The TimerManager keeps a map of all the named TimerChannel objects
            * and share them on demand by new Timer objects created with the same name.
            * <br>
            * Thus the TimerManager is able to change the Severity Level of selected TimerChannel object,
            * impacting all the Timer objects using it.
            * <br>
            * The TimerManager also keeps a list of all configured output object to output the Timer objects.
            */

            class TimerManager : public bs::base::common::Singleton<TimerManager>,
                                 public bs::base::configurations::Configurable {

            public:
                friend class bs::base::common::Singleton<TimerManager>;

            public:
                /**
                 * @brief Setting the configuration to manipulate all configurable objects.
                 * @param apConfigurationList
                 *
                 */
                void
                Configure(bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Terminates all active timers.
                 * @param aCleanup
                 * Boolean to indicate whether cleanup is to be performed after termination.
                 */
                void
                Terminate(bool aCleanup = false);

                /**
                 *
                 * @param apChannelName
                 * @return Pointer to channel object with the input name.
                 *
                 */
                TimerChannel::Pointer
                Get(const char *apChannelName);

                /**
                 * @brief
                 * Free all channel objects kept by manager.
                 */
                void
                Cleanup();

                /**
                 *
                 * @return
                 * A list of all active timers registered to all channel objects kept by manager.
                 */
                std::vector<Timer *>
                GetActiveTimers();

                /**
                 * @brief Reporting using a collection of output streams.
                 * @param aStreams
                 * @param aChannelName
                 * @return Status flag.
                 */
                int
                Report(const std::vector<std::ostream *> &aStreams, const std::string &aChannelName = " ");

                /**
                 * @brief Uses reporter module to generate report for a given channel, or all
                 * managed channels(default), and flushes report using given output stream.
                 * @param aOutputStream
                 * @param aChannelName
                 * @return Status flag.
                 */
                std::string
                Report(std::ostream &aOutputStream, const std::string &aChannelName = " ");


                TimerChannel::Map GetMap();


                void RegisterChannel(const TimerChannel::Pointer &apChannel);

                /**
                 * @brief Getter for time precision.
                 *
                 */
                double GetPrecision();

            private:
                /**
                 * @brief Default constructor.
                 * @note Private constructor for Singleton purposes.
                 */
                TimerManager() = default;

                /**
                 * @brief Acquires timer configuration from configuration map.
                 */
                void
                AcquireConfiguration() override;

            private:
                /// Map of all TimerChannel objects.
                TimerChannel::Map mChannelMap;
                /// Configuration map.
                bs::base::configurations::ConfigurationMap *mpConfigurationMap;
                /// Time precision.
                double mTimePrecision = 1;
            };

        } //namespace configurations
    } //namespace timer
}//namespace bs



#endif // BS_TIMER_CONFIGURATIONS_MANGER_HPP