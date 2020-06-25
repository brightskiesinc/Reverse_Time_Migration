//
// Created by amr on 07/06/2020.
//
#include "write_utils.h"

#include <IO/io_manager.h>
#include <Segy/segy_io_manager.h>
#include <seismic-io-framework/datatypes.h>
#include <string>

using namespace std;

void WriteSegy(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
               float dy, float *data, string filename, bool is_traces) {

  SEGYIOManager *IO = new SEGYIOManager();

  SeIO *sio = new SeIO();

  sio->DM.nt = nt;
  sio->DM.dt = dt;
  sio->DM.nx = nx;
  sio->DM.nz = nz;
  sio->DM.ny = ny;
  sio->DM.dx = dx;
  sio->DM.dz = dz;
  sio->DM.dy = dy;

  if (!is_traces) {
    for (int y = 0; y < ny; y++) {

      for (int x = 0; x < nx; x++) {

        GeneralTraces GT;
        sio->Velocity.push_back(GT);
        int id = y * nx + x;

        sio->Velocity[x].TraceMetaData.shot_id = id;
        sio->Velocity[x].TraceMetaData.receiver_location_x = x * dx;
        sio->Velocity[x].TraceMetaData.ensembleType =
            CSP; // for now it shoulld  be modified to translate the numbers of
                 // ensemble number
        sio->Velocity[x].TraceMetaData.source_location_x = x * dx;
        sio->Velocity[x].TraceMetaData.source_location_y = y * dy;
        sio->Velocity[x].TraceMetaData.source_location_z = 0;
        sio->Velocity[x].TraceMetaData.receiver_location_y = y * dy;
        sio->Velocity[x].TraceMetaData.receiver_location_y = y * dy;
        sio->Velocity[x].TraceMetaData.receiver_location_z = 0;
        sio->Velocity[x].TraceMetaData.trace_id_within_line = id;
        sio->Velocity[x].TraceMetaData.trace_id_within_file = id;
        sio->Velocity[x].TraceMetaData.trace_id_for_shot = 1;
        sio->Velocity[x].TraceMetaData.trace_id_for_ensemble = id;
        sio->Velocity[x].TraceMetaData.trace_identification_code = id;
        sio->Velocity[x].TraceMetaData.scalar = 10;

        for (int z = 0; z < nz; z++) {
          sio->Velocity[x].TraceData[z] = data[y * nx * nz + z * nx + x];
        }
      }
    }
    IO->WriteVelocityDataToFile(filename, "CSR", sio);
  } else {

    for (int y = 0; y < ny; y++) {
      for (int x = 0; x < nx; x++) {

        GeneralTraces GT;
        sio->Atraces.push_back(GT);
        int id = y * nx + x;

        sio->Atraces[x].TraceMetaData.shot_id = id;
        sio->Atraces[x].TraceMetaData.receiver_location_x = x * dx;
        sio->Atraces[x].TraceMetaData.ensembleType =
            CSP; // for now it shoulld  be modified to translate the numbers of
                 // ensemble number
        sio->Atraces[x].TraceMetaData.source_location_x = x * dx;
        sio->Atraces[x].TraceMetaData.source_location_y = y * dy;
        sio->Atraces[x].TraceMetaData.source_location_z = 0;
        sio->Atraces[x].TraceMetaData.receiver_location_y = y * dy;
        sio->Atraces[x].TraceMetaData.receiver_location_y = y * dy;
        sio->Atraces[x].TraceMetaData.receiver_location_z = 0;
        sio->Atraces[x].TraceMetaData.trace_id_within_line = id;
        sio->Atraces[x].TraceMetaData.trace_id_within_file = id;
        sio->Atraces[x].TraceMetaData.trace_id_for_shot = 1;
        sio->Atraces[x].TraceMetaData.trace_id_for_ensemble = id;
        sio->Atraces[x].TraceMetaData.trace_identification_code = id;
        sio->Atraces[x].TraceMetaData.scalar = 10;

        for (int z = 0; z < nz; z++) {
          sio->Atraces[x].TraceData[z] = data[y * nx * nz + z * nx + x];
        }
      }
    }
    IO->WriteTracesDataToFile(filename, "CSR", sio);
  }

  delete sio;
  delete IO;
}

void WriteSU(float *temp, int nx, int nz, const char *name, bool write_little_endian) {
  std::ofstream myfile(name, std::ios::out | std::ios::binary);
  if (!myfile.is_open()) {
    exit(EXIT_FAILURE);
  }
  char dummy_header[240];
  memset(dummy_header, 0, 240);
  unsigned short tid = 1;
  unsigned short ns = nz;
  bool little_endian = false;
  // little endian if true
  if (*(char *)&tid == 1) {
    little_endian = true;
  }
  bool swap_bytes = true;
  if ((little_endian && write_little_endian) || (!little_endian && !write_little_endian)) {
      swap_bytes = false;
  }
  for (int i = 0; i < nx; i++) {
    myfile.write(dummy_header, 14);
    if (swap_bytes) {
      myfile.write(((char *)&tid) + 1, 1);
      myfile.write((char *)(&tid), 1);
    } else {
      myfile.write((char *)&tid, 2);
    }
    myfile.write(dummy_header, 98);
    if (swap_bytes) {
      myfile.write(((char *)&ns) + 1, 1);
      myfile.write((char *)(&ns + 0), 1);
    } else {
      myfile.write((char *)&ns, 2);
    }
    myfile.write(dummy_header, 124);
    for (int j = 0; j < nz; j++) {
      float val = temp[i + j * nx];
      if (swap_bytes) {
        myfile.write(((char *)&val) + 3, 1);
        myfile.write(((char *)&val) + 2, 1);
        myfile.write(((char *)&val) + 1, 1);
        myfile.write(((char *)&val) + 0, 1);
      } else {
        myfile.write((char *)&val, 4);
      }
    }
  }
  myfile.close();
}

void WriteBinary(float *temp, int nx, int nz, const char *name) {
    std::ofstream myfile(name, std::ios::out | std::ios::binary);
    if (!myfile.is_open()) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < nz; j++) {
            myfile.write(reinterpret_cast<const char *>(temp + j * nx + i), sizeof(float));
        }
    }
    myfile.close();
}