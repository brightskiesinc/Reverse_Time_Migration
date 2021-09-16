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


#include <operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp>

#include <operations/common/DataTypes.h>
#include <operations/utils/io/write_utils.h>
#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyTraceGenerator.hpp>
#include <operations/test-utils/NumberHelpers.hpp>
#include <operations/test-utils/EnvironmentHandler.hpp>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace std;
using namespace bs::base::configurations;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::testutils;
using namespace operations::utils::io;

#define TRACE_STRIDE_X 3
#define TRACE_STRIDE_Y 4
#define TRACE_STRIDE_Z 0


void TEST_CASE_TRACE_MANAGER(GridBox *apGridBox,
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


    auto uut = new SeismicTraceManager(apConfigurationMap);

    uut->SetComputationParameters(apParameters);
    uut->SetGridBox(apGridBox);
    uut->AcquireConfiguration();

    /*
     * Generates a dummy *.segy file
     */

    std::stringstream ss;
    ss << OPERATIONS_TEST_DATA_PATH << "/dummy_trace_" << wnx << "_" << wnz << "_" << wny << std::endl;
    std::string file_name = ss.str();
    auto ground_truth = generate_dummy_trace(file_name, apGridBox,
                                             TRACE_STRIDE_X,
                                             TRACE_STRIDE_Y);
    file_name += ".segy";
    std::vector<std::string> files;
    files.push_back(file_name);

    auto shots = uut->GetWorkingShots(files, 0, wnx * wny, "CSR");

    int theoretical_shot_count = (wnx / TRACE_STRIDE_X);

    if (wny > 1) {
        theoretical_shot_count *= (wny / TRACE_STRIDE_Y);
    }
    int shot_count = shots.size();
    REQUIRE(shot_count != 0);
    REQUIRE(shot_count == theoretical_shot_count);

    int shot_id = 0;

    uut->ReadShot(files, shots[shot_id], "CSR");
    uut->PreprocessShot();

    REQUIRE(uut->GetTracesHolder()->Traces != nullptr);
    REQUIRE(uut->GetTracesHolder()->PositionsX != nullptr);
    REQUIRE(uut->GetTracesHolder()->PositionsY != nullptr);

    auto source_point = uut->GetSourcePoint();
    int start_idx = apParameters->GetBoundaryLength() + apParameters->GetHalfLength();

    REQUIRE(source_point->x == start_idx);
    REQUIRE(source_point->z == start_idx);
    REQUIRE(source_point->y == start_idx * (apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() != 1));

    REQUIRE(uut->GetTracesHolder()->PositionsX[0] == start_idx);
    REQUIRE(uut->GetTracesHolder()->PositionsY[0] ==
            start_idx * (apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() != 1));
    REQUIRE(uut->GetTracesHolder()->SampleNT == wnz);
    REQUIRE(uut->GetTracesHolder()->TraceSizePerTimeStep == 1);
    REQUIRE(uut->GetTracesHolder()->ReceiversCountX == 1);
    REQUIRE(uut->GetTracesHolder()->ReceiversCountY == 1);

    int stride_x = wnx / TRACE_STRIDE_X;
    auto traces = uut->GetTracesHolder()->Traces->GetHostPointer();
    for (int i = 0; i < wnz; i++) {
        auto diff = ground_truth[i * stride_x + shot_id] - traces[i];
        REQUIRE(diff == Approx(diff).margin(std::numeric_limits<float>::epsilon()));
    }

    uint pos = (apParameters->GetBoundaryLength() + apParameters->GetHalfLength()) *
               wnx + uut->GetTracesHolder()->PositionsX[0] +
               uut->GetTracesHolder()->PositionsY[0] * wnx * wnz;

    uut->ApplyTraces(apGridBox->GetNT() - 1);
    traces = uut->GetTracesHolder()->Traces->GetHostPointer();
    float test_value = traces[wnz - 1] * 1500 * 1500 * dt * dt;

    REQUIRE(approximately_equal(pressure_curr->GetHostPointer()[pos], test_value, 0.01));

    uut->ApplyTraces(1);
    traces = uut->GetTracesHolder()->Traces->GetHostPointer();
    test_value += traces[0] * 1500 * 1500 * dt * dt;


    REQUIRE(approximately_equal(pressure_curr->GetHostPointer()[pos], test_value, 0.01));

    uut->ApplyTraces(apGridBox->GetNT() / 2);
    traces = uut->GetTracesHolder()->Traces->GetHostPointer();
    test_value += traces[wnz / 2] * 1500 * 1500 * dt * dt;

    REQUIRE(approximately_equal(pressure_curr->GetHostPointer()[pos], test_value, 0.01));

    remove(file_name.c_str());

    delete ground_truth;

    delete pressure_curr;
    delete pressure_prev;
    delete velocity;

    delete apGridBox;
    delete apParameters;
    delete apConfigurationMap;
    delete uut;
}

TEST_CASE("SeismicTraceManager - 2D - No Window", "[No Window],[2D]") {
    TEST_CASE_TRACE_MANAGER(
            generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave()
    );
}

TEST_CASE("SeismicTraceManager - 2D - Window", "[Window],[2D]") {
    TEST_CASE_TRACE_MANAGER(
            generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave()
    );
}

TEST_CASE("SeismicTraceManager - 3D - No Window", "[No Window],[3D]") {
    TEST_CASE_TRACE_MANAGER(
            generate_grid_box(OP_TU_3D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave()
    );
}

TEST_CASE("SeismicTraceManager - 3D - Window", "[Window],[3D]") {
    TEST_CASE_TRACE_MANAGER(
            generate_grid_box(OP_TU_3D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave()
    );
}
