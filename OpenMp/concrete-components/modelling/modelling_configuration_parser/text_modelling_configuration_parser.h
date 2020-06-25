//
// Created by amrnasr on 13/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_TEXTMODELLINGCONFIGURATIONPARSER_H
#define ACOUSTIC2ND_RTM_TEXTMODELLINGCONFIGURATIONPARSER_H

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <skeleton/components/modelling/modelling_configuration_parser.h>

class TextModellingConfigurationParser : public ModellingConfigurationParser {
private:
  AcousticOmpComputationParameters *parameters;

public:
  ~TextModellingConfigurationParser() override;

  ModellingConfiguration ParseConfiguration(string filepath,
                                            bool is_2D) override;

  void SetComputationParameters(ComputationParameters *parameters) override;

  void SetGridBox(GridBox *grid_box) override;
};
/**
 *
 * The file format should be as following :
 *
 * source=(x,y,z)
 * receiver_start=(x,y,z)
 * receiver_inc=(x,y,z)
 * receiver_end=(x,y,z)
 * simulation_time=time
 *
 * With x, y, z, value and time replaced with integer values indicating the
 * actual values for these parameters.
 */

#endif // ACOUSTIC2ND_RTM_TEXTMODELLINGCONFIGURATIONPARSER_H
