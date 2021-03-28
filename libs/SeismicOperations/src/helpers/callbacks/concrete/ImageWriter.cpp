//
// Created by amr-nasr on 12/11/2019.
//

#include <operations/helpers/callbacks/concrete/ImageWriter.h>

#include <operations/helpers/callbacks/interface/Extensions.hpp>

#include <visualization/visualization.h>

using namespace std;
using namespace operations::helpers::callbacks;


string ImageWriter::GetExtension() {
    return OP_K_EXT_IMG;
}

void ImageWriter::WriteResult(uint nx, uint ny, uint nz, uint nt,
                              float dx, float dy, float dz, float dt, float *data, std::string filename,
                              bool is_traces) {
    GridToPNG(data + (ny / 2) * nx * nz, nz, nx, mPercentile, (char *) filename.c_str());
}