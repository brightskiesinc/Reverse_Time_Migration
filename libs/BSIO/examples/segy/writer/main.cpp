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
#include <bs/io/utils/timer/ExecutionTimer.hpp>
#include <bs/io/configurations/MapKeys.h>

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

    /*
     * Reading SEG-Y file.
     */

    json read_configuration;
    read_configuration[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_ONLY] = false;
    read_configuration[IO_K_PROPERTIES][IO_K_TEXT_HEADERS_STORE] = false;

    std::vector<TraceHeaderKey> gather_keys = {TraceHeaderKey::FLDR};
    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;
    std::vector<std::string> paths = {DATA_PATH "/shots0601_0800.segy",
                                      DATA_PATH "/vel_z6.25m_x12.5m_exact.segy"};

    SegyReader r(new JSONConfigurationMap(read_configuration));
    r.AcquireConfiguration();
    r.Initialize(gather_keys, sorting_keys, paths);

    ExecutionTimer::Evaluate([&]() {
        r.ReadAll();
    }, true);

    /*
     * Writing SEG-Y file.
     */

    json write_configuration;
    write_configuration[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;

    std::string path = WRITE_PATH "/result";
    std::vector<Gather *> gathers;

    SegyWriter w(new JSONConfigurationMap(write_configuration));
    w.AcquireConfiguration();
    w.Initialize(path);

    /* Get gather number */
    std::cout << std::endl << "Normal writing case:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        w.Write(gathers);
    }, true);

    /* Finalize and closes all opened internal streams. */
    w.Finalize();
}
