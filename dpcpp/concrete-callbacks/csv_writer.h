//
// Created by amr on 31/12/2019.
//

#ifndef ACOUSTIC2ND_RTM_CSV_WRITER_H
#define ACOUSTIC2ND_RTM_CSV_WRITER_H

#include "writer_callback.h"
#include <string>

using namespace std;

class CsvWriter : public WriterCallback {
public:
  static void WriteCsv(float *mat, uint nz, uint nx, string filename,
                       uint start_x, uint start_z, uint end_x, uint end_z);
  CsvWriter(uint show_each, bool write_velocity, bool write_forward,
            bool write_backward, bool write_reverse, bool write_migration,
            bool write_re_extended_velocity, bool write_single_shot_correlation,
            bool write_each_stacked_shot, bool write_traces_raw,
            bool writer_traces_preprocessed, string write_path)  :
            WriterCallback(show_each, write_velocity, write_forward,
                  write_backward, write_reverse, write_migration,
                  write_re_extended_velocity, write_single_shot_correlation, write_each_stacked_shot,
                  write_traces_raw, writer_traces_preprocessed,
                  write_path, "csv") {

  };


  string GetExtension() override;

  void WriteResult(uint nx, uint nz, uint nt, uint ny, float dx, float dz, float dt,
          float dy, float *data, std::string filename, bool is_traces) override ;

};

#endif // ACOUSTIC2ND_RTM_CSV_WRITER_H
