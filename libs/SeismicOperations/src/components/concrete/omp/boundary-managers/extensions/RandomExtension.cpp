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

#include <operations/components/independents/concrete/boundary-managers/extensions/RandomExtension.hpp>

using namespace std;
using namespace operations::components;
using namespace operations::components::addons;
using namespace bs::base::exceptions;

void RandomExtension::VelocityExtensionHelper(float *apPropertyArray,
                                              int aStartX, int aStartY, int aStartZ,
                                              int aEndX, int aEndY, int aEndZ,
                                              int aNx, int aNy, int aNz,
                                              uint aBoundaryLength) {

    /*
     * initialize values required for grain computing
     */
    int dx = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    int dy = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    int dz = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

    int grain_side_length = this->mGrainSideLength; // in meters

    int stride_x = grain_side_length / dx;
    int stride_z = grain_side_length / dz;

    int stride_y = 0;

    if (aNy != 1) {
        stride_y = grain_side_length / dy;
    }

    /*
     * compute maximum value of the property
     */
    float max_velocity = 0;
    max_velocity = *max_element(apPropertyArray, apPropertyArray + aNx * aNy * aNz);

    /*
     * processing boundaries in X dimension "left and right bounds"
     */

    /*
     * populate random seeds
     */
    vector<Point3D> seeds;


    float temp = 0;
    for (int row = aStartZ + aBoundaryLength; row < aEndZ - aBoundaryLength; row += stride_z) {
        for (int column = 0; column < aBoundaryLength; column += stride_x) {

            int index = row * aNx + column + aStartX;
            temp = GET_RANDOM_VALUE(aBoundaryLength, column) * max_velocity;
            apPropertyArray[index] = abs(apPropertyArray[row * aNx + aBoundaryLength + aStartX] - temp);

            seeds.push_back(Point3D(column + aStartX, 1, row));

            index = row * aNx + (aEndX - column - 1);
            temp = GET_RANDOM_VALUE(aBoundaryLength, column) * max_velocity;
            apPropertyArray[index] = abs(apPropertyArray[row * aNx + (aEndX - 1 - aBoundaryLength)] - temp);

            seeds.push_back(Point3D((aEndX - column - 1), 1, row));
        }
    }


    /*
     * fill empty points
     */
    for (int row = aStartZ + aBoundaryLength; row < aEndZ - aBoundaryLength; row++) {
        for (int column = 0; column < aBoundaryLength; column++) {

            Point3D left_point(column + aStartX, 1, row);
            Point3D right_point((aEndX - column - 1), 1, row);

            /*
             * check if this point is a seed point
             * same check works for both left and right points
             */
            bool is_seed = false;
            for (auto seed : seeds) {
                if (left_point == seed) {
                    is_seed = true;
                    break;
                }
            }
            if (is_seed) {
                continue; // this is a seed point, don't fill
            }

            Point3D left_point_seed(0, 0, 0);
            Point3D right_point_seed(0, 0, 0);
            /*
             * Get nearest seed
             */

            for (auto seed : seeds) {
                if (
                        (seed.x <= left_point.x) && (seed.x + stride_x > left_point.x) &&
                        (seed.z <= left_point.z) && (seed.z + stride_z > left_point.z)
                        ) {
                    left_point_seed = seed;
                }

                if (
                        (seed.x >= right_point.x) && (seed.x - stride_x < right_point.x) &&
                        (seed.z <= right_point.z) && (seed.z + stride_z > right_point.z)
                        ) {
                    right_point_seed = seed;
                }
            }

            /*
             * populate left point
             */
            int id_x;
            int id_z;

            float px = RANDOM_VALUE;
            float pz = RANDOM_VALUE;

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

            /*
             * populate right point
             */

            px = RANDOM_VALUE;
            pz = RANDOM_VALUE;

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

    seeds.clear();

    /*
     * processing boundaries in Z dimension "bottom bound"
     */

    /*
     * populate random seeds
     */
    for (int row = 0; row < aBoundaryLength; row += stride_z) {
        for (int column = aStartX; column < aEndX; column += stride_x) {


            int index = (aEndZ - row - 1) * aNx + column;
            temp = GET_RANDOM_VALUE(aBoundaryLength, row) * max_velocity;
            apPropertyArray[index] = abs(apPropertyArray[(aEndZ - 1 - aBoundaryLength) * aNx + column] - temp);

            seeds.push_back(Point3D(column, 1, (aEndZ - row - 1)));
        }
    }

    /*
     * fill empty points
     */
    for (int row = 0; row < aBoundaryLength; row++) {
        for (int column = aStartX; column < aEndX; column++) {

            Point3D bottom_point(column, 1, (aEndZ - row - 1));

            /*
             * check if this point is a seed point
             */
            bool is_seed = false;
            for (auto seed:seeds) {
                if (bottom_point == seed) {
                    is_seed = true;
                    break;
                }
            }
            if (is_seed) {
                continue; // this is a seed point, don't fill
            }

            Point3D bottom_point_seed(0, 0, 0);
            /*
             * Get nearest seed
             */

            for (auto seed:seeds) {
                if (
                        (seed.x <= bottom_point.x) && (seed.x + stride_x > bottom_point.x) &&
                        (seed.z >= bottom_point.z) && (seed.z - stride_z < bottom_point.z)
                        ) {
                    bottom_point_seed = seed;
                }
            }

            /*
             * populate left point
             */
            int id_x;
            int id_z;

            float px = RANDOM_VALUE;
            float pz = RANDOM_VALUE;

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

    seeds.clear();
}

void RandomExtension::TopLayerExtensionHelper(float *property_array,
                                              int aStartX, int aStartY, int aStartZ,
                                              int aEndX, int aEndY, int aEndZ,
                                              int aNx, int aNy, int aNz, uint aBoundaryLength) {
    // Do nothing, no top layer to extend in random boundaries.
}

void RandomExtension::TopLayerRemoverHelper(float *property_array,
                                            int aStartX, int aStartY, int aStartZ,
                                            int aEndX, int aEndY, int aEndZ,
                                            int aNx, int aNy, int aNz, uint aBoundaryLength) {
    // Do nothing, no top layer to remove in random boundaries.
}