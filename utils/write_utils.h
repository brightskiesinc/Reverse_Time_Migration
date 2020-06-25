//
// Created by amr on 07/06/2020.
//

#ifndef ACOUSTIC2ND_RTM_WRITE_UTILS_H
#define ACOUSTIC2ND_RTM_WRITE_UTILS_H

#include <rtm-framework/skeleton/base/datatypes.h>
#include <string>

void WriteSegy(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
               float dy, float *data, std::string filename, bool is_traces);

void WriteSU(float *temp, int nx, int nz, const char *name, bool write_little_endian);

void WriteBinary(float *temp, int nx, int nz, const char *name);

#endif // ACOUSTIC2ND_RTM_WRITE_UTILS_H
