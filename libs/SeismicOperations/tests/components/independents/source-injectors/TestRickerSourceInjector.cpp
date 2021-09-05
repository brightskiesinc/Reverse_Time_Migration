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


#include <operations/components/independents/concrete/source-injectors/RickerSourceInjector.hpp>

#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>
#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>
#include <operations/test-utils/NumberHelpers.hpp>
#include <operations/test-utils/EnvironmentHandler.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace std;
using namespace bs::base::configurations;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::testutils;


void TEST_CASE_RICKER_SOURCE_INJECTOR_TI(GridBox *apGridBox,
                                         ComputationParameters *apParameters,
                                         ConfigurationMap *apConfigurationMap) {
    /*
     * Register and allocate parameters and wave fields in
     * grid box according to the current test case.
     */

    auto pressure_curr_horizontal = new FrameBuffer<float>();
    auto pressure_prev_horizontal = new FrameBuffer<float>();

    auto pressure_curr_vertical = new FrameBuffer<float>();
    auto pressure_prev_vertical = new FrameBuffer<float>();

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

    pressure_curr_horizontal->Allocate(window_size);
    pressure_prev_horizontal->Allocate(window_size);

    pressure_curr_vertical->Allocate(window_size);
    pressure_prev_vertical->Allocate(window_size);

    velocity->Allocate(size);

    apGridBox->RegisterWaveField(WAVE | GB_PRSS | CURR | DIR_Z, pressure_curr_vertical);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | PREV | DIR_Z, pressure_prev_vertical);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | NEXT | DIR_Z, pressure_prev_vertical);

    apGridBox->RegisterWaveField(WAVE | GB_PRSS | CURR | DIR_X, pressure_curr_horizontal);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | PREV | DIR_X, pressure_prev_horizontal);
    apGridBox->RegisterWaveField(WAVE | GB_PRSS | NEXT | DIR_X, pressure_prev_horizontal);

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
    Device::MemSet(pressure_curr_horizontal->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemSet(pressure_prev_horizontal->GetNativePointer(), 0.0f, window_size * sizeof(float));

    Device::MemSet(pressure_curr_vertical->GetNativePointer(), 0.0f, window_size * sizeof(float));
    Device::MemSet(pressure_prev_vertical->GetNativePointer(), 0.0f, window_size * sizeof(float));

    Device::MemCpy(velocity->GetNativePointer(), temp_vel, size * sizeof(float), Device::COPY_HOST_TO_DEVICE);

    auto ricker_source_injector = new RickerSourceInjector(apConfigurationMap);
    auto source_point = new Point3D(0, 0, 0);
    uint location = 0;

    ricker_source_injector->SetGridBox(apGridBox);
    ricker_source_injector->SetComputationParameters(apParameters);
    ricker_source_injector->SetSourcePoint(source_point);

    SECTION("GetCutOffTimeStep")
    {
        uint cut_off_frequency = (1.0 / apParameters->GetSourceFrequency()) / apGridBox->GetDT();
        REQUIRE(ricker_source_injector->GetCutOffTimeStep() == Approx(cut_off_frequency));
    }

    SECTION("ApplySource")
    {
        // Calculated by hand with time step=1 & dt=1 & source frequency=20
        float ricker = 1;
        ricker = ricker * velocity->GetHostPointer()[location];
        float ground_truth_horizontal =
                pressure_curr_horizontal->GetHostPointer()[location] + ricker;
        float ground_truth_vertical =
                pressure_curr_vertical->GetHostPointer()[location] + ricker;

        uint time_step = 0;
        ricker_source_injector->ApplySource(time_step);

        float pressure_after_ricker_horizontal = apGridBox->Get(
                WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer()[location];
        float pressure_after_ricker_vertical = apGridBox->Get(
                WAVE | GB_PRSS | CURR | DIR_X)->GetHostPointer()[location];

        REQUIRE(ground_truth_horizontal == Approx(pressure_after_ricker_horizontal));
        REQUIRE(ground_truth_vertical == Approx(pressure_after_ricker_vertical));
    }
}

void TEST_CASE_RICKER_SOURCE_INJECTOR_ISO(GridBox *apGridBox,
                                          ComputationParameters *apParameters,
                                          ConfigurationMap *apConfigurationMap) {
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

    auto ricker_source_injector = new RickerSourceInjector(apConfigurationMap);
    auto source_point = new Point3D(0, 0, 0);
    uint location = 0;

    ricker_source_injector->SetGridBox(apGridBox);
    ricker_source_injector->SetComputationParameters(apParameters);
    ricker_source_injector->SetSourcePoint(source_point);

    SECTION("GetCutOffTimeStep")
    {
        uint cut_off_frequency = (1.0 / apParameters->GetSourceFrequency()) / apGridBox->GetDT();
        REQUIRE(ricker_source_injector->GetCutOffTimeStep() == Approx(cut_off_frequency));
    }

    SECTION("ApplySource")
    {
        // Calculated by hand with time step=1 & dt=1 & source frequency=20
        float ricker = 1;
        ricker = ricker * velocity->GetHostPointer()[location];
        float ground_truth = pressure_curr->GetHostPointer()[location] + ricker;

        uint time_step = 0;
        ricker_source_injector->ApplySource(time_step);
        float pressure_after_ricker =
                apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer()[location];

        REQUIRE(ground_truth == Approx(pressure_after_ricker));
    }
}

void TEST_CASE_RICKER_SOURCE_INJECTOR(GridBox *apGridBox,
                                      ComputationParameters *apParameters,
                                      ConfigurationMap *apConfigurationMap) {
    /*
     * Environment setting (i.e. Backend setting initialization).
     */
    set_environment();

    if (apParameters->GetApproximation() == ISOTROPIC) {
        TEST_CASE_RICKER_SOURCE_INJECTOR_ISO(apGridBox, apParameters, apConfigurationMap);
    } else if (apParameters->GetApproximation() == VTI ||
               apParameters->GetApproximation() == TTI) {
#ifdef USING_OMP
        TEST_CASE_RICKER_SOURCE_INJECTOR_TI(apGridBox, apParameters, apConfigurationMap);
#endif
    }
}

/*
 * Isotropic Test Cases
 */

TEST_CASE("RickerSourceInjector - 2D - No Window - ISO", "[No Window],[2D],[ISO]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
        generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 2D - Window - ISO", "[Window],[2D],[ISO]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
        generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 3D - No Window - ISO", "[No Window],[3D],[ISO]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_3D, OP_TU_NO_WIND),
        generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 3D - Window - ISO", "[Window],[3D],[ISO]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_3D, OP_TU_INC_WIND),
        generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
        generate_average_case_configuration_map_wave()
);
}

/*
 * TI (i.e. VTI & TTI) Test Cases
 */

TEST_CASE("RickerSourceInjector - 2D - No Window - VTI", "[No Window],[2D],[VTI]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
        generate_computation_parameters(OP_TU_NO_WIND, VTI),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 2D - Window - TI", "[Window],[2D],[TI]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
        generate_computation_parameters(OP_TU_INC_WIND, VTI),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 3D - No Window - TI", "[No Window],[3D],[TI]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_3D, OP_TU_NO_WIND),
        generate_computation_parameters(OP_TU_NO_WIND, VTI),
        generate_average_case_configuration_map_wave()
);
}

TEST_CASE("RickerSourceInjector - 3D - Window - TI", "[Window],[3D],[TI]") {
TEST_CASE_RICKER_SOURCE_INJECTOR(
        generate_grid_box(OP_TU_3D, OP_TU_INC_WIND),
        generate_computation_parameters(OP_TU_INC_WIND, VTI),
        generate_average_case_configuration_map_wave()
);
}
