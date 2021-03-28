//
// Created by amr-nasr on 12/11/2019.
//

#include <operations/helpers/callbacks/concrete/BinaryWriter.h>

#include <operations/helpers/callbacks/interface/Extensions.hpp>
#include <operations/utils/io/write_utils.h>

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::utils::io;


string BinaryWriter::GetExtension() {
    return OP_K_EXT_BIN;
}

void BinaryWriter::WriteResult(uint nx, uint ny, uint nz, uint nt,
                               float dx, float dy, float dz, float dt, float *data, std::string filename,
                               bool is_traces) {
    write_binary(data, nx, nz, filename.c_str());
}