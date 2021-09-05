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
#include <bs/io/utils/convertors/KeysConvertor.hpp>
#include <bs/io/utils/timer/ExecutionTimer.hpp>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <iostream>

using namespace std;
using namespace bs::io::streams;
using namespace bs::base::configurations;
using namespace bs::io::dataunits;
using namespace bs::io::utils::timer;
using json = nlohmann::json;

bool sortvector(const vector<long long int> &v1, const vector<long long int> &v2) {
    for (int i = 0; i < v1.size(); i++) {
        if (v1[i] < v2[i]) {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {

    if (argc < 7) {
        std::cout << "Invalid number of parameters..." << std::endl;
        std::cout << "Expected command : Sorter "
                     "<input_format> <input_path> <input_configuration> "
                     "<output_format> <output_path> <output_configuration> "
                     "<gather_key_1, gather_key_2,...> <+sort_key_1,-sort_key_2,...>"
                  << std::endl;
        exit(0);
    }

    std::string input_format = argv[1];
    std::string input_path = argv[2];
    std::string input_configuration = argv[3];
    std::string output_format = argv[4];
    std::string output_path = argv[5];
    std::string output_configuration = argv[6];

    string intermediate;
    std::vector<TraceHeaderKey> gather_keys;
    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;

    if (argc >= 8) {
        stringstream gathers_string(argv[7]);
        while (getline(gathers_string, intermediate, ',')) {
            gather_keys.push_back(bs::io::utils::convertors::KeysConvertor::ToTraceHeaderKey(intermediate));
        }
    } else {
        gather_keys.emplace_back(TraceHeaderKey::FLDR);
    }
    if (argc >= 9) {
        stringstream sorting_keys_string(argv[8]);
        while (getline(sorting_keys_string, intermediate, ',')) {
            auto sort_direction = bs::io::dataunits::Gather::SortDirection::ASC;
            if (intermediate[0] == '-' || intermediate[0] == '+') {
                if (intermediate[0] == '-') {
                    sort_direction = bs::io::dataunits::Gather::SortDirection::DES;
                }
                intermediate = intermediate.substr(1);
            }
            auto p = std::make_pair(bs::io::utils::convertors::KeysConvertor::ToTraceHeaderKey(intermediate),
                                    sort_direction);
            sorting_keys.push_back(p);
        }
    } else {
        auto p = std::make_pair(TraceHeaderKey::FLDR, bs::io::dataunits::Gather::SortDirection::ASC);
        sorting_keys.emplace_back(p);
    }

    std::ifstream fin(input_configuration);
    json configuration_map;
    fin >> configuration_map;
    fin.close();


    stringstream paths_keys_string(input_path);
    std::vector<std::string> paths;
    while (getline(paths_keys_string, intermediate, ',')) {
        paths.push_back(intermediate);
    }

    SeismicReader sr(
            SeismicReader::ToReaderType(input_format),
            new JSONConfigurationMap(configuration_map));
    sr.AcquireConfiguration();

    json configuration_map_writer;
    std::ifstream fin2(output_configuration);
    fin2 >> configuration_map_writer;
    fin2.close();

    SeismicWriter iw(
            SeismicWriter::ToWriterType(output_format),
            new JSONConfigurationMap(configuration_map_writer));
    iw.AcquireConfiguration();

    /* Initializing. */
    std::string path = output_path;
    iw.Initialize(path);

    /* Initializing + Indexing. */
    std::cout << std::endl << "Initializing + Indexing:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        sr.Initialize(gather_keys, sorting_keys, paths);
    }, true);

    /* Normal case. */
    std::vector<std::vector<std::string>> unique_values;
    std::cout << std::endl << "Get unique identifiers:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        unique_values = sr.GetIdentifiers();
    }, true);

    std::vector<std::vector<long long int>> unique_values_to_be_sorted;
    std::cout << std::endl << "Casting unique identifiers(string to double):" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        for (const auto &unique_value : unique_values) {
            std::vector<long long int> doubles;
            doubles.reserve(unique_value.size());
            for (const auto &temp: unique_value) {
                doubles.push_back(std::stod(temp));
            }
            unique_values_to_be_sorted.push_back(doubles);
        }
    }, true);

    std::cout << std::endl << "Sorting unique identifiers:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        std::sort(unique_values_to_be_sorted.begin(),
                  unique_values_to_be_sorted.end(), sortvector);
    }, true);

    Gather *gather;
    unique_values.clear();
    std::cout << std::endl << "Casting unique identifiers(double to string):" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        for (const auto &unique_value : unique_values_to_be_sorted) {
            std::vector<std::string> strings;
            strings.reserve(unique_value.size());
            for (const auto &temp: unique_value) {
                strings.push_back(to_string(temp));
            }
            unique_values.push_back(strings);
        }
    }, true);

    std::cout << std::endl << "Reading, Sorting, & writing sorted gathers:" << std::endl;

    long read_microseconds = 0;
    long write_microseconds = 0;
    long sort_microseconds = 0;

    ExecutionTimer::Evaluate([&]() {
        for (const auto &value: unique_values) {
            read_microseconds += ExecutionTimer::Evaluate([&]() {
                gather = sr.Read(value);
            }, false);
            sort_microseconds += ExecutionTimer::Evaluate([&]() {
                gather->SortGather(sorting_keys);
            }, false);
            write_microseconds += ExecutionTimer::Evaluate([&]() {
                iw.Write(gather);
            }, false);
            delete gather;
        }
    }, true);

    std::cout << "Read Time : " << (read_microseconds / (1e6f)) << " SEC" << std::endl;
    std::cout << "Sort Time : " << (sort_microseconds / (1e6f)) << " SEC" << std::endl;
    std::cout << "Write Time : " << (write_microseconds / (1e6f)) << " SEC" << std::endl;

    /* Finalize and closes all opened internal streams. */
    std::cout << std::endl << "Finalizing Reader:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        sr.Finalize();
    }, true);

    /* Finalize and closes all opened internal streams. */
    std::cout << std::endl << "Finalizing Reader:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        iw.Finalize();
    }, true);
    return 0;
}