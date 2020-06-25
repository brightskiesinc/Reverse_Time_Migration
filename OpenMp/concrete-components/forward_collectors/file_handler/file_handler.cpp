//
// Created by mirnamoawad on 1/16/20.
//
#include "file_handler.h"

void bin_file_save(const char *fname, const float *data, const size_t size) {
  std::ofstream myfile(fname, std::ios::out | std::ios::binary);
  if (!myfile.is_open()) {
    exit(EXIT_FAILURE);
  }
  myfile.write(reinterpret_cast<const char *>(data), size * sizeof(float));
  myfile.close();
}

void bin_file_load(const char *fname, float *data, const size_t size) {
  std::ifstream is(fname, std::ios::binary | std::ios::in);
  if (!is.is_open()) {
    exit(EXIT_FAILURE);
  }
  is.read(reinterpret_cast<char *>(data),
          std::streamsize(size * sizeof(float)));
  is.close();
}