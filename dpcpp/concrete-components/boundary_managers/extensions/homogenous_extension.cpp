//
// Created by amr on 18/11/2019.
//

#include "homogenous_extension.h"
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>

using namespace cl::sycl;

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
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      auto global_range =
          range<3>(end_x - start_x, boundary_length, end_z - start_z);
      auto local_range = range<3>(1, 1, 1);
      auto global_nd_range = nd_range<3>(global_range, local_range);

      cgh.parallel_for<class Homogenous_velocity_extension_Y>(
          global_nd_range, [=](nd_item<3> it) {
            int column = it.get_global_id(0) + start_x;
            int depth = it.get_global_id(1);
            int row = it.get_global_id(2) + start_z;

            /*!for values from y = HALF_LENGTH TO y= HALF_LENGTH +BOUND_LENGTH*/
            int p_idx = (depth + start_y) * nz_nx + row * nx + column;
            int p2_idx =
                (boundary_length + start_y) * nz_nx + row * nx + column;
            property_array[p_idx] = property_array[p2_idx];

            /*!for values from y = ny-HALF_LENGTH TO y =
             * ny-HALF_LENGTH-BOUND_LENGTH*/
            p_idx = (end_y - 1 - depth) * nz_nx + row * nx + column;
            p2_idx = (end_y - 1 - boundary_length) * nz_nx + row * nx + column;
            property_array[p_idx] = property_array[p2_idx];
          });
    });
  }

  /*!putting the nearest property_array adjacent to the boundary as the value
   * for all velocities at the boundaries for x and with all z and y */
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range =
        range<3>(boundary_length, end_y - start_y, end_z - start_z);
    auto local_range = range<3>(1, 1, 1);
    auto global_nd_range = nd_range<3>(global_range, local_range);

    cgh.parallel_for<class Homogenous_velocity_extension_X>(
        global_nd_range, [=](nd_item<3> it) {
          int column = it.get_global_id(0);
          int depth = it.get_global_id(1) + start_y;
          int row = it.get_global_id(2) + start_z;

          /*!for values from x = HALF_LENGTH TO x= HALF_LENGTH +BOUND_LENGTH*/
          int p_idx = depth * nz_nx + row * nx + column + start_x;
          int p2_idx = depth * nz_nx + row * nx + boundary_length + start_x;
          property_array[p_idx] = property_array[p2_idx];

          /*!for values from x = nx-HALF_LENGTH TO x =
           * nx-HALF_LENGTH-BOUND_LENGTH*/
          p_idx = depth * nz_nx + row * nx + (end_x - 1 - column);
          p2_idx = depth * nz_nx + row * nx + (end_x - 1 - boundary_length);
          property_array[p_idx] = property_array[p2_idx];
        });
  });

  /*!putting the nearest property_array adjacent to the boundary as the value
   * for all velocities at the boundaries for z and with all x and y */
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range =
        range<3>(end_x - start_x, end_y - start_y, boundary_length);
    auto local_range = range<3>(1, 1, 1);
    auto global_nd_range = nd_range<3>(global_range, local_range);

    cgh.parallel_for<class Homogenous_velocity_extension_Z>(
        global_nd_range, [=](nd_item<3> it) {
          int column = it.get_global_id(0) + start_x;
          int depth = it.get_global_id(1) + start_y;
          int row = it.get_global_id(2);

          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          int p_idx = depth * nz_nx + (start_z + row) * nx + column;
          int p2_idx =
              depth * nz_nx + (start_z + boundary_length) * nx + column;
          property_array[p_idx] = property_array[p2_idx];

          /*!for values from z = nz-HALF_LENGTH TO z =
           * nz-HALF_LENGTH-BOUND_LENGTH*/
          p_idx = depth * nz_nx + (end_z - 1 - row) * nx + column;
          p2_idx = depth * nz_nx + (end_z - 1 - boundary_length) * nx + column;
          property_array[p_idx] = property_array[p2_idx];
        });
  });

  //    // Zero-Corners in the boundaries nx-nz boundary
  //    intersection--boundaries not needed. for (int depth = start_y; depth <
  //    end_y; depth++) {
  //        for (int row = 0; row < boundary_length; row++) {
  //            for (int column = 0; column < boundary_length; column++) {
  //                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH
  //                +boundary_length */
  //                /*! and for x = HALF_LENGTH to x = HALF_LENGTH +
  //                boundary_length */
  //                /*! Top left boundary in other words */
  //                property_array[depth * nz_nx + (start_z + row) * nx + column
  //                + start_x] = 0;
  //                /*!for values from z = nz-HALF_LENGTH TO z =
  //                nz-HALF_LENGTH-boundary_length*/
  //                /*! and for x = HALF_LENGTH to x = HALF_LENGTH +
  //                boundary_length */
  //                /*! Bottom left boundary in other words */
  //                property_array[depth * nz_nx + (end_z - 1 - row) * nx +
  //                column + start_x] = 0;
  //                /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH
  //                +boundary_length */
  //                /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH -
  //                boundary_length */
  //                /*! Top right boundary in other words */
  //                property_array[depth * nz_nx + (start_z + row) * nx + (end_x
  //                - 1 - column)] = 0;
  //                /*!for values from z = nz-HALF_LENGTH TO z =
  //                nz-HALF_LENGTH-boundary_length*/
  //                /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH -
  //                boundary_length */
  //                /*! Bottom right boundary in other words */
  //                property_array[depth * nz_nx + (end_z - 1 - row) * nx +
  //                (end_x - 1 - column)] = 0;
  //            }
  //        }
  //    }
  //    // If 3-D, zero corners in the y-x and y-z plans.
  //    if (ny > 1) {
  //        // Zero-Corners in the boundaries ny-nz boundary
  //        intersection--boundaries not needed. for (int depth = 0; depth <
  //        boundary_length; depth++) {
  //            for (int row = 0; row < boundary_length; row++) {
  //                for (int column = start_x; column < end_x; column++) {
  //                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH
  //                    +boundary_length */
  //                    /*! and for y = HALF_LENGTH to y = HALF_LENGTH +
  //                    boundary_length */ property_array[(depth + start_y) *
  //                    nz_nx + (start_z + row) * nx + column] = 0;
  //                    /*!for values from z = nz-HALF_LENGTH TO z =
  //                    nz-HALF_LENGTH-boundary_length*/
  //                    /*! and for y = HALF_LENGTH to y = HALF_LENGTH +
  //                    boundary_length */ property_array[(depth + start_y) *
  //                    nz_nx + (end_z - 1 - row) * nx + column] = 0;
  //                    /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH
  //                    +boundary_length */
  //                    /*! and for y = ny-HALF_LENGTH to y = ny-HALF_LENGTH -
  //                    boundary_length */ property_array[(end_y - 1 - depth) *
  //                    nz_nx + (start_z + row) * nx + column] = 0;
  //                    /*!for values from z = nz-HALF_LENGTH TO z =
  //                    nz-HALF_LENGTH-boundary_length */
  //                    /*! and for y = ny-HALF_LENGTH to y = ny - HALF_LENGTH -
  //                    boundary_length */ property_array[(end_y - 1 - depth) *
  //                    nz_nx + (end_z - 1 - row) * nx + column] = 0;
  //                }
  //            }
  //        }
  //        // Zero-Corners in the boundaries nx-ny boundary
  //        intersection--boundaries not needed. for (int depth = 0; depth <
  //        boundary_length; depth++) {
  //            for (int row = start_z; row < end_z; row++) {
  //                for (int column = 0; column < boundary_length; column++) {
  //                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH
  //                    +boundary_length */
  //                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH +
  //                    boundary_length */ property_array[(depth + start_y) *
  //                    nz_nx + row * nx + column + start_x] = 0;
  //                    /*!for values from y = ny-HALF_LENGTH TO y =
  //                    ny-HALF_LENGTH-boundary_length*/
  //                    /*! and for x = HALF_LENGTH to x = HALF_LENGTH +
  //                    boundary_length */ property_array[(end_y - 1 - depth) *
  //                    nz_nx + row * nx + column + start_x] = 0;
  //                    /*!for values from y = HALF_LENGTH TO y = HALF_LENGTH
  //                    +boundary_length */
  //                    /*! and for x = nx-HALF_LENGTH to x = nx-HALF_LENGTH -
  //                    boundary_length */ property_array[(depth + start_y) *
  //                    nz_nx + row * nx + (end_x - 1 - column)] = 0;
  //                    /*!for values from y = ny-HALF_LENGTH TO y =
  //                    ny-HALF_LENGTH-boundary_length*/
  //                    /*! and for x = nx-HALF_LENGTH to x = nx - HALF_LENGTH -
  //                    boundary_length */ property_array[(end_y - 1 - depth) *
  //                    nz_nx + row * nx + (end_x - 1 - column)] = 0;
  //                }
  //            }
  //        }
  //    }
}

void HomogenousExtension::top_layer_extension_helper(
    float *property_array, int start_x, int start_z, int start_y, int end_x,
    int end_y, int end_z, int nx, int nz, int ny, uint boundary_length) {
  int nz_nx = nx * nz;

  /*!putting the nearest property_array adjacent to the boundary as the value
   * for all velocities at the boundaries for z and with all x and y */
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range =
        range<3>(end_x - start_x, start_y - end_y, boundary_length);
    auto local_range = range<3>(1, 1, 1);
    auto global_nd_range = nd_range<3>(global_range, local_range);

    cgh.parallel_for<class Homogenous_top_extension>(
        global_nd_range, [=](nd_item<3> it) {
          int column = it.get_global_id(0) + start_x;
          int depth = it.get_global_id(1) + start_y;
          int row = it.get_global_id(2);

          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          int p_idx = depth * nz_nx + (start_z + row) * nx + column;
          int p2_idx =
              depth * nz_nx + (start_z + boundary_length) * nx + column;
          property_array[p_idx] = property_array[p2_idx];
        });
  });
}

void HomogenousExtension::top_layer_remover_helper(
    float *property_array, int start_x, int start_z, int start_y, int end_x,
    int end_y, int end_z, int nx, int nz, int ny, uint boundary_length) {
  int nz_nx = nx * nz;
  /*!putting the nearest property_array adjacent to the boundary as the value
   * for all velocities at the boundaries for z and with all x and y */
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    auto global_range =
        range<3>(end_x - start_x, start_y - end_y, boundary_length);
    auto local_range = range<3>(1, 1, 1);
    auto global_nd_range = nd_range<3>(global_range, local_range);

    cgh.parallel_for<class Homogenous_top_remover>(
        global_nd_range, [=](nd_item<3> it) {
          int column = it.get_global_id(0) + start_x;
          int depth = it.get_global_id(1) + start_y;
          int row = it.get_global_id(2);

          /*!for values from z = HALF_LENGTH TO z = HALF_LENGTH +BOUND_LENGTH */
          int p_idx = depth * nz_nx + (start_z + row) * nx + column;
          property_array[p_idx] = 0;
        });
  });
}
