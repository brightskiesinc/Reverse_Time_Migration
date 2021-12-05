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

#include <vector>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/timer/utils/stats/StatisticsHelper.hpp>

using namespace bs::timer::utils::stats;


TEST_CASE("Statistics Helper - Class", "[Utils]") {
    SECTION("Long") {
        std::vector<long> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        REQUIRE(StatisticsHelper::GetMin(nums) == 1);
        REQUIRE(StatisticsHelper::GetMax(nums) == 10);
        REQUIRE(StatisticsHelper::GetAverage(nums) == 5.5);
        REQUIRE(StatisticsHelper::GetVariance(nums) == 8.25);
        REQUIRE(StatisticsHelper::GetDeviation(nums) == Approx(2.8723));

        nums = {-190000000, -100000000, 100000000, -1000000000, 190000000,
                280000000, 370000000, 460000000, 550000000, 640000000,
                730000000, 820000000, 910000000, 1000000000};
        REQUIRE(StatisticsHelper::GetMin(nums) == -1000000000);
        REQUIRE(StatisticsHelper::GetMax(nums) == 1000000000);
        REQUIRE(StatisticsHelper::GetAverage(nums) == 340000000);
        REQUIRE(StatisticsHelper::GetVariance(nums) == Approx(2.6044285714286e+17));
        REQUIRE(StatisticsHelper::GetDeviation(nums) == Approx(510336023.75578));

        nums = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        REQUIRE(StatisticsHelper::GetMin(nums) == 0);
        REQUIRE(StatisticsHelper::GetMax(nums) == 0);
        REQUIRE(StatisticsHelper::GetAverage(nums) == 0);
        REQUIRE(StatisticsHelper::GetVariance(nums) == 0);
        REQUIRE(StatisticsHelper::GetDeviation(nums) == 0);

        nums = {};
        REQUIRE(StatisticsHelper::GetMin(nums) == 0);
        REQUIRE(StatisticsHelper::GetMax(nums) == 0);
        REQUIRE(StatisticsHelper::GetAverage(nums) == 0);
        REQUIRE(StatisticsHelper::GetVariance(nums) == 0);
        REQUIRE(StatisticsHelper::GetDeviation(nums) == 0);
    }

    SECTION("Double") {
        std::vector<double> numsD = {0.000001, -0.000001, 1.1, 999999.99, -9999.9};
        REQUIRE(StatisticsHelper::GetMin(numsD) == -9999.9);
        REQUIRE(StatisticsHelper::GetMax(numsD) == 999999.99);
        REQUIRE(StatisticsHelper::GetAverage(numsD) == Approx(198000.238));
        REQUIRE(StatisticsHelper::GetVariance(numsD) == Approx(160815901352.19));
        REQUIRE(StatisticsHelper::GetDeviation(numsD) == Approx(401018.5798092));
    }
}
