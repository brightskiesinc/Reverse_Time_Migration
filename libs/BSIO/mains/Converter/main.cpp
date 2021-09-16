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

using namespace std;
using namespace bs::io::streams;
using namespace bs::base::configurations;
using namespace bs::io::dataunits;
using namespace bs::io::utils::timer;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
    if (argc < 7) {
        std::cout << "Invalid number of parameters..." << std::endl;
        std::cout << "Expected command : Converter "
                     "<input_format> <input_path> <input_configuration> "
                     "<output_format> <output_path> <output_configuration> <batch_size-optional>"
                  << std::endl;
        exit(0);
    }
    std::string input_format = argv[1];
    std::string input_path = argv[2];
    std::string input_configuration = argv[3];
    std::string output_format = argv[4];
    std::string output_path = argv[5];
    std::string output_configuration = argv[6];
    unsigned int batch_size = 1;
    if (argc > 7) {
        batch_size = stoi(argv[7]);
    }

    if (batch_size < 1) {
        std::cout << "Batch size must be larger than 0..." << std::endl;
        exit(0);
    }
    std::ifstream fin(input_configuration);
    json configuration_map;
    fin >> configuration_map;
    fin.close();

    std::vector<TraceHeaderKey> gather_keys = {TraceHeaderKey::FLDR};
    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;

    /** from command line */
    stringstream paths_keys_string(input_path);
    std::string intermediate;
    std::vector<std::string> paths;
    while (getline(paths_keys_string, intermediate, ',')) {
        paths.push_back(intermediate);
    }
    /** from command line */
    SeismicReader sr(
            SeismicReader::ToReaderType(input_format),
            new JSONConfigurationMap(configuration_map));
    sr.AcquireConfiguration();

    json configuration_map_writer;
    std::ifstream fin2(output_configuration);
    fin2 >> configuration_map_writer;
    fin2.close();

    /** From command line */
    SeismicWriter iw(
            SeismicWriter::ToWriterType(output_format),
            new JSONConfigurationMap(configuration_map_writer));
    iw.AcquireConfiguration();

    /** From command line */
    /* Initializing. */
    std::string path = output_path;
    iw.Initialize(path);

    /* Initializing + Indexing. */
    std::cout << std::endl << "Initializing + Indexing:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        sr.Initialize(gather_keys, sorting_keys, paths);
    }, true);

    /* Get number of gathers */
    unsigned int num_of_gathers;
    std::cout << std::endl << "Number of gathers:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        num_of_gathers = sr.GetNumberOfGathers();
    }, true);
    std::cout << "Number of gathers: " << num_of_gathers << std::endl;

    std::vector<Gather *> gathers;

    std::cout << std::endl << "Conversion:" << std::endl;
    long read_microseconds = 0;
    long write_microseconds = 0;
    ExecutionTimer::Evaluate([&]() {
        for (unsigned int i = 0; i < num_of_gathers; i += batch_size) {
            int actual_batch_size = std::min(batch_size, num_of_gathers - i);
            read_microseconds += ExecutionTimer::Evaluate([&]() {
                for (unsigned int j = 0; j < actual_batch_size; j++) {
                    gathers.push_back(sr.Read(i));
                }
            }, false);
            write_microseconds += ExecutionTimer::Evaluate([&]() {
                iw.Write(gathers);
            }, false);
            for (auto gather : gathers) {
                delete gather;
            }
            gathers.clear();
        }
    }, true);
    std::cout << "Conversion Read Time : " << (read_microseconds / (1e6f)) << " SEC" << std::endl;
    std::cout << "Conversion Write Time : " << (write_microseconds / (1e6f)) << " SEC" << std::endl;
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
