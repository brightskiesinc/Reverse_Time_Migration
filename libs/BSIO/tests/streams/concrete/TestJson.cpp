/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <bs/io/streams/concrete/readers/JsonReader.hpp>
#include <bs/io/data-units/concrete/Gather.hpp>


using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using json = nlohmann::json;
using std::vector;
using std::pair;

void
TEST_JSON() {
    JsonReader r(nullptr);

    vector<TraceHeaderKey> keys = {TraceHeaderKey::FLDR};
    pair<TraceHeaderKey, Gather::SortDirection> p1(TraceHeaderKey::FLDR,
                                                   Gather::SortDirection::ASC);
    vector<pair<TraceHeaderKey, Gather::SortDirection>> sort_keys = {p1};
    vector<std::string> paths = {IO_TESTS_WORKLOADS_PATH "/synthetic_velocity.json"};

    r.Initialize(keys, sort_keys, paths);

    vector<Gather *> read_gathers = r.ReadAll();
    REQUIRE(read_gathers.size() == 1);
    REQUIRE(read_gathers[0]->GetNumberTraces() == 500);

    vector<vector<std::string>> ids = r.GetIdentifiers();

    vector<vector<std::string>> header_values;
    vector<std::string> g1 = {"1"};
    vector<std::string> g2 = {"9"};
    header_values.push_back(g1);
    header_values.push_back(g2);

    vector<Gather *> read_gathers2 = r.Read(header_values);
    REQUIRE(read_gathers2.size() == 1);
    REQUIRE(read_gathers2[0]->GetNumberTraces() == 500);

    Gather *temp = r.Read(0);
    REQUIRE(temp->GetNumberTraces() == 500);

    r.Finalize();
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