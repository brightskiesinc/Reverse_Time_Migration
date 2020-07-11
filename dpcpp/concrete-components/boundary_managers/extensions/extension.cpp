//
// Created by amr on 18/11/2019.
//
#include "extension.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

using namespace cl::sycl;

// Constructor.
Extension::Extension() {
  this->grid = nullptr;
  this->boundary_length = 0;
  this->half_length = 0;
  this->property_array = nullptr;
  this->window_property_array = nullptr;
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
void Extension::SetProperty(float *property, float *w_property) {
  this->property_array = property;
  this->window_property_array = w_property;
}

// De-constructor.
Extension::~Extension() {
}
// Extend the velocities at boundaries.
void Extension::ExtendProperty() {
  auto sgrid = (AcousticSecondGrid *)grid;
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
  int end_x = sgrid->full_original_dimensions.nx - half_length;
  int end_y = sgrid->full_original_dimensions.ny - half_length;
  int end_z = sgrid->full_original_dimensions.nz - half_length;
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
  auto sgrid = (AcousticSecondGrid *)grid;
  if (window_property_array == property_array) {
    // No window model, no need to re-extend so return from function
    /*!the nx , ny and nz includes the inner domain + BOUND_LENGTH +HALF_LENGTH
     * in all dimensions and we want to extend the velocities at boundaries only
     * with the HALF_LENGTH excluded
     */
    int start_x = half_length;
    int start_y = half_length;
    int start_z = half_length;
    int end_x = sgrid->full_original_dimensions.nx - half_length;
    int end_y = sgrid->full_original_dimensions.ny - half_length;
    int end_z = sgrid->full_original_dimensions.nz - half_length;
    // No window model, no need to re-extend
    // Just re-extend the top boundary.
    this->top_layer_extension_helper(this->property_array, start_x, start_z,
                                     start_y, end_x, end_y, end_z, nx, nz, ny,
                                     boundary_length);
    return;
  } else {
    // window model.
    int nz_nx = nz * nx;
    /*!we want to work in velocities inside window but with the HALF_LENGTH
     * excluded in all dimensions to reach the bound_length so it is applied in
     * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH
     */
    int start_x = half_length;
    int start_y = half_length;
    int start_z = half_length;
    int end_x = sgrid->original_dimensions.nx - half_length;
    int end_y = sgrid->original_dimensions.ny - half_length;
    int end_z = sgrid->original_dimensions.nz - half_length;
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
  auto sgrid = (AcousticSecondGrid *)grid;
  int start_x = half_length;
  int start_y = half_length;
  int start_z = half_length;
  int end_x = sgrid->original_dimensions.nx - half_length;
  int end_y = sgrid->original_dimensions.nx - half_length;
  int end_z = sgrid->original_dimensions.nx - half_length;
  if (ny == 1) {
    end_y = 1;
    start_y = 0;
  }
  this->top_layer_remover_helper(this->window_property_array, start_x, start_z,
                                 start_y, end_x, end_y, end_z, wnx, wnz, wny,
                                 boundary_length);
}
