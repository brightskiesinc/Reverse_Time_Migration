/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <random>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

#include <prerequisites/libraries/nlohmann/json.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <operations/utils/io/write_utils.h>

#define GREEN "\033[1;32m"

using namespace std;
using json = nlohmann::json;

#define META_DATA json

META_DATA read_file(const string &file_name);

void print_help();

void generate_model(META_DATA &meta_data, float *data);

void generate_layers(META_DATA &meta_data, float *data);

void inject_salt_bodies(META_DATA &meta_data, float *data);

void inject_cracks(META_DATA &meta_data, float *data);

void write_csv(float *data, uint nx, uint ny, uint nz, const string &file_name);

int main(int argc, char *argv[]) {
    string meta_data_file;
    string data_file;

    int opt;
    while ((opt = getopt(argc, argv, ":m:d")) != -1) {
        switch (opt) {
            case 'm':
                meta_data_file = string(optarg);
                break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    META_DATA meta_data = read_file(meta_data_file);

    uint nx = 0, ny = 0, nz = 0;
    nx = meta_data["grid-size"]["nx"].get<int>();
    ny = meta_data["grid-size"]["ny"].get<int>();
    nz = meta_data["grid-size"]["nz"].get<int>();

    uint bytes = nx * ny * nz * sizeof(float);

    auto data = (float *) malloc(bytes);
    memset(data, 0.0f, bytes);

    string model_name = "synthetic_" + meta_data["model-name"].get<string>();
    generate_model(meta_data, data);

    float dx = 0.0f, dy = 0.0f, dz = 0.0f;
    dx = meta_data["cell-dimension"]["dx"].get<float>();
    dy = meta_data["cell-dimension"]["dy"].get<float>();
    dz = meta_data["cell-dimension"]["dz"].get<float>();
    if (dx <= 0.0f) {
        dx = 1.0f;
    }
    if (dy <= 0.0f) {
        dy = 1.0f;
    }
    if (dz <= 0.0f) {
        dz = 1.0f;
    }

    nlohmann::json configuration_map;
    bs::base::configurations::JSONConfigurationMap io_map(configuration_map);
    bs::io::streams::SeismicWriter writer(bs::io::streams::SeismicWriter::ToWriterType("segy"),
                                         &io_map);
    writer.AcquireConfiguration();
    writer.Initialize(model_name);
    auto gathers = operations::utils::io::TransformToGather(data, nx, ny, nz, dx, dy, dz,
                                                            0, 0, 0, 0,
                                                            1, 1e3, 1e3);
    writer.Write(gathers);
    writer.Finalize();
    for (auto g : gathers) {
        delete g;
    }

    auto percentile = meta_data["percentile"].get<float>();
    if (percentile < 0 || percentile > 100) {
        percentile = 98.5;
    }

    write_csv(data, nx, ny, nz, model_name + ".csv");

    free(data);

    cout << GREEN "Synthetic model generated successfully...";

    return 0;
}

void print_help() {
    cout << "Usage:" << endl
         << "\t ./Generator -m <meta_data.json>";
}

META_DATA read_file(const string &file_name) {
    std::ifstream in(file_name.c_str());
    json j;
    in >> j;
    return j;
}

void generate_model(META_DATA &meta_data, float *data) {
    generate_layers(meta_data, data);
    inject_salt_bodies(meta_data, data);
    inject_cracks(meta_data, data);
}

void generate_layers(META_DATA &meta_data, float *data) {
    uint nx = meta_data["grid-size"]["nx"].get<uint>();
    uint ny = meta_data["grid-size"]["ny"].get<uint>();
    uint nz = meta_data["grid-size"]["nz"].get<uint>();

    META_DATA properties = meta_data["properties"];

    float min = 0.0f, max = 0.0f;
    if (!properties["value-range"].empty()) {
        min = properties["value-range"]["min"].get<float>();
        max = properties["value-range"]["max"].get<float>();
    }

    uint num_cracks = 1;
    if (properties["cracks"]["enable"].get<bool>()) {
        num_cracks = properties["cracks"]["count"].get<uint>();
        if (num_cracks < 1) {
            num_cracks = 1;
        }
    }

    uint num_layers = 1;
    string type_layers;
    float sample_step_layer = 0.0f;
    if (properties["layers"]["enable"].get<bool>()) {
        num_layers = properties["layers"]["count"].get<uint>();
        if (num_layers < 1) {
            num_layers = 1;
        }

        type_layers = properties["layers"]["type"].get<string>();
        if (type_layers == "smooth") {
            sample_step_layer = 1.0 * (max - min) / num_layers;
        } else if (type_layers == "sharp") {
            sample_step_layer = -1;
        }
    }

    uint layer = 1;
    float val = 1.0f;
    for (int iy = 0; iy < ny; ++iy) {
        for (int iz = 0; iz < nz; ++iz) {
            if (sample_step_layer == -1) {
                if (iz > (layer * (nz / num_layers))) {
                    layer++;
                    val *= 2;
                }
            } else {
                val += sample_step_layer;
            }
            for (int ix = 0; ix < nx; ++ix) {
                data[iz * nx + ix + (iy * nx * nz)] = val;
            }
        }
    }
}

void inject_salt_bodies(META_DATA &meta_data, float *data) {
    META_DATA salt_bodies = meta_data["properties"]["salt-bodies"];
    if (!salt_bodies["enable"].get<bool>()) {
        return;
    }

    uint nx = meta_data["grid-size"]["nx"].get<uint>();
    uint ny = meta_data["grid-size"]["ny"].get<uint>();
    uint nz = meta_data["grid-size"]["nz"].get<uint>();

    uint salt_bodies_count = 0;
    uint salt_bodies_width = 0;
    salt_bodies_count = salt_bodies["count"].get<uint>();
    if (salt_bodies_count < 1) {
        salt_bodies_count = 1;
    }

    string s_width = salt_bodies["width"].get<string>();
    if (s_width == "narrow") {
        salt_bodies_width = 50;
    } else if (s_width == "wide") {
        salt_bodies_width = 100;
    }

    string salt_bodies_type = salt_bodies["type"].get<string>();

    random_device random;
    mt19937 generate(random());
    uniform_int_distribution<> distribution_nx(salt_bodies_width, nx - salt_bodies_width);
    uniform_int_distribution<> distribution_nz(salt_bodies_width, nz - salt_bodies_width);

    for (int is = 0; is < salt_bodies_count; ++is) {
        uint nx_start = distribution_nz(generate);
        uint nz_start = distribution_nz(generate);

        uint nx_end = nx_start + salt_bodies_width;
        uint nz_end = nz_start + salt_bodies_width;

        float val;
        if (salt_bodies_type == "random") {
            val *= -10.0f;
        } else if (salt_bodies_type == "identical") {
            val = -10.0f;
        }

        for (int iy = 0; iy < ny; ++iy) {
            for (int iz = nz_start; iz < nz_end; ++iz) {
                for (int ix = nx_start; ix < nx_end; ++ix) {
                    data[iz * nx + ix + (iy * nx * nz)] = val;
                }
            }
        }
    }
}

void inject_cracks(META_DATA &meta_data, float *data) {
    /// @todo To be implemented.
}

void write_csv(float *data, uint nx, uint ny, uint nz, const string &file_name) {
    ofstream out(file_name);
    out << nx << "," << nz << "\n";
    for (uint iz = 0; iz < nz; iz++) {
        for (uint ix = 0; ix < nx; ix++)
            out << data[iz * nx + ix] << ',';
        out << '\n';
    }
}