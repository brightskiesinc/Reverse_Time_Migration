#include "su_writer.h"

string SuWriter::GetExtension() {
    return ".su";
}

void SuWriter::WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                 float dy, float *data, std::string filename, bool is_traces) {
    WriteSU(data, nx, nz, filename.c_str(), write_little_endian);
}