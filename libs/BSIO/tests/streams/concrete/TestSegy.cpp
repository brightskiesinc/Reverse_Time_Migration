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

#include <bs/io/streams/concrete/readers/SegyReader.hpp>
#include <bs/io/streams/concrete/writers/SegyWriter.hpp>

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/configurations/MapKeys.h>
#include <bs/io/test-utils/DataGenerator.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

#include <sys/stat.h>

using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::io::testutils;
using namespace bs::base::configurations;
using json = nlohmann::json;
using std::vector;
using std::pair;


void
TEST_SEGY_FORMAT() {
    /* Create tests results directory. */
    std::string dir(IO_TESTS_RESULTS_PATH);
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    json node;
    node[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;
    node[IO_K_PROPERTIES][IO_K_FLOAT_FORMAT] = 1;
    JSONConfigurationMap jmap = JSONConfigurationMap(node);
    vector<Gather *> gathers;
    Gather *gather = DataGenerator::GenerateGather(1, 10, 1);
    gathers.push_back(gather);
    Gather *gather2 = DataGenerator::GenerateGather(1, 10, 10);
    gathers.push_back(gather2);
    int traces_before = 20;

    SegyWriter writer(&jmap);
    writer.AcquireConfiguration();
    std::string file_path(IO_TESTS_RESULTS_PATH "/SEGYFile");
    writer.Initialize(file_path);
    REQUIRE(writer.Write(gathers) == 0);
    writer.Finalize();

    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_ONLY] = false;
    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_STORE] = false;
    JSONConfigurationMap inmap = JSONConfigurationMap(node);
    SegyReader reader(&inmap);
    reader.AcquireConfiguration();
    vector<TraceHeaderKey> keys = {TraceHeaderKey::FLDR};
    pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection> p1(TraceHeaderKey::FLDR,
                                                                      bs::io::dataunits::Gather::SortDirection::ASC);
    vector<pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection>> sort_keys = {p1};
    vector<std::string> paths = {file_path + ".segy"};
    reader.Initialize(keys, sort_keys, paths);

    vector<Gather *> read_gathers = reader.ReadAll();
    size_t traces_after = 0;
    for (auto g : read_gathers) {
        traces_after += g->GetNumberTraces();
    }

    REQUIRE(traces_before == traces_after);
    REQUIRE(read_gathers.size() == 19);

    vector<vector<std::string>> aHeaderValues;
    vector<std::string> g1 = {"10"};
    vector<std::string> g2 = {"9"};
    aHeaderValues.push_back(g1);
    aHeaderValues.push_back(g2);

    vector<Gather *> read_gathers2 = reader.Read(aHeaderValues);
    REQUIRE(read_gathers2.size() == 2);
    traces_after = 0;
    for (auto g: read_gathers2) {
        traces_after += g->GetNumberTraces();
    }
    REQUIRE(traces_after == 3);

    vector<vector<std::string>> ids = reader.GetIdentifiers();
    REQUIRE(ids.size() == 19);

    Gather *temp = reader.Read(0);
    REQUIRE(temp->GetNumberTraces() == 2);

    reader.Finalize();
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test both SegyWriter and SegyReader.
 *
 * 1. Dummy gather to be generated and written to a file with a *.segy format and then shall be read
 *    using the SegyReader and output gather shall then be compared against the formerly generated one.
 *
 * 2. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 3. RC values to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Segy Format Test") {
TEST_SEGY_FORMAT();

}