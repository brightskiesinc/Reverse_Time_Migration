//
// Created by amr on 19/11/2019.
//

#ifndef ACOUSTIC2ND_RTM_HOMOGENOUS_EXTENSION_H
#define ACOUSTIC2ND_RTM_HOMOGENOUS_EXTENSION_H

#include "extension.h"

class HomogenousExtension : public Extension {
public:
  HomogenousExtension(bool use_top_layer = true);

private:
  bool use_top;
  void velocity_extension_helper(float *property_array, int start_x,
                                 int start_z, int start_y, int end_x, int end_y,
                                 int end_z, int nx, int nz, int ny,
                                 uint boundary_length) override;

  void top_layer_extension_helper(float *property_array, int start_x,
                                  int start_z, int start_y, int end_x,
                                  int end_y, int end_z, int nx, int nz, int ny,
                                  uint boundary_length) override;

  void top_layer_remover_helper(float *property_array, int start_x, int start_z,
                                int start_y, int end_x, int end_y, int end_z,
                                int nx, int nz, int ny,
                                uint boundary_length) override;
};

#endif // ACOUSTIC2ND_RTM_HOMOGENOUS_EXTENSION_H
