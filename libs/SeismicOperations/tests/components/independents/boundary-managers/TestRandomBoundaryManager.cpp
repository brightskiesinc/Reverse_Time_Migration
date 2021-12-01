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


#include <unordered_set>
#include <vector>
#include <algorithm>

#include <prerequisites/libraries/catch/catch.hpp>

#include <operations/components/independents/concrete/boundary-managers/RandomBoundaryManager.hpp>
#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/common/DataTypes.h>
#include <operations/configurations/MapKeys.h>
#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>
#include <operations/test-utils/NumberHelpers.hpp>
#include <operations/test-utils/EnvironmentHandler.hpp>

using namespace std;
using namespace bs::base::configurations;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::testutils;


void TEST_CASE_RANDOM(GridBox *apGridBox,
                      ComputationParameters *apParameters,
                      ConfigurationMap *apConfigurationMap) {
    /*
     * Environment setting (i.e. Backend setting initialization).
     */
    set_environment();

    /*
     * Register and allocate parameters and wave fields in
     * grid box according to the current test case.
     */

    auto pressure_curr = new FrameBuffer<float>();
    auto pressure_prev = new FrameBuffer<float>();
    auto velocity = new FrameBuffer<float>();

    /*
     * Variables initialization for grid box.
     */

    int nx, ny, nz;
    int wnx, wnz, wny;

    nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    wnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    wny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    wnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();


    uint window_size = wnx * wny * wnz;
    uint size = nx * ny * nz;

    pressure_curr->Allocate(window_size);
    pressure_prev->Allocate(window_size);
    velocity->Allocate(size);

    apGridBox->RegisterWaveField(WAVE | GB_PRSS | CURR | DIR_Z, pressure_curr);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | PREV | DIR_Z, pressure_prev);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | NEXT | DIR_Z, pressure_prev);
    apGridBox->RegisterParameter(PARM | GB_VEL, velocity);

    float temp_vel[size];
    float dt = apGridBox->GetDT();

    for (int iy = 0; iy < ny; iy++) {
        for (int iz = 0; iz < nz; iz++) {
            for (int ix = 0; ix < nx; ix++) {
                temp_vel[iz * nx + ix + (iy * nx * nz)] = 1500;
                temp_vel[iz * nx + ix + (iy * nx * nz)] *=
                        temp_vel[iz * nx + ix + (iy * nx * nz)] * dt * dt;
            }
        }
    }
    Device::MemSet(pressure_curr->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemSet(pressure_prev->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemCpy(velocity->GetNativePointer(), temp_vel, size * sizeof(float), Device::COPY_HOST_TO_DEVICE);

    auto h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();
    int location = (wnx / 2) + (wnz / 2) * wnx + (wny / 2) * wnx * wnz;
    h_pressure[location] = 1;

    auto boundary_manager_map = new JSONConfigurationMap(R"(
                 {
                    "wave": {
                        "physics": "acoustic",
                        "approximation": "isotropic",
                        "equation-order": "second",
                        "grid-sampling": "uniform"
                    },
                    "type": "random",
                    "properties": {
                        "use-top-layer": false,
                        "reflect-coeff": 0.03,
                        "shift-ratio": 0.2,
                        "relax-cp": 0.9,
                        "grain-side-length": 20
                     }
                }
    )"_json);

    auto computation_kernel_map = new JSONConfigurationMap(R"(
                 {
                    "wave": {
                        "physics": "acoustic",
                        "approximation": "isotropic",
                        "equation-order": "second",
                        "grid-sampling": "uniform"
                    }
                }
    )"_json);


    /* Some parsing should be done for having the value of grain,
     * check the test of get parameter. */

    auto boundary_manager = new RandomBoundaryManager(boundary_manager_map);
    auto computation_kernel = new SecondOrderComputationKernel(computation_kernel_map);

    boundary_manager->SetComputationParameters(apParameters);
    computation_kernel->SetComputationParameters(apParameters);

    boundary_manager->AcquireConfiguration();
    computation_kernel->AcquireConfiguration();

    boundary_manager->SetGridBox(apGridBox);
    computation_kernel->SetGridBox(apGridBox);

    computation_kernel->SetBoundaryManager(boundary_manager);

    boundary_manager->ExtendModel();

    int hl = apParameters->GetHalfLength();
    int bl = apParameters->GetBoundaryLength();

    float *v = velocity->GetHostPointer();

    int start_x = hl;
    int start_y = hl;
    int start_z = hl;

    int end_x = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - hl;
    int end_y = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - hl;
    int end_z = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - hl;

    float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


    if (ny > 1) {
        start_y = hl + bl;
        end_y = ny - hl;
    }

    // Grain size in meters.
    int grain_side_length = 0;
    grain_side_length = boundary_manager_map->GetValue(OP_K_PROPRIETIES, OP_K_GRAIN_SIDE_LENGTH, grain_side_length);


    int stride_x = grain_side_length / dx;
    int stride_z = grain_side_length / dz;
    int stride_y = grain_side_length / dy;

    if (ny != 1) {
        stride_y = grain_side_length / dy;
    }

    vector<float> seeds;
    vector<float> seeds_l;
    vector<float> seeds_r;

    int id_x;
    int id_z;

    int horizontal_seeds = ((end_z -bl- start_z - bl) /stride_z + 1) *(bl / stride_x ) ;
   
    int vertical_seeds = (bl / stride_z + 1 ) * ((end_x - start_x ) / stride_x  );


   int index = 0;

    for (int row = 0; row < bl; row++) {
        for (int column = start_x; column < end_x; column++) {

            index = ( end_z - row - 1) * nx + column;
          
           if ( find(seeds.begin(), seeds.end(), v[index]) == seeds.end() ){
               seeds.emplace_back(v[index]);
              
           }
            
        }
    }

    REQUIRE(seeds.size() >= vertical_seeds);


  seeds.clear();

  int index_l = 0;
  int index_r = 0;

    for (int row = start_z + bl; row < end_z - bl; row++) {
        for (int column = 0; column < bl; column++) {
            
            index_l = row * nx + column;
            index_r = row * nx + ( end_x - 1 - column);

            if ( find(seeds_l.begin(), seeds_l.end(), v[index_l]) == seeds_l.end() ){
               seeds_l.emplace_back(v[index_l]);
              
           }

           if ( find(seeds_r.begin(), seeds_r.end(), v[index_r]) == seeds_r.end() ){
               seeds_r.emplace_back(v[index_r]);
              
           }

        }
    }


   REQUIRE(seeds_l.size() >= horizontal_seeds);
   REQUIRE(seeds_r.size() >= horizontal_seeds);


    seeds.clear();


    delete apGridBox;
    delete apParameters;
    delete apConfigurationMap;

    delete boundary_manager_map;
    delete pressure_curr;
    delete pressure_prev;
}

TEST_CASE("Random Boundary Manager - 2D - No Window", "[No Window],[2D]") {
    TEST_CASE_RANDOM(
            generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}

TEST_CASE("Random Boundary Manager - 2D - Window", "[Window],[2D]") {
    TEST_CASE_RANDOM(
            generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}
