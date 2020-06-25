//
// Created by amr on 18/11/2019.
//

#include "homogenous_extension.h"

HomogenousExtension::HomogenousExtension(bool use_top_layer) {
  this->use_top = use_top_layer;
}
void HomogenousExtension::velocity_extension_helper(
    float *property_array, int start_x, int start_z, int start_y, int end_x,
    int end_y, int end_z, int nx, int nz, int ny, uint boundary_length) {
  /*!
   * change the values of velocities at boundaries (HALF_LENGTH excluded) to
   * zeros the start for x , y and z is at HALF_LENGTH and the end is at (nx -
   * HALF_LENGTH) or (ny - HALF_LENGTH) or (nz- HALF_LENGTH)
   */
  int nz_nx = nx * nz;
  // In case of 2D
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  } else {
    // general case for 3D
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for y and with all x and z */
    for (int depth = 0; depth < boundary_length; depth++) {
      for (int row = start_z; row < end_z; row++) {
        for (int column = start_x; column < end_x; column++) {
          /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH +BOUND_LENGTH*/
          property_array[(depth + start_y) * nz_nx + row * nx + column] =
              property_array[(boundary_length + start_y) * nz_nx + row * nx +
                             column];
          /*!for values from y = ny-HALF_LENGTH TO y =
           * ny-HALF_LENGTH-BOUND_LENGTH*/
          property_array[(end_y - 1 - depth) * nz_nx + row * nx + column] =
              property_array[(end_y - 1 - boundary_length) * nz_nx + row * nx +
                             column];
        }
      }
    }
  }
  /*!putting the nearest property_array adjacent to the boundary as the value
   * for all velocities at the boundaries for x and with all z and y */
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = start_z; row < end_z; row++) {
      for (int column = 0; column < boundary_length; column++) {
        /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
        property_array[depth * nz_nx + row * nx + column + start_x] =
            property_array[depth * nz_nx + row * nx + boundary_length +
                           start_x];
        /*!for values from x = nx-HALF_LENGTH TO x =
         * nx-HALF_LENGTH-BOUND_LENGTH*/
        property_array[depth * nz_nx + row * nx + (end_x - 1 - column)] =
            property_array[depth * nz_nx + row * nx +
                           (end_x - 1 - boundary_length)];
      }
    }
  }
  if (this->use_top) {
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for z and with all x and y */
    for (int depth = start_y; depth < end_y; depth++) {
      for (int row = 0; row < boundary_length; row++) {
        for (int column = start_x; column < end_x; column++) {
          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          property_array[depth * nz_nx + (start_z + row) * nx + column] =
              property_array[depth * nz_nx + (start_z + boundary_length) * nx +
                             column];
          /*!for values from z = nz-HALF_LENGTH TO z =
           * nz-HALF_LENGTH-BOUND_LENGTH*/
          property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
              property_array[depth * nz_nx +
                             (end_z - 1 - boundary_length) * nx + column];
        }
      }
    }
  } else {
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for z and with all x and y */
    for (int depth = start_y; depth < end_y; depth++) {
      for (int row = 0; row < boundary_length; row++) {
        for (int column = start_x; column < end_x; column++) {
          /*!for values from z = nz-HALF_LENGTH TO z =
           * nz-HALF_LENGTH-BOUND_LENGTH*/
          property_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
              property_array[depth * nz_nx +
                             (end_z - 1 - boundary_length) * nx + column];
        }
      }
    }
  }
}

void HomogenousExtension::top_layer_extension_helper(
    float *property_array, int start_x, int start_z, int start_y, int end_x,
    int end_y, int end_z, int nx, int nz, int ny, uint boundary_length) {
  if (this->use_top) {
    int nz_nx = nx * nz;
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for z and with all x and y */
    for (int depth = start_y; depth < end_y; depth++) {
      for (int row = 0; row < boundary_length; row++) {
        for (int column = start_x; column < end_x; column++) {
          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          property_array[depth * nz_nx + (start_z + row) * nx + column] =
              property_array[depth * nz_nx + (start_z + boundary_length) * nx +
                             column];
        }
      }
    }
  }
}

void HomogenousExtension::top_layer_remover_helper(
    float *property_array, int start_x, int start_z, int start_y, int end_x,
    int end_y, int end_z, int nx, int nz, int ny, uint boundary_length) {
  if (this->use_top) {
    int nz_nx = nx * nz;
    /*!putting the nearest property_array adjacent to the boundary as the value
     * for all velocities at the boundaries for z and with all x and y */
    for (int depth = start_y; depth < end_y; depth++) {
      for (int row = 0; row < boundary_length; row++) {
        for (int column = start_x; column < end_x; column++) {
          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          property_array[depth * nz_nx + (start_z + row) * nx + column] = 0;
        }
      }
    }
  }
}
