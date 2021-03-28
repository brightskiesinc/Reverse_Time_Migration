//
// Created by amr-nasr on 07/06/2020.
//

#ifndef OPERATIONS_LIB_UTILS_WRITE_UTILS_H
#define OPERATIONS_LIB_UTILS_WRITE_UTILS_H

#include <operations/common/DataTypes.h>
#include <string>

namespace operations {
    namespace utils {
        namespace io {

            void write_adcig_segy(uint nx, uint ny, uint nz, uint nt,
                                  uint n_angles,
                                  float dx, float dy, float dz, float dt,
                                  const float *data,
                                  const std::string &file_name, bool is_traces);

            void write_segy(uint nx, uint ny, uint nz, uint nt,
                            float dx, float dy, float dz, float dt,
                            const float *data, const std::string &file_name, bool is_traces);

            void write_su(const float *temp, uint nx, uint nz,
                          const char *file_name, bool write_little_endian);

            void write_binary(float *temp, uint nx, uint nz, const char *file_name);
        } //namespace io
    } //namespace utils
} //namespace operations

#endif // OPERATIONS_LIB_UTILS_WRITE_UTILS_H
