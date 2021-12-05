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

#include <sys/stat.h>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <bs/io/streams/concrete/readers/SeismicReader.hpp>
#include <bs/io/streams/concrete/writers/SeismicWriter.hpp>

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/configurations/MapKeys.h>
#include <bs/io/test-utils/DataGenerator.hpp>

using namespace std;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using namespace bs::io::testutils;
using json = nlohmann::json;


void
TEST_SEISMIC_FORMAT() {
    /* Create tests results directory. */
    string dir(IO_TESTS_RESULTS_PATH);
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    json node;
    node[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;
    node[IO_K_PROPERTIES][IO_K_FLOAT_FORMAT] = 1;
    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_ONLY] = false;
    node[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_STORE] = false;

    vector<Gather *> gathers;

    Gather *gather1 = DataGenerator::GenerateGather(1, 10, 1);
    gathers.push_back(gather1);

    Gather *gather2 = DataGenerator::GenerateGather(1, 10, 10);
    gathers.push_back(gather2);

    int traces_before = 20;

    JSONConfigurationMap writer_map = JSONConfigurationMap(node);

    /* SEG-Y Writer. */
    SeismicWriter w1(WriterType::SEGY, &writer_map);
    w1.AcquireConfiguration();

    string file_path_segy(IO_TESTS_RESULTS_PATH "/SEGYFile");
    w1.Initialize(file_path_segy);
    REQUIRE(w1.Write(gathers) == 0);

    w1.Finalize();

    /* SU Writer. */
    SeismicWriter w2(WriterType::SU, &writer_map);
    w2.AcquireConfiguration();

    string file_path_su("SUFile");
    w2.Initialize(file_path_su);
    REQUIRE(w2.Write(gathers) == 0);

    w2.Finalize();

    /* SEG-Y Reader. */
    SeismicReader r1(ReaderType::SEGY, &writer_map);
    r1.AcquireConfiguration();
    vector<TraceHeaderKey> keys = {TraceHeaderKey::FLDR};
    pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection> p1(TraceHeaderKey::FLDR,
                                                                      bs::io::dataunits::Gather::SortDirection::ASC);
    vector<pair<TraceHeaderKey, bs::io::dataunits::Gather::SortDirection>> sort_keys = {p1};
    vector<string> write_paths = {file_path_segy + ".segy"};
    r1.Initialize(keys, sort_keys, write_paths);

    vector<Gather *> read_gathers1 = r1.ReadAll();
    size_t traces_after = 0;
    for (auto &g : read_gathers1) {
        traces_after += g->GetNumberTraces();
    }
    REQUIRE(traces_before == traces_after);
    REQUIRE(read_gathers1.size() == 19);


    REQUIRE(SeismicWriter::ToString(WriterType::BINARY) == "binary");
    REQUIRE(SeismicReader::ToReaderType("json") == ReaderType::JSON);

    /* JSON Reader. */
    SeismicReader r2(ReaderType::JSON, nullptr);
    vector<string> read_paths = {IO_TESTS_WORKLOADS_PATH "synthetic_velocity.json"};
    r2.Initialize(keys, sort_keys, read_paths);

    vector<Gather *> read_gathers2 = r2.ReadAll();
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