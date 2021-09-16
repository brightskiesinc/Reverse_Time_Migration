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

#include <bs/io/api/cpp/BSIO.hpp>
#include <bs/io/utils/timer/ExecutionTimer.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace bs::io::streams;
using namespace bs::base::configurations;
using namespace bs::io::dataunits;
using namespace bs::io::utils::timer;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
    mkdir(WRITE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    json configuration_map;
    configuration_map[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_ONLY] = false;
    configuration_map[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_STORE] = false;

    std::vector<TraceHeaderKey> gather_keys = {TraceHeaderKey::FLDR};
    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;
    std::vector<std::string> paths = {DATA_PATH "/shots0601_0800.segy",
                                      DATA_PATH "/vel_z6.25m_x12.5m_exact.segy"};

    SeismicReader r(SeismicReader::ToReaderType("segy"),
                    new JSONConfigurationMap(configuration_map));
    r.AcquireConfiguration();

    /* Initializing + Indexing. */
    std::cout << std::endl << "Initializing + Indexing:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        r.Initialize(gather_keys, sorting_keys, paths);
    }, true);


    /* Normal case. */
    std::cout << std::endl << "Normal reading case [1]:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        r.Read({"604"});
    }, true);

    /* Normal case. */
    std::cout << std::endl << "Normal reading case [2]:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        vector<string> vec = {"729"};
        r.Read(vec);
    }, true);

    /* Not available case. */
    std::cout << std::endl << "Not available reading case:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        vector<string> vec = {"6329"};
        r.Read(vec);
    }, true);

    /* Get gather number */
    std::cout << std::endl << "Get gather number case:" << std::endl;
    unsigned int num;
    ExecutionTimer::Evaluate([&]() {
        num = r.GetNumberOfGathers();
    }, true);
    std::cout << "Number of gathers : " << num << std::endl;

    /* Get gather number */
    std::cout << std::endl << "Get gather number case:" << std::endl;
    std::vector<std::vector<std::string>> keys;
    ExecutionTimer::Evaluate([&]() {
        keys = r.GetIdentifiers();
    }, true);
    std::cout << "Number of gathers : " << keys.size() << std::endl;

    /* Finalize and closes all opened internal streams. */
    r.Finalize();
}
