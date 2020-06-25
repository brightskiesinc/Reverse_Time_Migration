//
// Created by amr on 09/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_STAGGERED_GRID_H
#define ACOUSTIC2ND_RTM_STAGGERED_GRID_H

#include <skeleton/base/datatypes.h>

class StaggeredGrid : public GridBox {
public:
  float *particle_velocity_x_current;
  float *particle_velocity_y_current;
  float *particle_velocity_z_current;
  float *pressure_next;
  float *density;
};
#endif // ACOUSTIC2ND_RTM_STAGGERED_GRID_H
