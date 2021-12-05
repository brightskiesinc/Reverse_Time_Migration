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

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <bs/io/api/cpp/BSIO.hpp>
#include <bs/io/utils/timer/ExecutionTimer.hpp>
#include <bs/io/utils/convertors/KeysConvertor.hpp>

using namespace std;
using json = nlohmann::json;
using namespace bs::base::configurations;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::io::utils::timer;

int main(int argc, char *argv[]) {

    if (argc < 4) {
        std::cout << "Invalid number of parameters..." << std::endl;
        std::cout << "Expected command : ReaderMetrics "
                     "<input_format> <input_path> <input_configuration> <gather_key_1,gather_key_2> "
                  << std::endl;
        exit(0);
    }

    std::string input_format = argv[1];
    std::string input_path = argv[2];
    std::string input_configuration = argv[3];

    std::ifstream fin(input_configuration);

    json configuration_map;
    fin >> configuration_map;
    fin.close();

    string intermediate;
    std::vector<TraceHeaderKey> gather_keys;
    if (argc >= 5) {
        stringstream gathers_string(argv[4]);
        while (getline(gathers_string, intermediate, ',')) {
            gather_keys.push_back(bs::io::utils::convertors::KeysConvertor::ToTraceHeaderKey(intermediate));
        }
    } else {
        gather_keys.emplace_back(TraceHeaderKey::FLDR);
    }

    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;

    stringstream paths_keys_string(input_path);
    std::vector<std::string> paths;
    while (getline(paths_keys_string, intermediate, ',')) {
        paths.push_back(intermediate);
    }

    SeismicReader sr(
            SeismicReader::ToReaderType(input_format),
            new JSONConfigurationMap(configuration_map));
    sr.AcquireConfiguration();

    /* Initializing + Indexing. */
    std::cout << std::endl << "Initializing + Indexing:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        sr.Initialize(gather_keys, sorting_keys, paths);
    }, true);

    std::vector<std::vector<std::string>> unique_identifiers;
    std::cout << std::endl << "Getting unique identifiers:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        unique_identifiers = sr.GetIdentifiers();
    }, true);

    /**  Read only one shot that exists */
    Gather *gather;
    std::vector<std::string> unique_values;
    unique_values.emplace_back("0");
    std::cout << std::endl << "Read one non existing shot: " << std::endl;
    ExecutionTimer::Evaluate([&]() {
        gather = sr.Read(unique_values);
        if (gather != nullptr && gather->GetNumberTraces() > 0) {
            std::cout << std::endl << "A gather has been read!!!" << std::endl;
        }
    }, true);
    delete gather;

    if (!unique_identifiers.empty()) {
        unique_values = unique_identifiers[unique_identifiers.size() / 2];
        std::cout << std::endl << "Read one existing shot: " << std::endl;
        ExecutionTimer::Evaluate([&]() {
            gather = sr.Read(unique_values);
        }, true);
        delete gather;
    }


    if (unique_identifiers.size() > 9) {
        std::cout << std::endl << "Window ten shots : " << std::endl;
        vector<Gather *> gathers;
        std::vector<std::vector<std::string>> window_headers;
        unsigned long starting_index = unique_identifiers.size() / 2 - 5;
        window_headers.reserve(10);
        for (int i = 0; i < 10; i++) {
            window_headers.push_back(unique_identifiers[starting_index + i]);
        }
        ExecutionTimer::Evaluate([&]() {
            gathers = sr.Read(window_headers);
        }, true);
    }


    if (unique_identifiers.size() > 99) {
        vector<Gather *> gathers;
        std::vector<std::vector<std::string>> window_headers;
        unsigned long starting_index = unique_identifiers.size() / 2 - 50;
        window_headers.reserve(100);
        for (int i = 0; i < 100; i++) {
            window_headers.push_back(unique_identifiers[starting_index + i]);
        }
        std::cout << std::endl << "Window hundred shots : " << std::endl;
        ExecutionTimer::Evaluate([&]() {
            gathers = sr.Read(window_headers);
        }, true);
    }

    /* Finalize and closes all opened internal streams. */
    std::cout << std::endl << "Finalizing Reader:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        sr.Finalize();
    }, true);
    return 0;
}
