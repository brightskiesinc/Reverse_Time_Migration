//
// Created by amr-nasr on 12/11/2019.
//

#include <operations/helpers/callbacks/concrete/SUWriter.h>

#include <operations/helpers/callbacks/interface/Extensions.hpp>
#include <operations/utils/io/write_utils.h>

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::utils::io;


string SuWriter::GetExtension() {
    return OP_K_EXT_SU;
}

void SuWriter::WriteResult(uint nx, uint ny, uint nz, uint nt,
                           float dx, float dy, float dz, float dt, float *data, std::string filename, bool is_traces) {
    write_su(data, nx, nz, filename.c_str(), mIsWriteLittleEndian);
}