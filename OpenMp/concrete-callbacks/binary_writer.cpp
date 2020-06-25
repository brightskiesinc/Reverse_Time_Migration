#include "binary_writer.h"

string BinaryWriter::GetExtension() {
    return ".bin";
}

void BinaryWriter::WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                 float dy, float *data, std::string filename, bool is_traces) {
    WriteBinary(data, nx, nz, filename.c_str());
}