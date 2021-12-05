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

#include <cmath>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>
#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>
#include <operations/common/DataTypes.h>
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

void TEST_CASE_CPML(GridBox *apGridBox,
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
                temp_vel[iz * nx + ix + (iy * nx * nz)] = (float) rand() * 100 / RAND_MAX;
                temp_vel[iz * nx + ix + (iy * nx * nz)] *=
                        temp_vel[iz * nx + ix + (iy * nx * nz)] * dt * dt;
            }
        }
    }

    Device::MemSet(pressure_curr->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemSet(pressure_prev->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemCpy(velocity->GetNativePointer(), temp_vel, size * sizeof(float), Device::COPY_HOST_TO_DEVICE);

    auto h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();
    auto location = (wnx / 2) + (wnz / 2) * wnx + (wny / 2) * wnx * wnz;
    h_pressure[location] = 1;

    auto configuration_map = new JSONConfigurationMap(R"(
                {
                    "wave": {
                        "physics": "acoustic",
                        "approximation": "isotropic",
                        "equation-order": "second",
                        "grid-sampling": "uniform"
                    },
                            "type": "cpml",
                            "properties": {
                                "use-top-layer": true,
                                "reflect-coeff": 0.03,
                                "shift-ratio": 0.2,
                                "relax-cp": 0.9
                             }
                }
            )"_json);

    auto boundary_manager = new CPMLBoundaryManager(configuration_map);
    auto computation_kernel = new SecondOrderComputationKernel(apConfigurationMap);

    boundary_manager->SetComputationParameters(apParameters);
    computation_kernel->SetComputationParameters(apParameters);

    boundary_manager->AcquireConfiguration();
    computation_kernel->AcquireConfiguration();

    boundary_manager->SetGridBox(apGridBox);
    computation_kernel->SetGridBox(apGridBox);

    computation_kernel->SetBoundaryManager(boundary_manager);
    computation_kernel->SetMode(operations::components::KERNEL_MODE::FORWARD);

    boundary_manager->ExtendModel();
    float old_norm, new_norm;

    old_norm = calculate_norm(pressure_curr->GetHostPointer(), wnx, wnz, wny);
    computation_kernel->Step();

    for (int i = 1; i <= 1000; i++) {
        computation_kernel->Step();
        if (i % 100 == 0) {
            new_norm = calculate_norm(pressure_curr->GetHostPointer(), wnx, wnz, wny);
            auto check1 = new_norm < old_norm;
            auto check2 =
                    approximately_equal(new_norm, old_norm)
                    == Approx(0).margin(std::numeric_limits<float>::epsilon());
            REQUIRE((check1 || check2));
            old_norm = new_norm;
        }
    }
    delete apGridBox;
    delete apParameters;
    delete apConfigurationMap;

    delete pressure_curr;
    delete pressure_prev;
    delete velocity;
}

TEST_CASE("CPML Boundary Manager - 2D - No Window", "[No Window],[2D]") {
    TEST_CASE_CPML(
            generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}

TEST_CASE("CPML Boundary Manager - 2D - Window", "[Window],[2D]") {
    TEST_CASE_CPML(
            generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}