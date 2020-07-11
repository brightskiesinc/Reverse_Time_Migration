//
// Created by amr on 18/11/2019.
//
#include "extension.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

// Constructor.
Extension::Extension() {
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
void Extension::SetGridBox(GridBox *grid) {
  this->grid = grid;
}

// Sets the property that will be extended by this extensions object.
void Extension::SetProperty(float *property, float *w_property) {
  this->property_array = property;
  this->window_property_array = w_property;
}

// De-constructor.
Extension::~Extension() {
}

// Extend the velocities at boundaries.
void Extension::ExtendProperty() {
  int nx = grid->grid_size.nx;
  int nz = grid->grid_size.nz;
  int ny = grid->grid_size.ny;
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
  /*! ReExtend the velocities in case of window model
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
  if (property_array == window_property_array) {
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
    /*!we want to work in velocities inside window but with the HALF_LENGTH
     * excluded in all dimensions to reach the bound_length so it is applied in
     * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH
     */
    int start_x = half_length;
    int start_y = half_length;
    int start_z = half_length;
    int end_x = wnx - half_length;
    int end_y = wny - half_length;
    int end_z = wnz - half_length;
    // extend the velocities at boundaries by zeros
    this->velocity_extension_helper(this->window_property_array, start_x, start_z,
                                    start_y, end_x, end_y, end_z, wnx, wnz, wny,
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
  int start_x = half_length;
  int start_y = half_length;
  int start_z = half_length;
  int end_x = wnx - half_length;
  int end_y = wny - half_length;
  int end_z = wnz - half_length;
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  }
  this->top_layer_remover_helper(this->window_property_array, start_x, start_z,
                                 start_y, end_x, end_y, end_z, wnx, wnz, wny,
                                 boundary_length);
}