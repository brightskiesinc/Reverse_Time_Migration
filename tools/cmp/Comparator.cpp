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

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#define STEP    1
#define MAX_NT  2
#define SRC     3
#define DST     4
#define PRFX    5

using namespace std;

void compare_files(const string &name, const string &cpu, const string &gpu);

void read_csv_header(uint *nz, uint *nx, const string &file_name);

void read_csv(float *mat, uint nz, uint nx, const string &file_name);

double within_epsilon(float *output, float *reference,
                      size_t dim_x, size_t dim_z, unsigned int radius,
                      int z_adjust, float delta, const string &time_step);

void print_help();

int main(int argc, char *argv[]) {
    if (argc == 3) {
        string file1(argv[STEP]);
        string file2(argv[MAX_NT]);
        compare_files("File comparison", file1, file2);
    } else if (argc == 5) {
        int step = stoi(argv[STEP]);
        int nt = stoi(argv[MAX_NT]);
        string path1(argv[SRC]);
        string path2(argv[DST]);
        string prefix(argv[PRFX]);
        for (int i = step; i <= nt; i += step) {
            string time_step = to_string(i);
            string file_1 = path1 + prefix + "_" + time_step + ".csv";
            string file_2 = path2 + prefix + "_" + time_step + ".csv";
            compare_files(time_step, file_1, file_2);
        }
    } else {
        print_help();
    }
    return 0;
}

/**
 * @brief Host-Code. Utility function to calculate L2-norm between resulting
 * buffer and reference buffer.
 */
double within_epsilon(float *output, float *reference,
                      const size_t dim_x, const size_t dim_z, const unsigned int radius,
                      const int z_adjust, const float delta, const string &time_step) {

    bool error = false;
    double norm2 = 0;

    for (size_t iz = 0; iz < dim_z; iz++) {
        for (size_t ix = 0; ix < dim_x; ix++) {
            if (ix >= radius && ix < (dim_x - radius) && iz >= radius &&
                iz < (dim_z - radius + z_adjust)) {
                float difference = fabsf(*reference - *output);
                norm2 += difference * difference;
                if (difference > delta) {
                    error = true;
                    cout << time_step << "\t"
                         << "ERROR (" << ix << "," << iz << ") = " << *output << " "
                         << *reference << " " << difference << endl;
                }
            }
            ++output;
            ++reference;
        }
    }
    norm2 = sqrt(norm2);
    return norm2;
}

void read_csv(float *mat, uint nz, uint nx, const string &file_name) {
    std::ifstream in(file_name);
    if (!in.is_open()) {
        cout << "Couldn't open file : " << file_name << endl;
    }
    string line, word;
    getline(in, line);
    for (uint row = 0; row < nz; row++) {
        getline(in, line, '\n');
        stringstream s(line);
        for (uint col = 0; col < nx; col++) {
            getline(s, word, ',');
            mat[row * nx + col] = stod(word);
        }
    }
}

void read_csv_header(uint *nz, uint *nx, const string &file_name) {
    std::ifstream in(file_name);
    if (!in.is_open()) {
        cout << "Couldn't open file : " << file_name << endl;
    }
    char v;
    int n;
    in >> n;
    *nx = n;
    in >> v;
    in >> n;
    *nz = n;
    in >> v;
}

void compare_files(const string &name, const string &cpu, const string &gpu) {
    uint nz, nx;
    read_csv_header(&nz, &nx, cpu);
    auto *cpu_array = (float *) malloc(sizeof(float) * nz * nx);
    auto *gpu_array = (float *) malloc(sizeof(float) * nz * nx);
    read_csv(cpu_array, nz, nx, cpu);
    read_csv(gpu_array, nz, nx, gpu);
    float abs_error = 0;
    float cpu_rel_error = 0;
    float gpu_rel_error = 0;
    for (int i = 0; i < nz; i++) {
        for (int j = 0; j < nx; j++) {
            abs_error += abs(cpu_array[i * nx + j] - gpu_array[i * nx + j]);
        }
    }
    double l2_error =
            within_epsilon(gpu_array, cpu_array, nz, nx, 4, 0, 0.1f, name);

    cout << name << "\t" << abs_error << "\t" << l2_error << endl;
    free(cpu_array);
    free(gpu_array);
}

void print_help() {
    cout << "Usage:" << endl
         << "\t ./Comparator <step> <max-nt> <src> <dst> <csv-prefix>" << endl
         << "\t ./Comparator <src> <dst>" << endl;
}
