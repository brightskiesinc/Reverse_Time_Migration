//
// Created by amr on 18/11/2019.
//
#include "extension.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

// Constructor.
Extension::Extension() {
  this->backup_array = nullptr;
  this->start_point = {0, 0, 0};
  this->end_point = {0, 0, 0};
}

// Sets the half length padding used in the extensions operations.
void Extension::SetHalfLength(uint half_length) {
  this->half_length = half_length;
}

// Sets the boundary length used in the extensions operations.
void Extension::SetBoundaryLength(uint bound_length) {
  this->boundary_length = bound_length;
}

// Sets the gridbox that the operations are ran on.
void Extension::SetGridBox(GridBox *grid) { this->grid = grid; }

// Sets the property that will be extended by this extensions object.
void Extension::SetProperty(float *property) {
  this->property_array = property;
}

// Save the original_array values at the boundaries to backup_array.
void Extension::SaveBoundary(float *original_array, int nx, int nz, int ny) {
  /*! reading the values of the start and end of the window in all dimensions
   *
   */
  int start_x = this->start_point.x;
  int start_y = this->start_point.y;
  int start_z = this->start_point.z;
  int end_x = this->end_point.x;
  int end_y = this->end_point.y;
  int end_z = this->end_point.z;
  // it is nullptr in the first shot
  if (this->backup_array == nullptr) {
    int wnx = end_x - start_x;
    int wny = end_y - start_y;
    int wnz = end_z - start_z;
    int size_of_backup = 0;
    if (ny == 1) {
      wny = 1;
    } else {
      /*!the boundaries area in y direction is rectangular of x and z covering
       * only the BOUND_LENGTH area for  y direction the 2(both sides)
       */
      size_of_backup = 2 * boundary_length * (wnx * wnz);
    }
    /*!increment the boundaries area with the sum of areas of the
     * rectangles of z and y covering only the BOUND_LENGTH area for  x
     * direction rectangles of x and y covering only the BOUND_LENGTH area for
     * z direction
     */
    size_of_backup += 2 * boundary_length * (wnz * wny + wnx * wny);
    // allocate memory for the backup_array with the size of the boundaries area
    this->backup_array = (float *)mem_allocate(sizeof(float), size_of_backup,
                                               "Boundary backup values");
  }
  // the general case for the upcoming shots
  int nz_nx = nz * nx;
  int index = 0;
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  } else {
    /*! in back_vel we store the velocities' old values before changing them to
     * zeros by this order: first we store the values for y from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH  for all x and z  in backup_array then the
     * values for y from ny-HALF_LENGTH to ny -HALF_LENGTH - BOUND_LENGTH for
     * all x and z  in backup_array then the values for x from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH for all y and z in backup_array then the values
     * for  x from nx-HALF_LENGTH to nx -HALF_LENGTH - BOUND_LENGTH for all y
     * and z in backup_array then the values for z from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH  for all x and y in backup_array then the
     * values for  z from nz-HALF_LENGTH to nz -HALF_LENGTH - BOUND_LENGTH for
     * all x and y in backup_array
     */

    // save the values fory boundaries for all x and z
    for (int depth = 0; depth < boundary_length; depth++) {
      for (int row = start_z; row < end_z; row++) {
        for (int column = start_x; column < end_x; column++) {
          // we store the values for y from HALF_LENGTH to boundary_length
          // +HALF_LENGTH  for all x and z  in backup_array
          this->backup_array[index] =
              original_array[(depth + start_y) * nz_nx + row * nx + column];
          index++;
          // the values for y from ny-HALF_LENGTH to ny -HALF_LENGTH -
          // boundary_length for all x and z  in backup_array
          this->backup_array[index] =
              original_array[(end_y - 1 - depth) * nz_nx + row * nx + column];
          index++;
        }
      }
    }
  }
  // save the values for x boundaries for all y and z
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = start_z; row < end_z; row++) {
      for (int column = 0; column < boundary_length; column++) {
        // for x from HALF_LENGTH to boundary_length +HALF_LENGTH for all y and
        // z in backup_array
        this->backup_array[index] =
            original_array[depth * nz_nx + row * nx + column + start_x];
        index++;
        // for  x from nx-HALF_LENGTH to nx -HALF_LENGTH - boundary_length for
        // all y and z in backup_array
        this->backup_array[index] =
            original_array[depth * nz_nx + row * nx + (end_x - 1 - column)];
        index++;
      }
    }
  }
  // save the values for z boundaries for all x and y
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = 0; row < boundary_length; row++) {
      for (int column = start_x; column < end_x; column++) {
        // for z from HALF_LENGTH to boundary_length +HALF_LENGTH  for all x and
        // y in backup_array
        this->backup_array[index] =
            original_array[depth * nz_nx + (start_z + row) * nx + column];
        index++;
        // for  z from nz-HALF_LENGTH to nz -HALF_LENGTH - boundary_length for
        // all x and y in backup_array
        this->backup_array[index] =
            original_array[depth * nz_nx + (end_z - 1 - row) * nx + column];
        index++;
      }
    }
  }
}
// Restore the original_array values from backup_array at the boundaries
// of the last used window to its original location.
/*!restore the old values  of velocities that were changed  due to the last used
 * window from back_vel to the place it was in before the window
 * @param original_array
 * @param nx
 * @param nz
 * @param ny
 */
void Extension::RestoreBoundary(float *original_array, int nx, int nz, int ny) {
  /*!in this case no window was used before  (start  of previous used window =
   * end of previous used window ) no need to restore values  as it is the first
   * window used so return from function
   */
  if (this->start_point.x == this->end_point.x &&
      this->start_point.y == this->end_point.y &&
      this->start_point.z == this->end_point.z) {
    return;
  }
  /*! in this case a previous window exists so we want to restore the
   * original_array values of its boundaries before being changed to zeros
   * start_point and end_point are the start and end points of the last used
   * window in all dimensions as in the first window used this function returned
   * without doing anything so then the SaveBoundary function is called so the
   * SaveBoundary function updates the start and end to the start and end of the
   * last used window in all dimensions
   */
  int start_x = this->start_point.x;
  int start_y = this->start_point.y;
  int start_z = this->start_point.z;
  int end_x = this->end_point.x;
  int end_y = this->end_point.y;
  int end_z = this->end_point.z;
  int nz_nx = nz * nx;
  int index = 0;
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  } else {
    /*! we restore the velocities from  backup_array (old values before changing
     * them to zeros) by the same order of  storing them in SaveBoundary
     * function first we restore the values for y from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH  for all x and z  from backup_array then the
     * values for y from ny-HALF_LENGTH to ny -HALF_LENGTH - BOUND_LENGTH for
     * all x and z  from backup_array then the values for x from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH for all y and z from backup_array then the
     * values for x from nx-HALF_LENGTH to nx -HALF_LENGTH - BOUND_LENGTH for
     * all y and z from backup_array then the values for z from HALF_LENGTH to
     * BOUND_LENGTH +HALF_LENGTH  for all x and y from backup_array then the
     * values for z from nz-HALF_LENGTH to nz -HALF_LENGTH - BOUND_LENGTH for
     * all x and y from backup_array
     */
    // restore the values for y boundaries for all x and y
    for (int depth = 0; depth < boundary_length; depth++) {
      for (int row = start_z; row < end_z; row++) {
        for (int column = start_x; column < end_x; column++) {
          // for y from HALF_LENGTH to boundary_length +HALF_LENGTH  for all x
          // and z  from backup_array
          original_array[(depth + start_y) * nz_nx + row * nx + column] =
              this->backup_array[index];
          index++;
          // for y from ny-HALF_LENGTH to ny -HALF_LENGTH - boundary_length for
          // all x and z  from backup_array
          original_array[(end_y - 1 - depth) * nz_nx + row * nx + column] =
              this->backup_array[index];
          index++;
        }
      }
    }
  }

  // restore the values for x boundaries for all y and z
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = start_z; row < end_z; row++) {
      for (int column = 0; column < boundary_length; column++) {
        // for x from HALF_LENGTH to boundary_length +HALF_LENGTH for all y and
        // z from backup_array
        original_array[depth * nz_nx + row * nx + column + start_x] =
            this->backup_array[index];
        index++;
        // for x from nx-HALF_LENGTH to nx -HALF_LENGTH - boundary_length for
        // all y and z from backup_array
        original_array[depth * nz_nx + row * nx + (end_x - 1 - column)] =
            this->backup_array[index];
        index++;
      }
    }
  }
  // restore the values for z boundaries  for all x and y
  for (int depth = start_y; depth < end_y; depth++) {
    for (int row = 0; row < boundary_length; row++) {
      for (int column = start_x; column < end_x; column++) {
        // for z from HALF_LENGTH to boundary_length +HALF_LENGTH  for all x and
        // y from backup_array
        original_array[depth * nz_nx + (start_z + row) * nx + column] =
            this->backup_array[index];
        index++;
        // for z from nz-HALF_LENGTH to nz -HALF_LENGTH - boundary_length for
        // all x and y from backup_array
        original_array[depth * nz_nx + (end_z - 1 - row) * nx + column] =
            this->backup_array[index];
        index++;
      }
    }
  }
}
// De-constructor.
Extension::~Extension() {
  if (this->backup_array != nullptr) {
    // free the memory of the backup_array
    mem_free((void *)this->backup_array);
  }
}
// Extend the velocities at boundaries.
void Extension::ExtendProperty() {
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;
  int nz_nx = nz * nx;
  /*!the nx , ny and nz includes the inner domain + BOUND_LENGTH +HALF_LENGTH in
   * all dimensions and we want to extend the velocities at boundaries only with
   * the HALF_LENGTH excluded
   */
  int start_x = half_length;
  int start_y = half_length;
  int start_z = half_length;
  int end_x = nx - half_length;
  int end_y = ny - half_length;
  int end_z = nz - half_length;
  /*!
   *change the values of velocities at boundaries (HALF_LENGTH excluded) to
   *zeros
   */
  velocity_extension_helper(this->property_array, start_x, start_z, start_y,
                            end_x, end_y, end_z, nx, nz, ny, boundary_length);
}
// In case of window mode extend the velocities at boundaries.
void Extension::ReExtendProperty() {
  /*! Extend the velocities at boundaries by zeros in case of window model
   * three steps
   * 1-restore old values of velocity from last used window.
   * 2-update the start and end points with the start and end of the new window
   * in all dimensions. 3-save the velocities at boundaries of new window  in
   * backup_array. 4-put the velocities at boundaries of new window by the
   * provided implementation(like zeros, randomly, or homogenous)_.
   */
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;
  /*!the window size is a struct containing the window nx,ny,and nz
   * with the HALF_LENGTH and BOUND_LENGTH in all dimensions
   */
  WindowSize window_size = grid->window_size;
  int wnx = window_size.window_nx;
  int wny = window_size.window_ny;
  int wnz = window_size.window_nz;
  if (wnx == nx && wny == ny && wnz == nz) {
    // No window model, no need to re-extend so return from function
    /*!the nx , ny and nz includes the inner domain + BOUND_LENGTH +HALF_LENGTH
     * in all dimensions and we want to extend the velocities at boundaries only
     * with the HALF_LENGTH excluded
     */
    int start_x = half_length;
    int start_y = half_length;
    int start_z = half_length;
    int end_x = nx - half_length;
    int end_y = ny - half_length;
    int end_z = nz - half_length;
    // No window model, no need to re-extend
    // Just re-extend the top boundary.
    this->top_layer_extension_helper(this->property_array, start_x, start_z,
                                     start_y, end_x, end_y, end_z, nx, nz, ny,
                                     boundary_length);
    return;
  } else {
    // window model.
    /*!restore old values of velocity from last used window
     * for the first used window the RestoreBoundary is returned without doing
     * anything
     */
    this->RestoreBoundary(this->property_array, nx, nz, ny);
    int nz_nx = nz * nx;
    /*!we want to work in velocities inside window but with the HALF_LENGTH
     * excluded in all dimensions to reach the bound_length so it is applied in
     * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH
     */
    int start_x = half_length + window_size.window_start.x;
    int start_y = half_length + window_size.window_start.y;
    int start_z = half_length + window_size.window_start.z;
    int end_x = window_size.window_start.x + wnx - half_length;
    int end_y = window_size.window_start.y + wny - half_length;
    int end_z = window_size.window_start.z + wnz - half_length;
    /*!update the start and end points with the start and end
     * of the new window in all dimensions
     */
    this->start_point.x = start_x;
    this->start_point.y = start_y;
    this->start_point.z = start_z;
    this->end_point.x = end_x;
    this->end_point.y = end_y;
    this->end_point.z = end_z;
    /*!save the velocity values at the boundary of the new window in the
     * backup_array
     *
     */
    this->SaveBoundary(this->property_array, nx, nz, ny);
    // extend the velocities at boundaries by zeros
    this->velocity_extension_helper(this->property_array, start_x, start_z,
                                    start_y, end_x, end_y, end_z, nx, nz, ny,
                                    boundary_length);
  }
}

// Zeros out the top layer in preparation of the backward propagation.
void Extension::AdjustPropertyForBackward() {
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;
  /*!the window size is a struct containing the window nx,ny,and nz
   * with the HALF_LENGTH and BOUND_LENGTH in all dimensions
   */
  WindowSize window_size = grid->window_size;
  int wnx = window_size.window_nx;
  int wny = window_size.window_ny;
  int wnz = window_size.window_nz;
  /*!we want to work in velocities inside window but with the HALF_LENGTH
   * excluded in all dimensions to reach the bound_length so it is applied in
   * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH
   */
  int start_x = half_length + window_size.window_start.x;
  int start_y = half_length + window_size.window_start.y;
  int start_z = half_length + window_size.window_start.z;
  int end_x = window_size.window_start.x + wnx - half_length;
  int end_y = window_size.window_start.y + wny - half_length;
  int end_z = window_size.window_start.z + wnz - half_length;
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  }
  this->top_layer_remover_helper(this->property_array, start_x, start_z,
                                 start_y, end_x, end_y, end_z, nx, nz, ny,
                                 boundary_length);
}