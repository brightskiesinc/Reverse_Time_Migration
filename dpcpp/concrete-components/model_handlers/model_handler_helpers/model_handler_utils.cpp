//
// Created by amr on 27/04/2020.
//
#include <iostream>

#include "model_handler_utils.h"

#define make_divisible(v, d) (v + (d - (v % d)))

void add_helper_padding(GridBox *box, ComputationParameters *param) {
  auto grid = (AcousticSecondGrid *)box;
  auto parameters = (AcousticDpcComputationParameters *)param;
  uint block_x = parameters->block_x;
  uint block_z = parameters->block_z;
  uint nx = grid->grid_size.nx;
  uint nz = grid->grid_size.nz;
  uint inx = nx - 2 * parameters->half_length;
  uint inz = nz - 2 * parameters->half_length;
  // Store old values of nx,nz,ny to use in boundaries.
  memcpy(&grid->original_dimensions, &grid->grid_size, sizeof(grid->grid_size));
  if (block_x > inx) {
    block_x = inx;
    std::cout << "Block Factor x > domain size... Reduced to domain size"
              << std::endl;
  }
  if (block_z > inz) {
    block_z = inz;
    std::cout << "Block Factor z > domain size... Reduced to domain size"
              << std::endl;
  }
  if (block_x % 16 != 0 && block_x != 1) {
    block_x = make_divisible(
        block_x,
        16); // Ensure block in x is divisible by 16(biggest vector length).
    std::cout << "Adjusting block factor in x to make it divisible by "
                 "16(Possible Vector Length)..."
              << std::endl;
  }
  if (inx % block_x != 0) {
    std::cout
        << "Adding padding to make domain divisible by block size in  x-axis"
        << std::endl;
    grid->compute_nx = inx = make_divisible(inx, block_x);
    nx = inx + 2 * parameters->half_length;
  }
  if (inz % block_z != 0) {
    std::cout
        << "Adding padding to make domain divisible by block size in  z-axis"
        << std::endl;
    inz = make_divisible(inz, block_z);
    nz = inz + 2 * parameters->half_length;
  }
  if (parameters->cor_block % 16 != 0 && parameters->cor_block != 1) {
    parameters->cor_block = make_divisible(
        parameters->cor_block, 16); // Ensure correlation block is divisible by
                                    // 16(biggest vector length).
    std::cout << "Adjusting correlation block factor to make it divisible by "
                 "16(Possible Vector Length)..."
              << std::endl;
  }
  // Add padding to ensure alignment for each row.
  if (nx % parameters->cor_block != 0) {
    std::cout << "Adding padding to ensure alignment and matching of "
                 "correlation block of each row"
              << std::endl;
    nx = make_divisible(nx, parameters->cor_block);
  }
  // Add padding to ensure alignment for each row.
  if (nx % 16 != 0) {
    std::cout << "Adding padding to ensure alignment of each row" << std::endl;
    nx = make_divisible(nx, 16);
  }
  // Set grid with the padded values.
  grid->grid_size.nx = nx;
  grid->grid_size.nz = nz;
  parameters->block_x = block_x;
  parameters->block_z = block_z;
  std::cout << "Size after padding : " << std::endl;
  std::cout << "\tBlock in x : " << parameters->block_x << std::endl;
  std::cout << "\tBlock in z : " << parameters->block_z << std::endl;
  std::cout << "\tCorrelation block : " << parameters->cor_block << std::endl;
  std::cout << "\tOriginal nx : " << grid->original_dimensions.nx << std::endl;
  std::cout << "\tOriginal nz : " << grid->original_dimensions.nz << std::endl;
  std::cout << "\tPadded nx : " << grid->grid_size.nx << std::endl;
  std::cout << "\tPadded nz : " << grid->grid_size.nz << std::endl;
  std::cout << "\tComputation nx : " << grid->compute_nx << std::endl;
  std::cout << "\tComputation nz : "
            << grid->grid_size.nz - 2 * parameters->half_length << std::endl;
}