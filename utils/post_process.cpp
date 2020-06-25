//
// Created by amr on 24/06/2020.
//
#include "write_utils.h"
#include <IO/io_manager.h>
#include <Segy/segy_io_manager.h>
#include "noise_filtering.h"

int main(int argc, char *argv[]) {
    string file1;
    string file2;
    if (argc == 3) {
        file1 = string(argv[1]);
        file2 = string(argv[2]);
        cout << "Input segy file to be processed : " << file1 << std::endl;
        cout << "Output segy file to write : " << file2 << std::endl;
    } else {
        cout << "Invalid arguments, must provide two arguments(first is input file, second is output file" << std::endl;
        exit(0);
    }
    // Read data.
    SeIO *sio = new SeIO();
    IOManager *IO = new SEGYIOManager();
    IO->ReadVelocityDataFromFile(file1, "CSR", sio);
    uint nx = sio->DM.nx;
    uint nz = sio->DM.nz;
    uint ny = sio->DM.ny;
    float dx = sio->DM.dx;
    float dz = sio->DM.dz;
    float dy = sio->DM.dy;
    float * data = new float[nx * nz * ny];
    for (unsigned int k = 0; k < ny; k++) {
        for (unsigned int i = 0; i < nx; i++) {
            for (unsigned int j = 0; j < nz; j++) {
                data[k * nx * nz + j * nx + i] =
                        sio->Velocity.at(i).TraceData[j];
            }
        }
    }
    // Do Filtering.
    float *processed_correlation = new float[nx * nz * ny];
    filter_stacked_correlation(data, processed_correlation, nx, nz, ny,
            dx, dz, dy);
    // Write data.
    WriteSegy(nx, nz, 1, ny, dx, dz, 1, dy, processed_correlation, file2, false);
}
