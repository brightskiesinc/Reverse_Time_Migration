//
// Created by mirnamoawad on 1/20/20.
//

#ifndef ACOUSTIC2ND_RTM_BOUNDARY_SAVER_H
#define ACOUSTIC2ND_RTM_BOUNDARY_SAVER_H

void SaveBoundaries(GridBox *main_grid, ComputationParameters *parameters,
                    float *backup_boundaries, uint step, uint boundarysize);
void RestoreBoundaries(GridBox *main_grid, GridBox *internal_grid,
                       ComputationParameters *parameters,
                       float *backup_boundaries, uint step, uint boundarysize);

#endif // ACOUSTIC2ND_RTM_BOUNDARY_SAVER_H
