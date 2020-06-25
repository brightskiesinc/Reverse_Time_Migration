#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

/*
 * Host-Code
 * Utility function to calculate L2-norm between resulting buffer and reference
 * buffer
 */
double within_epsilon(float *output, float *reference, const size_t dimx,
                      const size_t dimz, const unsigned int radius,
                      const int zadjust, const float delta, string timestep) {

  bool error = false;
  double norm2 = 0;

  for (size_t iz = 0; iz < dimz; iz++) {
    for (size_t ix = 0; ix < dimx; ix++) {
      if (ix >= radius && ix < (dimx - radius) && iz >= radius &&
          iz < (dimz - radius + zadjust)) {
        float difference = fabsf(*reference - *output);
        norm2 += difference * difference;
        if (difference > delta) {
          error = true;
          cout << timestep << "\t"
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

void ReadCsv(float *mat, uint nz, uint nx, string filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "Couldn't open file : " << filename << endl;
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

void ReadCsvHeader(uint *nz, uint *nx, string filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "Couldn't open file : " << filename << endl;
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

void compare_files(string name, string cpu, string gpu) {
  uint nz, nx;
  ReadCsvHeader(&nz, &nx, cpu);
  float *cpu_array = (float *)malloc(sizeof(float) * nz * nx);
  float *gpu_array = (float *)malloc(sizeof(float) * nz * nx);
  ReadCsv(cpu_array, nz, nx, cpu);
  ReadCsv(gpu_array, nz, nx, gpu);
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

int main(int argc, char *argv[]) {
  if (argc == 3) {
    string file1(argv[1]);
    string file2(argv[2]);
    compare_files("File comparision", file1, file2);
  } else if (argc == 6) {
    int step = stoi(argv[1]);
    int nt = stoi(argv[2]);
    string path1(argv[3]);
    string path2(argv[4]);
    string prefix(argv[5]);
    for (int i = step; i <= nt; i += step) {
      string timestep = to_string(i);
      string file_1 = path1 + prefix + "_" + timestep + ".csv";
      string file_2 = path2 + prefix + "_" + timestep + ".csv";
      compare_files(timestep, file_1, file_2);
    }
  } else {
    printf("Usage : ./compare_csv <step> <max_nt> <path_1> <path_2> "
           "<csv_prefix>\n");
    printf("Usage : ./compare_csv <file_1> <file_2>\n");
  }
  return 0;
}
