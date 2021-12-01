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

#include <bs/io/streams/concrete/readers/SUReader.hpp>
#include <bs/io/streams/concrete/writers/SUWriter.hpp>
#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/configurations/MapKeys.h>
#include <bs/io/test-utils/DataGenerator.hpp>

using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using namespace bs::io::testutils;
using json = nlohmann::json;
using std::vector;


void
TEST_SU_FORMAT() {
    /* Create tests results directory. */
    std::string dir(IO_TESTS_RESULTS_PATH);
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    SECTION("Write Big Endian") {
        json node;
        node[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;
        node[IO_K_PROPERTIES][IO_K_WRITE_PATH] = std::string(IO_TESTS_RESULTS_PATH);
        JSONConfigurationMap map = JSONConfigurationMap(node);

        vector<Gather *> gathers;

        Gather *gather1 = DataGenerator::GenerateGather(0, 10, 1);
        gathers.push_back(gather1);

        Gather *gather2 = DataGenerator::GenerateGather(0, 10, 10);
        gathers.push_back(gather2);

        SUWriter w(&map);
        w.AcquireConfiguration();
        std::string filePath("SUFile");
        w.Initialize(filePath);
        REQUIRE(w.Write(gathers) == 0);

        w.Finalize();
    }

    SECTION("Write Little Endian") {
        json node;
        node[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = true;
        node[IO_K_PROPERTIES][IO_K_WRITE_PATH] = std::string(IO_TESTS_RESULTS_PATH);
        JSONConfigurationMap map = JSONConfigurationMap(node);

        vector<Gather *> gathers;

        Gather *gather1 = DataGenerator::GenerateGather(0, 10, 1);
        gathers.push_back(gather1);

        Gather *gather2 = DataGenerator::GenerateGather(0, 10, 10);
        gathers.push_back(gather2);

        SUWriter w(&map);
        w.AcquireConfiguration();
        std::string filePath("SUFile");
        w.Initialize(filePath);
        REQUIRE(w.Write(gathers) == 0);

        w.Finalize();
    }
}


/**
 * REQUIRED TESTS:
 *
 * N.B. This test shall test both SUWriter and SUReader.
 *
 * 1. Dummy gather to be generated and written to a file with a *.su format and then shall be read
 *    using the SegyReader and output gather shall then be compared against the formerly generated one.
 *
 * 2. All functions individually to return desired output in same test case yet different sections,
 *    also different permutations of configuration map shall be used to assure correct variations.
 *
 * 3. RC values to be tested and required against its ground truth
 *    (i.e. both failure and success cases shall be tested)
 */

TEST_CASE("SU Format Test") {
    TEST_SU_FORMAT();
}