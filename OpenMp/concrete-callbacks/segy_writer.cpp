#include "segy_writer.h"


string SegyWriter::GetExtension() {
    return ".segy";
}

void SegyWriter::WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                 float dy, float *data, std::string filename, bool is_traces) {
    WriteSegy(nx, nz, nt, ny, dx, dz, dt, dy, data, filename, is_traces);
}