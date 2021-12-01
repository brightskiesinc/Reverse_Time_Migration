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

#ifndef BS_TIMER_UTILS_STATISTICS_HELPER_HPP
#define BS_TIMER_UTILS_STATISTICS_HELPER_HPP

#include <vector>
#include <cmath>
#include <type_traits>

#include <bs/base/common/assertions.h>

namespace bs {
    namespace timer {
        namespace utils {
            namespace stats {

                /**
                 * @brief Statistics helper class. Serves all statistical methods, like min, max,
                 * variance, deviation and so on.
                 */
                class StatisticsHelper {
                public:
                    /**
                     * @brief Takes a vector of longs/doubles and returns its maximum.
                     */
                    template<typename T>
                    static T
                    GetMax(const std::vector<T> aNums) {
                        T max = 0;
                        if (!aNums.empty()) {
                            ASSERT_IS_POD(T);
                            max = aNums.at(0);
                            for (auto element: aNums) {
                                if (element > max)
                                    max = element;
                            }
                        }
                        return max;
                    }

                    /**
                     * @brief Takes a vector of longs/doubles and returns its minimum.
                     */
                    template<typename T>
                    static T
                    GetMin(const std::vector<T> aNums) {
                        T min = 0;
                        if (!aNums.empty()) {
                            ASSERT_IS_POD(T);
                            min = aNums.at(0);
                            for (auto element: aNums) {
                                if (element < min) {
                                    min = element;
                                }
                            }
                        }
                        return min;
                    }

                    /**
                     * @brief Takes a vector of longs/doubles and returns its total.
                    */
                    template<typename T>
                    static T
                    GetTotal(const std::vector<T> aNums) {
                        T total = 0;
                        if (!aNums.empty()) {
                            ASSERT_IS_POD(T);
                            for (auto element: aNums) {
                                total += element;
                            }
                        }
                        return total;
                    }

                    /**
                     * @brief Takes a vector of longs/doubles and returns its average.
                     */
                    template<typename T>
                    static double
                    GetAverage(const std::vector<T> &aNums) {
                        double average = 0;
                        if (!aNums.empty()) {
                            ASSERT_IS_POD(T);
                            double total = 0;
                            for (auto element : aNums) {
                                total += element;
                            }
                            average = total / aNums.size();
                        }
                        return average;
                    }

                    /**
                     * @brief Takes a vector of longs/doubles and returns its variance.
                     */
                    template<typename T>
                    static double
                    GetVariance(const std::vector<T> &aNums) {
                        double variance = 0;
                        if (!aNums.empty()) {
                            ASSERT_IS_POD(T);
                            auto mean = GetAverage(aNums);
                            double total = 0;
                            for (auto element : aNums) {
                                total += pow((element - mean), 2);
                            }
                            variance = total / aNums.size();
                        }
                        return variance;
                    }

                    /**
                     * @brief Takes a vector of longs/doubles and returns its deviation.
                     */
                    template<typename T>
                    static double
                    GetDeviation(const std::vector<T> &aNums) {
                        double deviation = 0;
                        if (!aNums.empty()) {
                            deviation = sqrt(GetVariance(aNums));
                        }
                        return deviation;
                    }

                private:
                    /**
                     * @brief Private constructor for preventing objects creation.
                     */
                    StatisticsHelper() = default;
                };
            }//namespace stats
        }//namespace utils
    }//namespace timer
}//namespace bs


#endif // BS_TIMER_UTILS_STATISTICS_HELPER_HPP
