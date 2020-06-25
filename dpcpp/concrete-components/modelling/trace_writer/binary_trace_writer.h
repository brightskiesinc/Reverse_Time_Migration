//
// Created by amrnasr on 13/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_BINARYTRACEWRITER_H
#define ACOUSTIC2ND_RTM_BINARYTRACEWRITER_H

#include <skeleton/components/modelling/trace_writer.h>

#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <concrete-components/data_units/acoustic_second_grid.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class BinaryTraceWriter : public TraceWriter {
private:
  ofstream *output_file;
  Point3D r_start;
  Point3D r_inc;
  Point3D r_end;
  AcousticSecondGrid *grid;
  AcousticDpcComputationParameters *parameters;

public:
  BinaryTraceWriter();

  ~BinaryTraceWriter() override;

  void InitializeWriter(ModellingConfiguration *modelling_config,
                        string output_filename) override;

  void RecordTrace() override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};

#endif // ACOUSTIC2ND_RTM_BINARYTRACEWRITER_H
