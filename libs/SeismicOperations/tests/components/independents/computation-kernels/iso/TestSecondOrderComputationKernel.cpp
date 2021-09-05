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


#include <operations/components/independents/concrete/computation-kernels/isotropic/SecondOrderComputationKernel.hpp>

#include <operations/common/DataTypes.h>
#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>
#include <operations/test-utils/NumberHelpers.hpp>
#include <operations/test-utils/EnvironmentHandler.hpp>

#include <bs/base/memory/MemoryManager.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

#include <limits>

using namespace std;
using namespace bs::base::configurations;
using namespace bs::base::memory;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::testutils;


void TEST_CASE_SECOND_ORDER_COMPUTATION_KERNEL(GridBox *apGridBox,
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
    int start_y, end_y;

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

    /*
     * SecondOrderComputationKernel setting.
     */

    auto computation_kernel = new SecondOrderComputationKernel(apConfigurationMap);
    computation_kernel->SetGridBox(apGridBox);
    computation_kernel->SetComputationParameters(apParameters);
    computation_kernel->SetMode(operations::components::KERNEL_MODE::FORWARD);

    /*
     * Source injection.
     */

    auto h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();
    int location = (wnx / 2) + (wnz / 2) * wnx + (wny / 2) * wnx * wnz;
    h_pressure[location] = 1;

    /*
     * Technology settings for memory transfer.
     */


    auto d_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
    Device::MemCpy(d_pressure, h_pressure, window_size * sizeof(float), Device::COPY_HOST_TO_DEVICE);

    h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();


    computation_kernel->Step();

    h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();

    /*
     * Variables initialization for kernel usage.
     */

    int misses = 0;
    int half_length = apParameters->GetHalfLength();

    start_y = (ny > 1) ? half_length : 0;
    end_y = (ny > 2) ? wny - half_length : 1;

    /*
     * [CHECK]
     * The vertical and horizontal lines around the location centered
     * are not zeros, and symmetrical
     */



    for (int i = 1; i <= half_length; i++) {

        if (h_pressure[location + i] == 0 || h_pressure[location - i] == 0 ||
            h_pressure[location + i * wnx] == 0 || h_pressure[location - i * wnx] == 0) {

            misses += 1;
        }

        if (h_pressure[location + i] > 1.0 || h_pressure[location - i] > 1.0 ||
            h_pressure[location + i * wnx] > 1 || h_pressure[location - i * wnx] > 1) {


            misses += 1;
        }


        if (!approximately_equal(h_pressure[location + i], h_pressure[location - i])) {
            misses += 1;
        }

        if (!approximately_equal(h_pressure[location + i * wnx], h_pressure[location - i * wnx])) {
            misses += 1;
        }

        if (!approximately_equal(h_pressure[location + i], h_pressure[location - i * wnx])) {
            misses += 1;
        }


        if (ny > 1) {

            if (h_pressure[location + i * wnx * wnz] == 0 ||
                h_pressure[location - i * wnx * wnz] == 0) {
                misses += 1;
            }

            if (h_pressure[location + i * wnx * wnz] > 1 ||
                h_pressure[location - i * wnx * wnz] > 1) {
                misses += 1;
            }

            if (!approximately_equal(h_pressure[location + i * wnx * wnz],
                                     h_pressure[location - i * wnx * wnz])) {
                misses += 1;
            }

        }

    }
    REQUIRE(misses == 0);
    for (int t = 0; t < 20; t++) {
        computation_kernel->Step();
    }
    misses = 0;
    h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();

    int index;

    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = half_length; iz < wnz - half_length; iz++) {
            for (int ix = half_length; ix < wnx - half_length; ix++) {
                index = iy * wnx * wnz + iz * wnx + ix;
                if (h_pressure[index] == 0) {
                    misses += 1;
                }
            }
        }
    }
    REQUIRE(misses == 0);

    /*
     * [CHECK]
     * Norm checking.
     */
    for (int t = 0; t < 1000; t++) {
        computation_kernel->Step();
        if (t % 100 == 0) {
            h_pressure = apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer();
            float norm = calculate_norm(h_pressure, wnx, wnz, wny);
            REQUIRE(Approx(norm).epsilon(1) == 1);
        }
    }
    delete apGridBox;
    delete apParameters;
    delete apConfigurationMap;
}

TEST_CASE("Isotropic Second Order - 2D - No Window", "[No Window],[2D]") {
TEST_CASE_SECOND_ORDER_COMPUTATION_KERNEL(
        generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
        generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("Isotropic Second Order - 2D - Window", "[Window],[2D]") {
TEST_CASE_SECOND_ORDER_COMPUTATION_KERNEL(
        generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
        generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

/* Both DPC++ and OpenMP Offload does not support 3D yet,
 * so if one them is on we skip this test of 3D. */

#if !defined(USING_OMP_OFFLOAD) && !defined(USING_DPCPP)

TEST_CASE("Isotropic Second Order - 3D - No Window", "[No Window],[3D]") {
    TEST_CASE_SECOND_ORDER_COMPUTATION_KERNEL(
            generate_grid_box(OP_TU_3D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}

TEST_CASE("Isotropic Second Order - 3D - Window", "[Window],[3D]") {
    TEST_CASE_SECOND_ORDER_COMPUTATION_KERNEL(
            generate_grid_box(OP_TU_3D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}

#endif
