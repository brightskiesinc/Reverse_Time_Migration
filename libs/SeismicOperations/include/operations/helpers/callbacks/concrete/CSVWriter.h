//
// Created by amr-nasr on 31/12/2019.
//

#ifndef OPERATIONS_LIB_HELPERS_CALLBACKS_CSV_WRITER_H
#define OPERATIONS_LIB_HELPERS_CALLBACKS_CSV_WRITER_H

#include <operations/helpers/callbacks/interface/WriterCallback.h>
#include <string>

namespace operations {
    namespace helpers {
        namespace callbacks {

            class CsvWriter : public WriterCallback {
            public:
                CsvWriter(uint show_each, bool write_velocity, bool write_forward,
                          bool write_backward, bool write_reverse, bool write_migration,
                          bool write_re_extended_velocity, bool write_single_shot_correlation,
                          bool write_each_stacked_shot, bool write_traces_raw,
                          bool writer_traces_preprocessed,
                          const std::vector<std::string> &vec_params,
                          const std::vector<std::string> &vec_re_extended_params,
                          const std::string &write_path)
                        : WriterCallback(show_each, write_velocity, write_forward,
                                         write_backward, write_reverse, write_migration,
                                         write_re_extended_velocity, write_single_shot_correlation,
                                         write_each_stacked_shot,
                                         write_traces_raw, writer_traces_preprocessed,
                                         vec_params,
                                         vec_re_extended_params,
                                         write_path, "csv") {

                };

                std::string GetExtension() override;

                void WriteResult(uint nx, uint ny, uint nz, uint nt,
                                 float dx, float dy, float dz, float dt,
                                 float *data, std::string filename, bool is_traces) override;

            public:
                static void WriteCsv(float *mat, uint nz, uint nx, const std::string &filename,
                                     uint start_x, uint start_z, uint end_x, uint end_z);
            };
        } //namespace callbacks
    } //namespace operations
} //namespace operations

#endif // OPERATIONS_LIB_HELPERS_CALLBACKS_CSV_WRITER_H
