/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */



#include "operations/components/independents/concrete/boundary-managers/extensions/RandomExtension.hpp"
#include <operations/utils/checks/Checks.hpp>

using namespace std;
using namespace bs::base::exceptions;
using namespace operations::components;
using namespace operations::components::addons;
using namespace operations::utils::checks;


/**
 * @brief generates random seeds on the left and right boundaries (x boundaries)
 * 
 * @param apRandom          pointer to the parameter that would have random boundaries
 * @param apSeeds           pointer to the seeds allocated memory
 * @param aStrideX          distance between 2 adjacent seeds in x direction
 * @param aStrideZ          distance between 2 adjacent seeds in z direction
 * @param aStartX           domain start in x direction
 * @param aEndX             domain end in x direction
 * @param aStartZ           domain start in z direction
 * @param aEndZ             domain end in z direction
 * @param aNx               domain width
 * @param aBoundaryLength   length of the boundaries
 * @param aMaxVelocity      maximum value of the parameter
 */
void Randomize(float *apRandom, Point3D *apSeeds, int aStrideX, int aStrideZ, int aStartX, int aEndX,
               int aStartZ, int aEndZ, int aNx, int aBoundaryLength, float aMaxVelocity) {

    int index_v = 0;
    int seed_index = 0;
    int index = 0;

    for (int row = aStartZ + aBoundaryLength; row < aEndZ - aBoundaryLength; row += aStrideZ) {
        for (int column = 0; column < aBoundaryLength; column += aStrideX) {

            index = row * aNx + column + aStartX;
            float r_val = GET_RANDOM_VALUE(aBoundaryLength, column) * aMaxVelocity;
            apRandom[index] = abs(apRandom[row * aNx + aBoundaryLength + aStartX] - r_val);
            apSeeds[seed_index] = Point3D(column + aStartX, 1, row);
            seed_index++;

            index = row * aNx + (aEndX - column - 1);
            r_val = GET_RANDOM_VALUE(aBoundaryLength, column) * aMaxVelocity;
            apRandom[index] = abs(apRandom[row * aNx + (aEndX - 1 - aBoundaryLength)] - r_val);
            apSeeds[seed_index] = Point3D((aEndX - column - 1), 1, row);
            seed_index++;

        }
    }


}

/**
 * @brief generates random seeds on the bottom boundary of the domain (z boundaries)
 * 
 * @param apRandom          pointer to the parameter that would have random boundaries
 * @param apSeedsV           pointer to the seeds allocated memory for z boundaries only
 * @param aStrideX          distance between 2 adjacent seeds in x direction
 * @param aStrideZ          distance between 2 adjacent seeds in z direction
 * @param aStartX           domain start in x direction
 * @param aEndX             domain end in x direction
 * @param aStartZ           domain start in z direction
 * @param aEndZ             domain end in z direction
 * @param aNx               domain width
 * @param aBoundaryLength   length of the boundaries
 * @param aMaxVelocity      maximum value of the parameter
 */

void RandomizeV(float *apRandom, Point3D *apSeedsV, int aStrideX, int aStrideZ, int aStartX, int aEndX,
                int aStartZ, int aEndZ, int aNx, int aBoundaryLength, float aMaxVelocity) {

    float temp = 0;
    int index = 0;
    int index_v = 0;

    for (int row = 0; row < aBoundaryLength; row += aStrideZ) {
        for (int column = aStartX; column < aEndX; column += aStrideX) {

            int index = (aEndZ - row - 1) * aNx + column;
            temp = GET_RANDOM_VALUE(aBoundaryLength, row) * aMaxVelocity;
            apRandom[index] = abs(apRandom[(aEndZ - 1 - aBoundaryLength) * aNx + column] - temp);
            apSeedsV[index_v] = Point3D(column, 1, (aEndZ - row - 1));
            index_v++;

        }
    }
}


/**
 * @brief generates 2 random values per point in the boundary, these would be used
 *        in the grain filling around the initial seeds 
 * 
 * @param apArrayX          Random values for vertical distance from seeds calcuation
 * @param apArrayZ          Random values for horizontal distance from seeds calculation
 * @param aStartX           domain start in x direction
 * @param aEndX             domain end in x direction
 * @param aStartZ           domain start in z direction
 * @param aEndZ             domain end in z direction
 * @param aNx               domain width
 * @param aNz               domain hight
 * @param aBoundaryLength   length of the boundaries
 */
void SetRandom(float *apArrayX, float *apArrayZ, int aStartX, int aEndX, int aStartZ, int aEndZ, int aNx, int aNz,
                int aBoundaryLength) {

    int index = 0;

    for (int row = aStartZ + aBoundaryLength; row < aEndZ - aBoundaryLength; row++) {
        for (int column = 0; column < aBoundaryLength; column++) {

            index = row * aNx + column + aStartX;
            apArrayX[index] = RANDOM_VALUE;
            apArrayZ[index] = RANDOM_VALUE;

            index = row * aNx + (aEndX - column - 1);
            apArrayX[index] = RANDOM_VALUE;
            apArrayZ[index] = RANDOM_VALUE;

        }
    }


}

/**
 * @brief generates 2 random values per point in the boundary, these would be used
 *        in the grain filling around the initial seeds 
 *        this one would be used in the bottom boundaries only (z boundaries)
 * 
 * @param apArrayX          Random values for vertical distance from seeds calcuation
 * @param apArrayZ          Random values for horizontal distance from seeds calculation
 * @param aStartX           domain start in x direction
 * @param aEndX             domain end in x direction
 * @param aStartZ           domain start in z direction
 * @param aEndZ             domain end in z direction
 * @param aNx               domain width
 * @param aNz               domain hight
 * @param aBoundaryLength   length of the boundaries
 */

void SetRandomV(float *apArrayX, float *apArrayZ, int aStartX, int aEndX, int aStartZ, int aEndZ, int aNx, int aNz,
                int aBoundaryLength) {

    int index = 0;

    for (int row = 0; row < aBoundaryLength; row++) {
        for (int column = aStartX; column < aEndX; column++) {

            index = (aEndZ - row - 1) * aNx + column;
            apArrayX[index] = RANDOM_VALUE;
            apArrayZ[index] = RANDOM_VALUE;

        }
    }
}


void RandomExtension::VelocityExtensionHelper(float *apPropertyArray,
                                              int aStartX, int aStartY, int aStartZ,
                                              int aEndX, int aEndY, int aEndZ,
                                              int aNx, int aNy, int aNz,
                                              uint aBoundaryLength) {
    /*
    * initialize values required for grain computing
    */

    // finding the gpu device 
    int device_num = omp_get_default_device();
    int host_num = omp_get_initial_device();



    if (is_device_not_exist()) {
        throw DEVICE_NOT_FOUND_EXCEPTION();
    }


    int dx = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    int dy = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    int dz = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int grain_sidelength = this->mGrainSideLength; // in meters

    int stride_x = grain_sidelength / dx;
    int stride_z = grain_sidelength / dz;

    int stride_y = 0;

    if (aNy != 1) {
        stride_y = grain_sidelength / dy;
    }

    uint size = aNx * aNz * aNy;
    uint allocated_size = size * sizeof(float);

    float max_velocity = 0;

    int calculated_size =
            ((aEndZ - aBoundaryLength - aStartZ - aBoundaryLength) / stride_z + 1) * (aBoundaryLength / stride_x + 1);
    calculated_size *= 2;

    uint allocated_bytes = calculated_size * sizeof(Point3D);

    Point3D *seeds = (Point3D *) omp_target_alloc(allocated_bytes, device_num);
    Point3D *seeds_h = (Point3D *) malloc(allocated_bytes);

    int vertical_calculated_size = (aBoundaryLength / stride_z + 1) * ((aEndX - aStartX) / stride_x + 1);
    uint v_allocated_bytes = vertical_calculated_size * sizeof(Point3D);


    Point3D *seeds_v = (Point3D *) omp_target_alloc(v_allocated_bytes, device_num);
    Point3D *seeds_v_h = (Point3D *) malloc(v_allocated_bytes);


    float *arr_x = (float *) omp_target_alloc(allocated_size, device_num);
    float *arr_z = (float *) omp_target_alloc(allocated_size, device_num);
    float *arr_x_h = (float *) malloc(allocated_size);
    float *arr_z_h = (float *) malloc(allocated_size);


    float *random = (float *) malloc(allocated_size);

    omp_target_memcpy(random, apPropertyArray, allocated_size, 0, 0, host_num, device_num);


    float temp = 0;
    int seed_index = 0;

    max_velocity = *max_element(random, random + aNx * aNy * aNz);


    Randomize(random, seeds_h, stride_x, stride_z, aStartX, aEndX, aStartZ, aEndZ, aNx, aBoundaryLength, max_velocity);
    SetRandom(arr_x_h, arr_z_h, aStartX, aEndX, aStartZ, aEndZ, aNx, aNz, aBoundaryLength);


    omp_target_memcpy(seeds, seeds_h, allocated_bytes, 0, 0, device_num, host_num);

    omp_target_memcpy(arr_x, arr_x_h, allocated_size, 0, 0, device_num, host_num);

    omp_target_memcpy(arr_z, arr_z_h, allocated_size, 0, 0, device_num, host_num);

    omp_target_memcpy(apPropertyArray, random, allocated_size, 0, 0, device_num, host_num);


#pragma omp target is_device_ptr( apPropertyArray, seeds, arr_x, arr_z) device(device_num)
#pragma omp parallel for
    for (int row = aStartZ + aBoundaryLength; row < aEndZ - aBoundaryLength; row++) {
        for (int column = 0; column < aBoundaryLength; column++) {

            Point3D left_point(column + aStartX, 1, row);
            Point3D right_point((aEndX - column - 1), 1, row);
            Point3D seed(0, 0, 0);

            int index = row * aNx + column + aStartX;


            bool is_seed = false;

            for (int s = 0; s < calculated_size; s++) {
                if (left_point == seeds[s]) {
                    is_seed = true;
                    break;
                }
            }

            Point3D left_point_seed(0, 0, 0);
            Point3D right_point_seed(0, 0, 0);


            if (!is_seed) {

                for (int s = 0; s < calculated_size; s++) {
                    seed = seeds[s];

                    if (
                            (seed.x <= left_point.x) && (seed.x + stride_x > left_point.x) &&
                            (seed.z <= left_point.z) && (seed.z + stride_z > left_point.z)
                            ) {
                        left_point_seed = Point3D(seeds[s].x, seeds[s].y, seeds[s].z);

                    }

                    if (
                            (seed.x >= right_point.x) && (seed.x - stride_x < right_point.x) &&
                            (seed.z <= right_point.z) && (seed.z + stride_z > right_point.z)
                            ) {
                        right_point_seed = Point3D(seeds[s].x, seeds[s].y, seeds[s].z);

                    }

                }


                int id_x;
                int id_z;

                float px, pz;

                px = arr_x[index];
                pz = arr_z[index];

                float denom_x = (float) (left_point.x - left_point_seed.x) / stride_x;
                float denom_z = (float) (left_point.z - left_point_seed.z) / stride_z;

                if ((px <= denom_x) && (pz <= denom_z)) {
                    id_x = left_point_seed.x + stride_x;
                    id_z = left_point_seed.z + stride_z;
                } else if ((px <= denom_x) && (pz > denom_z)) {
                    id_x = left_point_seed.x + stride_x;
                    id_z = left_point_seed.z;
                } else if ((px > denom_x) && (pz <= denom_z)) {
                    id_x = left_point_seed.x;
                    id_z = left_point_seed.z + stride_z;
                } else if ((px > denom_x) && (pz > denom_z)) {
                    id_x = left_point_seed.x;
                    id_z = left_point_seed.z;
                }

                if (id_z >= aEndZ - aBoundaryLength) {
                    id_z = left_point_seed.z;
                }


                apPropertyArray[left_point.z * aNx + left_point.x] = apPropertyArray[id_z * aNx + id_x];

                index = row * aNx + (aEndX - column - 1);

                px = arr_x[index];
                pz = arr_z[index];

                denom_x = (float) (right_point_seed.x - right_point.x) / stride_x;
                denom_z = (float) (right_point.z - right_point_seed.z) / stride_z;

                if ((px >= denom_x) && (pz <= denom_z)) {
                    id_x = right_point_seed.x;
                    id_z = right_point_seed.z + stride_z;
                } else if ((px >= denom_x) && (pz > denom_z)) {
                    id_x = right_point_seed.x;
                    id_z = right_point_seed.z;
                } else if ((px < denom_x) && (pz <= denom_z)) {
                    id_x = right_point_seed.x - stride_x;
                    id_z = right_point_seed.z + stride_z;
                } else if ((px < denom_x) && (pz > denom_z)) {
                    id_x = right_point_seed.x - stride_x;
                    id_z = right_point_seed.z;
                }

                if (id_x >= aEndX) {
                    id_x = right_point_seed.x;
                }

                if (id_z >= aEndZ - aBoundaryLength) {
                    id_z = right_point_seed.z;
                }

                apPropertyArray[right_point.z * aNx + right_point.x] = apPropertyArray[id_z * aNx + id_x];


            }

        }


    }


    omp_target_memcpy(random, apPropertyArray, allocated_size, 0, 0, host_num, device_num);


    RandomizeV(random, seeds_v_h, stride_x, stride_z, aStartX, aEndX, aStartZ, aEndZ, aNx, aBoundaryLength,
                max_velocity);
    SetRandomV(arr_x_h, arr_z_h, aStartX, aEndX, aStartZ, aEndZ, aNx, aNz, aBoundaryLength);

    omp_target_memcpy(seeds_v, seeds_v_h, v_allocated_bytes, 0, 0, device_num, host_num);

    omp_target_memcpy(arr_x, arr_x_h, allocated_size, 0, 0, device_num, host_num);

    omp_target_memcpy(arr_z, arr_z_h, allocated_size, 0, 0, device_num, host_num);

    omp_target_memcpy(apPropertyArray, random, allocated_size, 0, 0, device_num, host_num);


#pragma omp target is_device_ptr( apPropertyArray, seeds_v, arr_x, arr_z) device(device_num)
#pragma omp parallel for
    for (int row = 0; row < aBoundaryLength; row++) {
        for (int column = aStartX; column < aEndX; column++) {

            int index = (aEndZ - row - 1) * aNx + column;

            Point3D bottom_point(column, 1, aEndZ - row - 1);
            Point3D seed_v(0, 0, 0);


            bool is_seed = false;
            for (int s = 0; s < vertical_calculated_size; s++) {
                if (bottom_point == seeds_v[s]) {
                    is_seed = true;
                    break;
                }
            }

            Point3D bottom_point_seed(0, 0, 0);

            if (!is_seed) {
                for (int s = 0; s < vertical_calculated_size; s++) {
                    seed_v = seeds_v[s];
                    if (
                            (seed_v.x <= bottom_point.x) && (seed_v.x + stride_x > bottom_point.x) &&
                            (seed_v.z >= bottom_point.z) && (seed_v.z - stride_z < bottom_point.z)
                            ) {
                        bottom_point_seed = seed_v;
                    }
                }

                int id_x;
                int id_z;

                float px, pz;

                px = arr_x[index];
                pz = arr_z[index];

                float denom_x = (float) (bottom_point.x - bottom_point_seed.x) / stride_x;
                float denom_z = (float) (bottom_point_seed.z - bottom_point.z) / stride_z;

                if ((px <= denom_x) && (pz >= denom_z)) {
                    id_x = bottom_point_seed.x + stride_x;
                    id_z = bottom_point_seed.z;
                } else if ((px <= denom_x) && (pz < denom_z)) {
                    id_x = bottom_point_seed.x + stride_x;
                    id_z = bottom_point_seed.z - stride_z;
                } else if ((px > denom_x) && (pz >= denom_z)) {
                    id_x = bottom_point_seed.x;
                    id_z = bottom_point_seed.z;
                } else if ((px > denom_x) && (pz < denom_z)) {
                    id_x = bottom_point_seed.x;
                    id_z = bottom_point_seed.z - stride_z;
                }

                if (id_z >= aEndZ) {
                    id_z = bottom_point_seed.z;
                }
                apPropertyArray[bottom_point.z * aNx + bottom_point.x] = apPropertyArray[id_z * aNx + id_x];
            }
        }
    }


    omp_target_free(seeds, device_num);
    omp_target_free(seeds_v, device_num);
    omp_target_free(arr_x, device_num);
    omp_target_free(arr_z, device_num);


    free(random);
    free(arr_x_h);
    free(arr_z_h);
    free(seeds_v_h);
    free(seeds_h);

}

void RandomExtension::TopLayerExtensionHelper(float *apPropertyArray,
                                              int aStartX, int aStartY, int aStartZ,
                                              int aEndX, int aEndY, int aEndZ,
                                              int aNx, int aNy, int aNz, uint aBoundaryLength) {
    // Do nothing, no top layer to extend in random boundaries.
}

void RandomExtension::TopLayerRemoverHelper(float *apPropertyArray,
                                            int aStartX, int aStartY, int aStartZ,
                                            int aEndX, int aEndY, int aEndZ,
                                            int aNx, int aNy, int aNz, uint aBoundaryLength) {
    // Do nothing, no top layer to remove in random boundaries.
}
