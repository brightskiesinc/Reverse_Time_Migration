//
// Created by amr-nasr on 12/11/2019.
//

#include <operations/helpers/callbacks/concrete/SegyWriter.h>

#include <operations/helpers/callbacks/interface/Extensions.hpp>
#include <operations/utils/io/write_utils.h>

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::utils::io;


string SegyWriter::GetExtension() {
    return OP_K_EXT_SGY;
}

void SegyWriter::WriteResult(uint nx, uint ny, uint nz, uint nt,
                             float dx, float dy, float dz, float dt,
                             float *data, std::string filename, bool is_traces) {
    write_segy(nx, ny, nz, nt, dx, dy, dz, dt, data, filename, is_traces);
}