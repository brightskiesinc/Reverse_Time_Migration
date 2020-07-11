//
// Created by amr on 20/01/2020.
//
#include "compress.h"

int main(int argc, char *argv[]) {
  uint nx = 5000;
  uint nz = 2000;
  uint ny = 1;

  float *pressure = new float[nx * nz * ny];
  float *pressure_2 = new float[nx * nz * ny];
  for (int i = 0; i < ny; i++) {
    for (int j = 0; j < nz / 2; j++) {
      for (int k = 0; k < nx; k++) {
        pressure[i * nx * nz + j * nx + k] = 0.5f;
      }
    }
  }
  for (int i = 0; i < ny; i++) {
    for (int j = nz / 2; j < nz; j++) {
      for (int k = 0; k < nx; k++) {
        pressure[i * nx * nz + j * nx + k] = 1.6f;
      }
    }
  }
  size_t result_size;
  float tolerance = 0.01;
  int wrong_counter = 0;
  zfp::compression(pressure, nx, ny, nz, 1, tolerance, 2, "bp", false);
  zfp::decompression(pressure_2, nx, ny, nz, 1, tolerance, 2, "bp", false);
  for (int i = 0; i < ny; i++) {
    for (int j = nz / 2; j < nz; j++) {
      for (int k = 0; k < nx; k++) {
        uint offset = i * nx * nz + j * nx + k;
        if (fabs(pressure[offset] - pressure_2[offset]) > tolerance) {
          wrong_counter++;
          printf("ERROR : Not matching values org(%f) != reread(%f)\n",
                 pressure[offset], pressure_2[offset]);
        }
      }
    }
  }
  printf("Success : wrong nums are %d\n", wrong_counter);
}
