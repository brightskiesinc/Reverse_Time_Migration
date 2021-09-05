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

#include <bs/io/streams/concrete/readers/SeismicReader.hpp>
#include <bs/io/streams/concrete/writers/SeismicWriter.hpp>

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <bs/io/configurations/MapKeys.h>
#include <bs/io/test-utils/DataGenerator.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

#include <sys/stat.h>

using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using namespace bs::io::testutils;
using json = nlohmann::json;
using std::vector;
using std::pair;


void
TEST_SEISMIC_FORMAT() {
    /* Create tests results directory. */
    std::string dir(IO_TESTS_RESULTS_PATH);
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    json node;
    node[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;
    node[IO_K_PROPERTIES][IO_K_FLOAT_FORMAT] = 1;
    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_ONLY] = false;
    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_STORE] = false;

    vector<Gather *> gathers;
    Gather *gather = DataGenerator::GenerateGather(1, 10, 1);
    gathers.push_back(gather);
    Gather *gather2 = DataGenerator::GenerateGather(1, 10, 10);
    gathers.push_back(gather2);
    int traces_before = 20;

    JSONConfigurationMap jmap = JSONConfigurationMap(node);
    SeismicWriter writer(WriterType::SEGY, &jmap);

    writer.AcquireConfiguration();
    std::string file_path_segy(IO_TESTS_RESULTS_PATH "/SEGYFile");
    writer.Initialize(file_path_segy);
    REQUIRE(writer.Write(gathers) == 0);
    writer.Finalize();

    SeismicWriter writer2(WriterType::SU, &jmap);
    writer2.AcquireConfiguration();
    std::string file_path_su("SUFile");
    writer2.Initialize(file_path_su);
    REQUIRE(writer2.Write(gathers) == 0);
    writer2.Finalize();

    SeismicReader reader(ReaderType::SEGY, &jmap);
    reader.AcquireConfiguration();
    vector<TraceHeaderKey> keys = {TraceHeaderKey::FLDR};
    pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection> p1(TraceHeaderKey::FLDR,
                                                                      bs::io::dataunits::Gather::SortDirection::ASC);
    vector<pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection>> sort_keys = {p1};
    vector<std::string> aPaths = {file_path_segy + ".segy"};
    reader.Initialize(keys, sort_keys, aPaths);

    vector<Gather *> read_gathers = reader.ReadAll();
    size_t traces_after = 0;
    for (auto g: read_gathers) {
        traces_after += g->GetNumberTraces();
    }
    REQUIRE(traces_before == traces_after);
    REQUIRE(read_gathers.size() == 19);


    REQUIRE(SeismicWriter::ToString(WriterType::BINARY) == "binary");
    REQUIRE(SeismicReader::ToReaderType("json") == ReaderType::JSON);

    SeismicReader reader2(ReaderType::JSON, nullptr);
    vector<std::string> aPaths2 = {IO_TESTS_WORKLOADS_PATH "synthetic_velocity.json"};
    reader2.Initialize(keys, sort_keys, aPaths2);

    vector<Gather *> read_gathers2 = reader2.ReadAll();
    REQUIRE(read_gathers2.size() == 1);
    REQUIRE(read_gathers2[0]->GetNumberTraces() == 500);
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test both SeismicWriter and SeismicReader.
 *
 * 1. Dummy gather to be generated and written to files with all supported formats and then shall be read
 *    using the SeismicReader and output gather shall then be compared against the formerly generated one.
 *    The provided reader types should be all the supported ones, having an iterator to iterate upon all
 *    to assure all readers follow the same pattern.
 *
 * 2. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 3. RC values to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("Seismic Format Test") {
TEST_SEISMIC_FORMAT();

}