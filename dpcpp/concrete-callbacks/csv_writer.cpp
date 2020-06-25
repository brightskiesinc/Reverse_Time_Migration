//
// Created by amr on 31/12/2019.
//

#include "csv_writer.h"
#include <fstream>

void CsvWriter::WriteCsv(float *mat, uint nz, uint nx, string filename,
                         uint start_x, uint start_z, uint end_x, uint end_z) {
    std::ofstream out(filename);
    out << (end_x - start_x) << "," << (end_z - start_z) << "\n";
    for (uint row = start_z; row < end_z; row++) {
        for (uint col = start_x; col < end_x; col++)
            out << mat[row * nx + col] << ',';
        out << '\n';
    }
}

string CsvWriter::GetExtension() {
    return ".csv";
}

void CsvWriter::WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
                             float dy, float *data, std::string filename, bool is_traces) {
    WriteCsv(data + (ny / 2) * nx * nz, nz, nx, filename, 0, 0, nx, nz);
}



