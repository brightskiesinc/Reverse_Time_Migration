//
// Created by amr on 24/06/2020.
//

#ifndef ACOUSTIC2ND_RTM_NOISE_FILTERING_H
#define ACOUSTIC2ND_RTM_NOISE_FILTERING_H

void filter_stacked_correlation(float *input_frame, float *output_frame,
        unsigned int nx, unsigned int nz, unsigned int ny,
        float dx, float dz, float dy);

#endif //ACOUSTIC2ND_RTM_NOISE_FILTERING_H
