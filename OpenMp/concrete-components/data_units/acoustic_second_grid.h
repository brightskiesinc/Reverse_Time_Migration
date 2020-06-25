//
// Created by amr on 07/12/2019.
//

#ifndef ACOUSTIC2ND_RTM_ACOUSTIC_SECOND_GRID_H
#define ACOUSTIC2ND_RTM_ACOUSTIC_SECOND_GRID_H

#include <skeleton/base/datatypes.h>

class AcousticSecondGrid : public GridBox {
public:
  float *pressure_previous;
  float *pressure_next;
};

#endif // ACOUSTIC2ND_RTM_ACOUSTIC_SECOND_GRID_H
