/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/streams/concrete/readers/JsonReader.hpp>

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <prerequisites/libraries/catch/catch.hpp>
#include <iostream>

using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using json = nlohmann::json;
using std::vector;
using std::pair;

void
TEST_JSON() {
    JsonReader reader(nullptr);
    vector<TraceHeaderKey> keys = {TraceHeaderKey::FLDR};
    pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection> p1(TraceHeaderKey::FLDR,
                                                                      bs::io::dataunits::Gather::SortDirection::ASC);
    vector<pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection>> sort_keys = {p1};
    vector<std::string> aPaths = {IO_TESTS_WORKLOADS_PATH "/synthetic_velocity.json"};
    reader.Initialize(keys, sort_keys, aPaths);

    vector<Gather *> read_gathers = reader.ReadAll();
    REQUIRE(read_gathers.size() == 1);
    REQUIRE(read_gathers[0]->GetNumberTraces() == 500);

    vector<vector<std::string>> ids = reader.GetIdentifiers();

    vector<vector<std::string>> aHeaderValues;
    vector<std::string> g1 = {"1"};
    vector<std::string> g2 = {"9"};
    aHeaderValues.push_back(g1);
    aHeaderValues.push_back(g2);

    vector<Gather *> read_gathers2 = reader.Read(aHeaderValues);
    REQUIRE(read_gathers2.size() == 1);
    REQUIRE(read_gathers2[0]->GetNumberTraces() == 500);

    Gather *temp = reader.Read(0);
    REQUIRE(temp->GetNumberTraces() == 500);

    reader.Finalize();
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test only JsonReader.
 *
 * 1. Dummy json map to be generated that describe a desired output model and the create it's gather
 *    object that reflects to that map.
 *
 * 2. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 3. RC values to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("JsonReader") {
TEST_JSON();

}