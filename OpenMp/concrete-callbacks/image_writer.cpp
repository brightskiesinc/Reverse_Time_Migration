//
// Created by amrnasr on 12/11/2019.
//
#include "image_writer.h"

string ImageWriter::GetExtension() {
    return ".png";
}

void ImageWriter::WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                             float dy, float *data, std::string filename, bool is_traces) {
    GridToPNG(data + (ny / 2) * nx * nz, nz, nx, percentile, (char *)filename.c_str());
}