#include "second_order_computation_kernel.h"
#include <cmath>
#include <iostream>
#include <rtm-framework/skeleton/helpers/timer/timer.hpp>

//
// Created by mirnamoawad on 10/28/19.
//

#define fma(a, b, c) ((a) * (b) + (c))
#define FINITE_ADD(arr, index, off) (arr[index + off] + arr[index - off])
#define VECTOR_LENGTH 16
typedef sycl::float16 vector_float;
/**** CPU Kernels manually unrolled ****/
void iso_2dfd_iteration_global_2(sycl::nd_item<2> it, const vector_float *cur,
                                 const vector_float *cur_p1,
                                 const vector_float *cur_m1,
                                 const vector_float *prev, vector_float *next,
                                 const vector_float *vel, const float *c_x,
                                 const float *c_z, float c_xyz, int *v,
                                 int block_x, int block_z, int v_nx) {
  int idz =
      (it.get_global_id(0) * block_x) + (it.get_global_id(1) * block_z) * v_nx;
  float c_x_0 = c_x[0];
  float c_z_0 = c_z[0];
  int v_0 = v[0];
  for (int i = 0; i < block_z; i++) {
    int idx = idz;
    for (int j = 0; j < block_x; j++) {
      vector_float value = (vector_float)(0.0f);
      value = fma(cur[idx], c_xyz, value);
      value = fma(cur_m1[idx], c_x_0, value);
      value = fma(cur_p1[idx], c_x_0, value);
      value = fma(FINITE_ADD(cur, idx, v_0), c_z_0, value);
      value = fma(vel[idx], value, -prev[idx]);
      value = fma(2.0f, cur[idx], value);
      next[idx] = value;
      idx++;
    }
    idz += v_nx;
  }
}

void iso_2dfd_iteration_global_4(
    sycl::nd_item<2> it, const vector_float *cur, const vector_float *cur_p1,
    const vector_float *cur_p2, const vector_float *cur_m1,
    const vector_float *cur_m2, const vector_float *prev, vector_float *next,
    const vector_float *vel, const float *c_x, const float *c_z, float c_xyz,
    int *v, int block_x, int block_z, int v_nx) {
  int idz =
      (it.get_global_id(0) * block_x) + (it.get_global_id(1) * block_z) * v_nx;
  float c_x_0 = c_x[0];
  float c_x_1 = c_x[1];
  float c_z_0 = c_z[0];
  float c_z_1 = c_z[1];
  int v_0 = v[0];
  int v_1 = v[1];
  for (int i = 0; i < block_z; i++) {
    int idx = idz;
    for (int j = 0; j < block_x; j++) {
      vector_float value = (vector_float)(0.0f);
      value = fma(cur[idx], c_xyz, value);
      value = fma(cur_m1[idx], c_x_0, value);
      value = fma(cur_p1[idx], c_x_0, value);
      value = fma(cur_m2[idx], c_x_1, value);
      value = fma(cur_p2[idx], c_x_1, value);
      value = fma(FINITE_ADD(cur, idx, v_0), c_z_0, value);
      value = fma(FINITE_ADD(cur, idx, v_1), c_z_1, value);
      value = fma(vel[idx], value, -prev[idx]);
      value = fma(2.0f, cur[idx], value);
      next[idx] = value;
      idx++;
    }
    idz += v_nx;
  }
}

void iso_2dfd_iteration_global_8(
    sycl::nd_item<2> it, const vector_float *cur, const vector_float *cur_p1,
    const vector_float *cur_p2, const vector_float *cur_p3,
    const vector_float *cur_p4, const vector_float *cur_m1,
    const vector_float *cur_m2, const vector_float *cur_m3,
    const vector_float *cur_m4, const vector_float *prev, vector_float *next,
    const vector_float *vel, const float *c_x, const float *c_z, float c_xyz,
    int *v, int block_x, int block_z, int v_nx) {
  int idz =
      (it.get_global_id(0) * block_x) + (it.get_global_id(1) * block_z) * v_nx;
  float c_x_0 = c_x[0];
  float c_x_1 = c_x[1];
  float c_x_2 = c_x[2];
  float c_x_3 = c_x[3];
  float c_z_0 = c_z[0];
  float c_z_1 = c_z[1];
  float c_z_2 = c_z[2];
  float c_z_3 = c_z[3];
  int v_0 = v[0];
  int v_1 = v[1];
  int v_2 = v[2];
  int v_3 = v[3];
  for (int i = 0; i < block_z; i++) {
    int idx = idz;
    for (int j = 0; j < block_x; j++) {
      vector_float value = (vector_float)(0.0f);
      value = fma(cur[idx], c_xyz, value);
      value = fma(cur_m1[idx], c_x_0, value);
      value = fma(cur_p1[idx], c_x_0, value);
      value = fma(cur_m2[idx], c_x_1, value);
      value = fma(cur_p2[idx], c_x_1, value);
      value = fma(cur_m3[idx], c_x_2, value);
      value = fma(cur_p3[idx], c_x_2, value);
      value = fma(cur_m4[idx], c_x_3, value);
      value = fma(cur_p4[idx], c_x_3, value);
      value = fma(FINITE_ADD(cur, idx, v_0), c_z_0, value);
      value = fma(FINITE_ADD(cur, idx, v_1), c_z_1, value);
      value = fma(FINITE_ADD(cur, idx, v_2), c_z_2, value);
      value = fma(FINITE_ADD(cur, idx, v_3), c_z_3, value);
      value = fma(vel[idx], value, -prev[idx]);
      value = fma(2.0f, cur[idx], value);
      next[idx] = value;
      idx++;
    }
    idz += v_nx;
  }
}

void iso_2dfd_iteration_global_12(
    sycl::nd_item<2> it, const vector_float *cur, const vector_float *cur_p1,
    const vector_float *cur_p2, const vector_float *cur_p3,
    const vector_float *cur_p4, const vector_float *cur_p5,
    const vector_float *cur_p6, const vector_float *cur_m1,
    const vector_float *cur_m2, const vector_float *cur_m3,
    const vector_float *cur_m4, const vector_float *cur_m5,
    const vector_float *cur_m6, const vector_float *prev, vector_float *next,
    const vector_float *vel, const float *c_x, const float *c_z, float c_xyz,
    int *v, int block_x, int block_z, int v_nx) {
  int idz =
      (it.get_global_id(0) * block_x) + (it.get_global_id(1) * block_z) * v_nx;
  float c_x_0 = c_x[0];
  float c_x_1 = c_x[1];
  float c_x_2 = c_x[2];
  float c_x_3 = c_x[3];
  float c_x_4 = c_x[4];
  float c_x_5 = c_x[5];
  float c_z_0 = c_z[0];
  float c_z_1 = c_z[1];
  float c_z_2 = c_z[2];
  float c_z_3 = c_z[3];
  float c_z_4 = c_z[4];
  float c_z_5 = c_z[5];
  int v_0 = v[0];
  int v_1 = v[1];
  int v_2 = v[2];
  int v_3 = v[3];
  int v_4 = v[4];
  int v_5 = v[5];
  for (int i = 0; i < block_z; i++) {
    int idx = idz;
    for (int j = 0; j < block_x; j++) {
      vector_float value = (vector_float)(0.0f);
      value = fma(cur[idx], c_xyz, value);
      value = fma(cur_m1[idx], c_x_0, value);
      value = fma(cur_p1[idx], c_x_0, value);
      value = fma(cur_m2[idx], c_x_1, value);
      value = fma(cur_p2[idx], c_x_1, value);
      value = fma(cur_m3[idx], c_x_2, value);
      value = fma(cur_p3[idx], c_x_2, value);
      value = fma(cur_m4[idx], c_x_3, value);
      value = fma(cur_p4[idx], c_x_3, value);
      value = fma(cur_m5[idx], c_x_4, value);
      value = fma(cur_p5[idx], c_x_4, value);
      value = fma(cur_m6[idx], c_x_5, value);
      value = fma(cur_p6[idx], c_x_5, value);
      value = fma(FINITE_ADD(cur, idx, v_0), c_z_0, value);
      value = fma(FINITE_ADD(cur, idx, v_1), c_z_1, value);
      value = fma(FINITE_ADD(cur, idx, v_2), c_z_2, value);
      value = fma(FINITE_ADD(cur, idx, v_3), c_z_3, value);
      value = fma(FINITE_ADD(cur, idx, v_4), c_z_4, value);
      value = fma(FINITE_ADD(cur, idx, v_5), c_z_5, value);
      value = fma(vel[idx], value, -prev[idx]);
      value = fma(2.0f, cur[idx], value);
      next[idx] = value;
      idx++;
    }
    idz += v_nx;
  }
}

void iso_2dfd_iteration_global_16(
    sycl::nd_item<2> it, const vector_float *cur, const vector_float *cur_p1,
    const vector_float *cur_p2, const vector_float *cur_p3,
    const vector_float *cur_p4, const vector_float *cur_p5,
    const vector_float *cur_p6, const vector_float *cur_p7,
    const vector_float *cur_p8, const vector_float *cur_m1,
    const vector_float *cur_m2, const vector_float *cur_m3,
    const vector_float *cur_m4, const vector_float *cur_m5,
    const vector_float *cur_m6, const vector_float *cur_m7,
    const vector_float *cur_m8, const vector_float *prev, vector_float *next,
    const vector_float *vel, const float *c_x, const float *c_z, float c_xyz,
    int *v, int block_x, int block_z, int v_nx) {
  int idz =
      (it.get_global_id(0) * block_x) + (it.get_global_id(1) * block_z) * v_nx;
  float c_x_0 = c_x[0];
  float c_x_1 = c_x[1];
  float c_x_2 = c_x[2];
  float c_x_3 = c_x[3];
  float c_x_4 = c_x[4];
  float c_x_5 = c_x[5];
  float c_x_6 = c_x[6];
  float c_x_7 = c_x[7];
  float c_z_0 = c_z[0];
  float c_z_1 = c_z[1];
  float c_z_2 = c_z[2];
  float c_z_3 = c_z[3];
  float c_z_4 = c_z[4];
  float c_z_5 = c_z[5];
  float c_z_6 = c_z[6];
  float c_z_7 = c_z[7];
  int v_0 = v[0];
  int v_1 = v[1];
  int v_2 = v[2];
  int v_3 = v[3];
  int v_4 = v[4];
  int v_5 = v[5];
  int v_6 = v[6];
  int v_7 = v[7];
  for (int i = 0; i < block_z; i++) {
    int idx = idz;
    for (int j = 0; j < block_x; j++) {
      vector_float value = (vector_float)(0.0f);
      value = fma(cur[idx], c_xyz, value);
      value = fma(cur_m1[idx], c_x_0, value);
      value = fma(cur_p1[idx], c_x_0, value);
      value = fma(cur_m2[idx], c_x_1, value);
      value = fma(cur_p2[idx], c_x_1, value);
      value = fma(cur_m3[idx], c_x_2, value);
      value = fma(cur_p3[idx], c_x_2, value);
      value = fma(cur_m4[idx], c_x_3, value);
      value = fma(cur_p4[idx], c_x_3, value);
      value = fma(cur_m5[idx], c_x_4, value);
      value = fma(cur_p5[idx], c_x_4, value);
      value = fma(cur_m6[idx], c_x_5, value);
      value = fma(cur_p6[idx], c_x_5, value);
      value = fma(cur_m7[idx], c_x_6, value);
      value = fma(cur_p7[idx], c_x_6, value);
      value = fma(cur_m8[idx], c_x_7, value);
      value = fma(cur_p8[idx], c_x_7, value);
      value = fma(FINITE_ADD(cur, idx, v_0), c_z_0, value);
      value = fma(FINITE_ADD(cur, idx, v_1), c_z_1, value);
      value = fma(FINITE_ADD(cur, idx, v_2), c_z_2, value);
      value = fma(FINITE_ADD(cur, idx, v_3), c_z_3, value);
      value = fma(FINITE_ADD(cur, idx, v_4), c_z_4, value);
      value = fma(FINITE_ADD(cur, idx, v_5), c_z_5, value);
      value = fma(FINITE_ADD(cur, idx, v_6), c_z_6, value);
      value = fma(FINITE_ADD(cur, idx, v_7), c_z_7, value);
      value = fma(vel[idx], value, -prev[idx]);
      value = fma(2.0f, cur[idx], value);
      next[idx] = value;
      idx++;
    }
    idz += v_nx;
  }
}

/******************************************************************************************************************/

int SecondOrderComputationKernel::instance = 0;

SecondOrderComputationKernel::~SecondOrderComputationKernel() {
  cl::sycl::free((void *)d_coeff_x, in_queue->get_context());
  cl::sycl::free((void *)d_coeff_y, in_queue->get_context());
  cl::sycl::free((void *)d_coeff_z, in_queue->get_context());
  cl::sycl::free((void *)d_vertical, in_queue->get_context());
  cl::sycl::free((void *)d_front, in_queue->get_context());
  // time_out->close();
}

SecondOrderComputationKernel::SecondOrderComputationKernel() {
  this->boundary_manager = nullptr;
  this->grid = nullptr;
  this->parameters = nullptr;
  // this->time_out = new std::ofstream ("time_result_" + to_string(instance) +
  // ".txt");
  instance++;
}

template <bool is_2D, HALF_LENGTH half_length>
void SecondOrderComputationKernel::Computation_syclDevice(
    AcousticSecondGrid *grid, AcousticDpcComputationParameters *parameters) {
  // Read parameters into local variables to be shared.

  size_t nx = grid->grid_size.nx;
  size_t nz = grid->grid_size.nz;

  // Pre-compute the coefficients for each direction.
  int hl = half_length;
  // ToDo try to utilize unrolling and template to reduce redundancy.
  if (parameters->device == CPU) {
    int compute_nz = (nz - 2 * hl) / parameters->block_z;
    auto current = (vector_float *)(grid->pressure_current + hl + hl * nx);
    auto current_p_1 =
        (vector_float *)(grid->pressure_current + hl + 1 + hl * nx);
    auto current_p_2 =
        (vector_float *)(grid->pressure_current + hl + 2 + hl * nx);
    auto current_p_3 =
        (vector_float *)(grid->pressure_current + hl + 3 + hl * nx);
    auto current_p_4 =
        (vector_float *)(grid->pressure_current + hl + 4 + hl * nx);
    auto current_p_5 =
        (vector_float *)(grid->pressure_current + hl + 5 + hl * nx);
    auto current_p_6 =
        (vector_float *)(grid->pressure_current + hl + 6 + hl * nx);
    auto current_p_7 =
        (vector_float *)(grid->pressure_current + hl + 7 + hl * nx);
    auto current_p_8 =
        (vector_float *)(grid->pressure_current + hl + 8 + hl * nx);
    auto current_m_1 =
        (vector_float *)(grid->pressure_current + hl - 1 + hl * nx);
    auto current_m_2 =
        (vector_float *)(grid->pressure_current + hl - 2 + hl * nx);
    auto current_m_3 =
        (vector_float *)(grid->pressure_current + hl - 3 + hl * nx);
    auto current_m_4 =
        (vector_float *)(grid->pressure_current + hl - 4 + hl * nx);
    auto current_m_5 =
        (vector_float *)(grid->pressure_current + hl - 5 + hl * nx);
    auto current_m_6 =
        (vector_float *)(grid->pressure_current + hl - 6 + hl * nx);
    auto current_m_7 =
        (vector_float *)(grid->pressure_current + hl - 7 + hl * nx);
    auto current_m_8 =
        (vector_float *)(grid->pressure_current + hl - 8 + hl * nx);
    auto prev = (vector_float *)(grid->pressure_previous + hl + hl * nx);
    auto next = (vector_float *)(grid->pressure_next + hl + hl * nx);
    auto vel = (vector_float *)(grid->velocity + hl + hl * nx);
    int v_nx = nx / VECTOR_LENGTH;
    float *c_x = d_coeff_x;
    float *c_z = d_coeff_z;
    float c_xyz = coeff_xyz;
    int *v = d_vertical;
    int block_z = parameters->block_z;
    int block_x = parameters->block_x / VECTOR_LENGTH;
    if (half_length == O_2) {
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range =
            range<2>(grid->compute_nx / parameters->block_x, compute_nz);
        auto local_range = range<2>(1, 1);
        sycl::nd_range<2> workgroup_range(global_range, local_range);
        cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
          iso_2dfd_iteration_global_2(it, current, current_p_1, current_m_1,
                                      prev, next, vel, c_x, c_z, c_xyz, v,
                                      block_x, block_z, v_nx);
        });
      });
    } else if (half_length == O_4) {
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range =
            range<2>(grid->compute_nx / parameters->block_x, compute_nz);
        auto local_range = range<2>(1, 1);
        sycl::nd_range<2> workgroup_range(global_range, local_range);
        cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
          iso_2dfd_iteration_global_4(
              it, current, current_p_1, current_p_2, current_m_1, current_m_2,
              prev, next, vel, c_x, c_z, c_xyz, v, block_x, block_z, v_nx);
        });
      });
    } else if (half_length == O_8) {
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range =
            range<2>(grid->compute_nx / parameters->block_x, compute_nz);
        auto local_range = range<2>(1, 1);
        sycl::nd_range<2> workgroup_range(global_range, local_range);
        cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
          iso_2dfd_iteration_global_8(
              it, current, current_p_1, current_p_2, current_p_3, current_p_4,
              current_m_1, current_m_2, current_m_3, current_m_4, prev, next,
              vel, c_x, c_z, c_xyz, v, block_x, block_z, v_nx);
        });
      });
    } else if (half_length == O_12) {
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range =
            range<2>(grid->compute_nx / parameters->block_x, compute_nz);
        auto local_range = range<2>(1, 1);
        sycl::nd_range<2> workgroup_range(global_range, local_range);
        cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
          iso_2dfd_iteration_global_12(
              it, current, current_p_1, current_p_2, current_p_3, current_p_4,
              current_p_5, current_p_6, current_m_1, current_m_2, current_m_3,
              current_m_4, current_m_5, current_m_6, prev, next, vel, c_x, c_z,
              c_xyz, v, block_x, block_z, v_nx);
        });
      });
    } else if (half_length == O_16) {
      AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
        auto global_range =
            range<2>(grid->compute_nx / parameters->block_x, compute_nz);
        auto local_range = range<2>(1, 1);
        sycl::nd_range<2> workgroup_range(global_range, local_range);
        cgh.parallel_for(workgroup_range, [=](nd_item<2> it) {
          iso_2dfd_iteration_global_16(
              it, current, current_p_1, current_p_2, current_p_3, current_p_4,
              current_p_5, current_p_6, current_p_7, current_p_8, current_m_1,
              current_m_2, current_m_3, current_m_4, current_m_5, current_m_6,
              current_m_7, current_m_8, prev, next, vel, c_x, c_z, c_xyz, v,
              block_x, block_z, v_nx);
        });
      });
    }
  } else if (parameters->device == GPU_SHARED) {
    int compute_nz = (nz - 2 * hl);
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      auto global_range = range<2>(compute_nz, grid->compute_nx);
      auto local_range = range<2>(parameters->block_z, parameters->block_x);
      sycl::nd_range<2> workgroup_range(global_range, local_range);
      const float *current = grid->pressure_current;
      const float *prev = grid->pressure_previous;
      float *next = grid->pressure_next;
      const float *vel = grid->velocity;
      const float *c_x = d_coeff_x;
      const float *c_z = d_coeff_z;
      const float c_xyz = coeff_xyz;
      const int *v = d_vertical;
      const int idx_range = parameters->block_z;
      const int local_nx = parameters->block_x + 2 * hl;
      auto localRange_ptr_cur = range<1>(((parameters->block_x + (2 * hl)) *
                                          (parameters->block_z + (2 * hl))));
      //  Create an accessor for SLM buffer
      accessor<float, 1, access::mode::read_write, access::target::local> tab(
          localRange_ptr_cur, cgh);
      cgh.parallel_for<class secondOrderComputation_dpcpp>(
          workgroup_range, [=](nd_item<2> it) {
            float *local = tab.get_pointer();
            int idx =
                it.get_global_id(1) + hl + (it.get_global_id(0) + hl) * nx;
            size_t id0 = it.get_local_id(1);
            size_t id1 = it.get_local_id(0);
            size_t identifiant = (id0 + hl) + (id1 + hl) * local_nx;
            float c_x_loc[half_length];
            float c_z_loc[half_length];
            int v_loc[half_length];
            // Set local coeff.
            for (unsigned int iter = 0; iter < half_length; iter++) {
              c_x_loc[iter] = c_x[iter];
              c_z_loc[iter] = c_z[iter];
              v_loc[iter] = (iter + 1) * local_nx;
            }
            bool copyHaloX = false;
            bool copyHaloY = false;
            const unsigned int items_X = it.get_local_range(1);
            // Set Shared Memory.
            local[identifiant] = current[idx];
            if (id0 < half_length) {
              copyHaloX = true;
            }
            if (id1 < half_length) {
              copyHaloY = true;
            }
            if (copyHaloX) {
              local[identifiant - half_length] = current[idx - half_length];
              local[identifiant + items_X] = current[idx + items_X];
            }
            if (copyHaloY) {
              local[identifiant - half_length * local_nx] =
                  current[idx - half_length * nx];
              local[identifiant + idx_range * local_nx] =
                  current[idx + idx_range * nx];
            }
            it.barrier(access::fence_space::local_space);
            float value = 0;
            value = fma(local[identifiant], c_xyz, value);
            for (int iter = 1; iter <= half_length; iter++) {
              value = fma(local[identifiant - iter], c_x_loc[iter - 1], value);
              value = fma(local[identifiant + iter], c_x_loc[iter - 1], value);
            }
            for (int iter = 1; iter <= half_length; iter++) {
              value = fma(local[identifiant - v_loc[iter - 1]],
                          c_z_loc[iter - 1], value);
              value = fma(local[identifiant + v_loc[iter - 1]],
                          c_z_loc[iter - 1], value);
            }
            value = fma(vel[idx], value, -prev[idx]);
            value = fma(2.0f, local[identifiant], value);
            next[idx] = value;
          });
    });
  } else if (parameters->device == GPU_SEMI_SHARED) {
    int compute_nz = (nz - 2 * hl) / parameters->block_z;
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      auto global_range = range<2>(compute_nz, grid->compute_nx);
      auto local_range = range<2>(1, parameters->block_x);
      sycl::nd_range<2> workgroup_range(global_range, local_range);
      const float *current = grid->pressure_current;
      const float *prev = grid->pressure_previous;
      float *next = grid->pressure_next;
      const float *vel = grid->velocity;
      const float *c_x = d_coeff_x;
      const float *c_z = d_coeff_z;
      const float c_xyz = coeff_xyz;
      const int *v = d_vertical;
      const int idx_range = parameters->block_z;
      const int local_nx = parameters->block_x + 2 * hl;
      const int local_nz = parameters->block_z + 2 * hl;
      auto localRange_ptr_cur = range<1>(((parameters->block_x + (2 * hl)) *
                                          (parameters->block_z + (2 * hl))));
      //  Create an accessor for SLM buffer
      accessor<float, 1, access::mode::read_write, access::target::local> tab(
          localRange_ptr_cur, cgh);
      cgh.parallel_for<class secondOrderComputation_dpcpp>(
          workgroup_range, [=](nd_item<2> it) {
            float *local = tab.get_pointer();
            int idx = it.get_global_id(1) + hl +
                      (it.get_global_id(0) * idx_range + hl) * nx;
            size_t id0 = it.get_local_id(1);
            size_t identifiant = (id0 + hl) + hl * local_nx;
            float c_x_loc[half_length];
            float c_z_loc[half_length];
            int v_loc[half_length];
            // Set local coeff.
            for (unsigned int iter = 0; iter < half_length; iter++) {
              c_x_loc[iter] = c_x[iter];
              c_z_loc[iter] = c_z[iter];
              v_loc[iter] = (iter + 1) * local_nx;
            }
            bool copyHaloX = false;
            if (id0 < half_length)
              copyHaloX = true;
            const unsigned int items_X = it.get_local_range(1);
            int load_identifiant = identifiant - hl * local_nx;
            int load_idx = idx - hl * nx;
            // Set Shared Memory.
            for (int i = 0; i < local_nz; i++) {
              local[load_identifiant] = current[load_idx];
              if (copyHaloX) {
                local[load_identifiant - half_length] =
                    current[load_idx - half_length];
                local[load_identifiant + items_X] = current[load_idx + items_X];
              }
              load_idx += nx;
              load_identifiant += local_nx;
            }
            it.barrier(access::fence_space::local_space);
            for (int i = 0; i < idx_range; i++) {
              float value = 0;
              value = fma(local[identifiant], c_xyz, value);
              for (int iter = 1; iter <= half_length; iter++) {
                value =
                    fma(local[identifiant - iter], c_x_loc[iter - 1], value);
                value =
                    fma(local[identifiant + iter], c_x_loc[iter - 1], value);
              }
              for (int iter = 1; iter <= half_length; iter++) {
                value = fma(local[identifiant - v_loc[iter - 1]],
                            c_z_loc[iter - 1], value);
                value = fma(local[identifiant + v_loc[iter - 1]],
                            c_z_loc[iter - 1], value);
              }
              value = fma(vel[idx], value, -prev[idx]);
              value = fma(2.0f, local[identifiant], value);
              next[idx] = value;
              idx += nx;
              identifiant += local_nx;
            }
          });
    });
  } else if (parameters->device == GPU) {
    int compute_nz = (nz - 2 * hl) / parameters->block_z;
    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
      auto global_range = range<2>(compute_nz, grid->compute_nx);
      auto local_range = range<2>(1, parameters->block_x);
      sycl::nd_range<2> workgroup_range(global_range, local_range);
      const float *current = grid->pressure_current;
      const float *prev = grid->pressure_previous;
      float *next = grid->pressure_next;
      const float *vel = grid->velocity;
      const float *c_x = d_coeff_x;
      const float *c_z = d_coeff_z;
      const float c_xyz = coeff_xyz;
      const int *v = d_vertical;
      const int idx_range = parameters->block_z;
      const int pad = 0;
      auto localRange_ptr_cur =
          range<1>((parameters->block_x + (2 * hl) + pad));
      //  Create an accessor for SLM buffer
      accessor<float, 1, access::mode::read_write, access::target::local> tab(
          localRange_ptr_cur, cgh);
      cgh.parallel_for<class secondOrderComputation_dpcpp>(
          workgroup_range, [=](nd_item<2> it) {
            float *local = tab.get_pointer();
            int idx = it.get_global_id(1) + hl +
                      (it.get_global_id(0) * idx_range + hl) * nx;
            size_t id0 = it.get_local_id(1);
            size_t identifiant = (id0 + hl);
            float c_x_loc[half_length];
            float c_z_loc[half_length];
            int v_end = v[half_length - 1];
            float front[half_length + 1];
            float back[half_length];
            for (unsigned int iter = 0; iter <= half_length; iter++) {
              front[iter] = current[idx + nx * iter];
            }
            for (unsigned int iter = 1; iter <= half_length; iter++) {
              back[iter - 1] = current[idx - nx * iter];
              c_x_loc[iter - 1] = c_x[iter - 1];
              c_z_loc[iter - 1] = c_z[iter - 1];
            }
            bool copyHaloX = false;
            if (id0 < half_length)
              copyHaloX = true;
            const unsigned int items_X = it.get_local_range(1);
            for (int i = 0; i < idx_range; i++) {
              local[identifiant] = front[0];
              if (copyHaloX) {
                local[identifiant - half_length] = current[idx - half_length];
                local[identifiant + items_X] = current[idx + items_X];
              }
              it.barrier(access::fence_space::local_space);
              float value = 0;
              value = fma(local[identifiant], c_xyz, value);
              for (int iter = 1; iter <= half_length; iter++) {
                value =
                    fma(local[identifiant - iter], c_x_loc[iter - 1], value);
                value =
                    fma(local[identifiant + iter], c_x_loc[iter - 1], value);
              }
              for (int iter = 1; iter <= half_length; iter++) {
                value = fma(front[iter], c_z_loc[iter - 1], value);
                value = fma(back[iter - 1], c_z_loc[iter - 1], value);
              }
              value = fma(vel[idx], value, -prev[idx]);
              value = fma(2.0f, local[identifiant], value);
              next[idx] = value;
              idx += nx;
              for (unsigned int iter = half_length - 1; iter > 0; iter--) {
                back[iter] = back[iter - 1];
              }
              back[0] = front[0];
              for (unsigned int iter = 0; iter < half_length; iter++) {
                front[iter] = front[iter + 1];
              }
              // Only one new data-point read from global memory
              // in z-dimension (depth)
              front[half_length] = current[idx + v_end];
            }
          });
    });
  }
  AcousticDpcComputationParameters::device_queue->wait();
}

void SecondOrderComputationKernel::Step() {
  // Pre-compute the coefficients for each direction.
  int half_length = parameters->half_length;

  int size = (grid->original_dimensions.nx - 2 * half_length) *
             (grid->original_dimensions.nz - 2 * half_length);

  // General note: floating point operations for forward is the same as backward
  // (calculated below are for forward). number of floating point operations for
  // the computation kernel in 2D for the half_length loop:6*k,where K is the
  // half_length 5 floating point operations outside the half_length loop Total
  // = 6*K+5 =6*K+5
  int flops_per_second = 6 * half_length + 5;

  // Take a step in time.
  Timer *timer = Timer::getInstance();
  /*timeval start_time;
  gettimeofday(&start_time, NULL);

  double start = start_time.tv_usec + start_time.tv_sec * 1000000;
  start /= 1000000;*/
  timer->_start_timer_for_kernel("SecondOrderComputationKernel::Step", size, 4,
                                 true, flops_per_second);
  if ((grid->grid_size.ny) == 1) {
    switch (parameters->half_length) {
    case O_2:
      Computation_syclDevice<true, O_2>(grid, parameters);
      break;
    case O_4:
      Computation_syclDevice<true, O_4>(grid, parameters);
      break;
    case O_8:
      Computation_syclDevice<true, O_8>(grid, parameters);
      break;
    case O_12:
      Computation_syclDevice<true, O_12>(grid, parameters);
      break;
    case O_16:
      Computation_syclDevice<true, O_16>(grid, parameters);
      break;
    }
  } else {
    /*
    switch (parameters->half_length) {
        case O_2:
            Computation_syclDevice<false, O_2>(grid, parameters);
            break;
        case O_4:
            Computation_syclDevice<false, O_4>(grid, parameters);
            break;
        case O_8:
            Computation_syclDevice<false, O_8>(grid, parameters);
            break;
        case O_12:
            Computation_syclDevice<false, O_12>(grid, parameters);
            break;
        case O_16:
            Computation_syclDevice<false, O_16>(grid, parameters);
            break;
    }
     */
    std::cout << "3D not supported" << std::endl;
  }
  // Swap pointers : Next to current, current to prev and unwanted prev to next
  // to be overwritten.
  if (grid->pressure_previous == grid->pressure_next) {
    // two pointers case : curr becomes both next and prev, while next becomes
    // current.
    grid->pressure_previous = grid->pressure_current;
    grid->pressure_current = grid->pressure_next;
    grid->pressure_next = grid->pressure_previous;
  } else {
    // three pointers : normal swapping between the three pointers.
    float *temp = grid->pressure_next;
    grid->pressure_next = grid->pressure_previous;
    grid->pressure_previous = grid->pressure_current;
    grid->pressure_current = temp;
  }
  timer->stop_timer("SecondOrderComputationKernel::Step");
  /*timeval end_time;
  gettimeofday(&end_time, NULL);

  double end = end_time.tv_usec + end_time.tv_sec * 1000000;
  end /= 1000000;
  double duration = end - start;
  *time_out << duration << std::endl;*/
  if (this->boundary_manager != nullptr) {
    this->boundary_manager->ApplyBoundary(0);
  }
}

void SecondOrderComputationKernel::FirstTouch(float *ptr, uint nx, uint nz,
                                              uint ny) {
  uint half_length = parameters->half_length;
  int block_x = parameters->block_x;
  int block_y = parameters->block_y;
  int block_z = parameters->block_z;
  //    AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh)
  //    {
  //        auto global_range = range<2>(nx - 2 * half_length , nz - 2 *
  //        half_length); auto local_range = range<2>(parameters->block_x,
  //        parameters->block_z); sycl::nd_range<2>workgroup_range(global_range,
  //        local_range); cgh.parallel_for<class
  //        secondOrderComputation_dpcpp>(workgroup_range,[=](nd_item<2> it){
  //
  //            int idx = (it.get_global_id(0) + half_length) +
  //            (it.get_global_id(1) + half_length) * nx; ptr[idx] = 0.0f;
  //        });
  //    });
  //    AcousticDpcComputationParameters::device_queue->wait();
}

void SecondOrderComputationKernel::SetComputationParameters(
    ComputationParameters *parameters) {
  this->parameters = (AcousticDpcComputationParameters *)(parameters);
  if (this->parameters == nullptr) {
    std::cout << "Not a compatible computation parameters : "
                 "expected AcousticDpcComputationParameters"
              << std::endl;
    exit(-1);
  }
}

void SecondOrderComputationKernel::SetGridBox(GridBox *grid_box) {
  ;
  this->grid = (AcousticSecondGrid *)(grid_box);
  if (this->grid == nullptr) {
    std::cout << "Not a compatible gridbox : "
                 "expected AcousticSecondGrid"
              << std::endl;
    exit(-1);
  }
  in_queue = AcousticDpcComputationParameters::device_queue;
  int wnx = grid->window_size.window_nx;
  int wny = grid->window_size.window_ny;
  int wnz = grid->window_size.window_nz;
  float dx = grid->cell_dimensions.dx;
  float dy;
  float dz = grid->cell_dimensions.dz;
  float dx2 = 1 / (dx * dx);
  float dy2;
  float dz2 = 1 / (dz * dz);
  float *coeff = parameters->second_derivative_fd_coeff;
  bool is_2D = wny == 1;
  int wnxnz = wnx * wnz;

  if (!is_2D) {
    dy = grid->cell_dimensions.dy;
    dy2 = 1 / (dy * dy);
  }
  int hl = parameters->half_length;
  int array_length = sizeof(float) * hl;
  float coeff_x[hl];
  float coeff_y[hl];
  float coeff_z[hl];
  int vertical[hl];
  int front[hl];
  d_coeff_x = (float *)cl::sycl::malloc_device(
      array_length,
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  d_coeff_y = (float *)cl::sycl::malloc_device(
      array_length,
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  d_coeff_z = (float *)cl::sycl::malloc_device(
      array_length,
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  d_vertical = (int *)cl::sycl::malloc_device(
      hl * sizeof(int),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  d_front = (int *)cl::sycl::malloc_device(
      hl * sizeof(int),
      AcousticDpcComputationParameters::device_queue->get_device(),
      AcousticDpcComputationParameters::device_queue->get_context());
  for (int i = 0; i < hl; i++) {
    coeff_x[i] = coeff[i + 1] * dx2;
    coeff_z[i] = coeff[i + 1] * dz2;
    if (parameters->device == CPU) {
      vertical[i] = (i + 1) * (wnx / VECTOR_LENGTH);
    } else {
      vertical[i] = (i + 1) * (wnx);
    }
    if (!is_2D) {
      coeff_y[i] = coeff[i + 1] * dy2;
      front[i] = (i + 1) * wnxnz;
    }
  }
  if (is_2D) {
    coeff_xyz = coeff[0] * (dx2 + dz2);
  } else {
    coeff_xyz = coeff[0] * (dx2 + dy2 + dz2);
  }
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memcpy(d_coeff_x, coeff_x, array_length); });
  AcousticDpcComputationParameters::device_queue->submit(
      [&](handler &cgh) { cgh.memcpy(d_coeff_z, coeff_z, array_length); });
  AcousticDpcComputationParameters::device_queue->submit([&](handler &cgh) {
    cgh.memcpy(d_vertical, vertical, hl * sizeof(int));
  });
  if (!is_2D) {
    AcousticDpcComputationParameters::device_queue->submit(
        [&](handler &cgh) { cgh.memcpy(d_coeff_y, coeff_y, array_length); });
    AcousticDpcComputationParameters::device_queue->submit(
        [&](handler &cgh) { cgh.memcpy(d_front, front, hl * sizeof(int)); });
  }
  AcousticDpcComputationParameters::device_queue->wait();
}