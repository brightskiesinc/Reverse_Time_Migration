//
// Created by amr-nasr on 31/12/2019.
//

#include <operations/helpers/callbacks/concrete/CSVWriter.h>

#include <operations/helpers/callbacks/interface/Extensions.hpp>
#include <operations/utils/io/write_utils.h>

#include <fstream>

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::utils::io;


string CsvWriter::GetExtension() {
    return OP_K_EXT_CSV;
}

void CsvWriter::WriteCsv(float *mat, uint nz, uint nx, const string &filename,
                         uint start_x, uint start_z, uint end_x, uint end_z) {
    std::ofstream out(filename);
    out << (end_x - start_x) << "," << (end_z - start_z) << "\n";
    for (uint row = start_z; row < end_z; row++) {
        for (uint col = start_x; col < end_x; col++)
            out << mat[row * nx + col] << ',';
        out << '\n';
    }
}

void CsvWriter::WriteResult(uint nx, uint ny, uint nz, uint nt,
                            float dx, float dy, float dz, float dt, float *data, std::string filename, bool is_traces) {
    WriteCsv(data + (ny / 2) * nx * nz, nz, nx, filename, 0, 0, nx, nz);
}



