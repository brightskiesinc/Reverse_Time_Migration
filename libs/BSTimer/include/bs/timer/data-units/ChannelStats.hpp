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

#ifndef BS_TIMER_DATA_UNITS_CHANNEL_STATS_HPP
#define BS_TIMER_DATA_UNITS_CHANNEL_STATS_HPP

#include <bs/timer/utils/stats/StatisticsHelper.hpp>
#include <bs/timer/common/Definitions.hpp>
#include <bs/timer/core/interface/Snapshot.hpp>

#include <map>

namespace bs {
    namespace timer {
        namespace dataunits {
            /**
             * @brief Class to hold all data and statistics accompanied with a channel.
             *
             */
            class ChannelStats {

            public:
                ChannelStats() :
                        mGridSize(-1),
                        mDataSize(-1),
                        mFLOPS(-1),
                        mNumberOfCalls(0) {}

                ~ChannelStats();

                /**
                 * @brief Returns a map of all statistics of the channel object.
                 *
                 */
                std::map<std::string, double> GetMap();

                /**
                 * @brief Calculates maximum runtime.
                 *
                 */
                double
                GetMaxRuntime();

                /**
                 * @brief Calculates minimum runtime.
                 *
                 */
                double
                GetMinRuntime();

                /**
                 * @brief Calculates average runtime.
                 *
                 */
                double
                GetAverageRuntime();

                /**
                 * @brief Calculates variance in runtimes.
                 *
                 */
                double
                GetVariance();

                /**
                 * @brief Calculates deviation in runtimes.
                 *
                 */
                double
                GetDeviation();

                /**
                 * @brief Calculates total runtime.
                 *
                 */
                double
                GetTotal();

                /**
                 * @brief Adds a runtime value to the vector of runtimes accompanied to channel.
                 * @param aRuntime
                 */
                void
                AddRuntime(double
                           aRuntime);

                /**
                 *
                 * @return Vector of runtimes.
                 */
                std::vector<double> GetRuntimes();

                /**
                 *
                 * @return Vector of bandwidths.
                 */
                std::vector<double> GetBandwidths();

                /**
                 *
                 * @return Number of calls for a function.
                 */
                unsigned int
                GetNumberOfCalls() const;

                /**
                 * @brief Adds a snapshot object to channel.
                 * @param apSnapshot
                 */
                void
                AddSnapshot(core::snapshot::Snapshot *apSnapshot);

                /**
                 * @brief Calls all resolve functions of current snapshots
                 */
                void
                Resolve();

                /**
                 * @brief Setter
                 * @param aGridSize
                 */
                void
                SetGridSize(int aGridSize);

                /**
                 * @brief Setter
                 * @param aDataSize
                 */
                void
                SetDataSize(int aDataSize);

                /**
                 * @brief Setter
                 * @param aFLOPS
                 */
                void
                SetFLOPS(int aFLOPS);

                /**
                 * @brief Getter
                 */
                int
                GetGridSize() const;

                /**
                 * @brief Getter
                 */
                int
                GetDataSize() const;

                /**
                 * @brief Getter
                 */
                double
                GetMaxBandwidth();

                /**
                 * @brief Getter
                 */
                double
                GetMinBandwidth();

                /**
                 * @brief Getter
                 */
                double
                GetAverageBandwidth();

                /**
                 * @brief Getter
                 */
                double
                GetMaxThroughput();

                /**
                 * @brief Getter
                 */
                double
                GetMinThroughput();

                /**
                 * @brief Getter
                 */
                double
                GetAverageThroughput();

                /**
                 * @brief Getter
                 */
                double
                GetMinGFLOPS();

                /**
                 * @brief Getter
                 */
                double
                GetMaxGFLOPS();

                /**
                 * @brief Getter
                 */
                double
                GetAverageGFLOPS();

                /**
                 * @brief Getter
                 */
                int
                GetNumberOfOperations() const;

            private:
                /// A map that holds all statistics of a channel object.
                std::map<std::string, double> mStatisticsMap;
                /// Vector of runtimes accompanied with a channel object.
                std::vector<double> mRuntimes;
                /// Vector of bandwidths
                std::vector<double> mBandwidths;
                /// Number of times the function is called.
                unsigned int mNumberOfCalls;
                /// Vector of snapshots accompanied with this timer.
                std::vector<core::snapshot::Snapshot *> mSnapshots;
                /// Grid size
                int mGridSize;
                /// Size of data
                int mDataSize;
                /// Number of floating point operations per iteration
                int mFLOPS;
                /// Flag to determine if this channel has been resolved
                bool mResolved = false;
            };
        }//namespace data
    }//namespace timer
}//namespace bs

#endif // BS_TIMER_DATA_UNITS_CHANNEL_STATS_HPP
