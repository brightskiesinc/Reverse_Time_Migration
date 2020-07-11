//
// Created by amr on 04/07/2020.
//

#ifndef ACOUSTIC2ND_RTM_READ_UTILS_H
#define ACOUSTIC2ND_RTM_READ_UTILS_H

#include <rtm-framework/skeleton/base/datatypes.h>
#include <seismic-io-framework/datatypes.h>

void ParseSeioToTraces(SeIO *sio, Point3D *source, Traces *traces, uint **x_position, uint **y_position,
        GridBox *grid, ComputationParameters *parameters, float *total_time);

#endif //ACOUSTIC2ND_RTM_READ_UTILS_H
