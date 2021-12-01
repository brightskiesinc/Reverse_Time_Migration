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

#include <map>
#include <string>

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <operations/components/independents/concrete/model-handlers/SeismicModelHandler.hpp>
#include <operations/common/DataTypes.h>
#include <operations/test-utils/dummy-data-generators/DummyConfigurationMapGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>
#include <operations/test-utils/dummy-data-generators/DummyModelGenerator.hpp>
#include <operations/test-utils/NumberHelpers.hpp>
#include <operations/test-utils/EnvironmentHandler.hpp>
#include <operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp>
#include <operations/components/dependency/concrete/HasDependents.hpp>

using namespace std;
using namespace bs::base::configurations;
using namespace bs::base::logger;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::testutils;
using namespace operations::helpers;


void TEST_CASE_SEISMIC_MODEL_HANDLER_CORE(GridBox *apGridBox,
                                          ComputationParameters *apParameters) {
    SECTION("LogicalGridSize") {

        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize() == 23);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() == 23);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize() == 23);

    }SECTION("CellDimensions") {

        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension() == Approx(10));
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension() == Approx(10));
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension() == Approx(10));
    }SECTION("ReferencePoint") {

        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetReferencePoint() == 0);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint() == 0);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetZAxis().GetReferencePoint() == 0);
    }SECTION("WindowStart") {
        REQUIRE(apGridBox->GetWindowStart(X_AXIS) == 0);
        REQUIRE(apGridBox->GetWindowStart(Y_AXIS) == 0);
        REQUIRE(apGridBox->GetWindowStart(Z_AXIS) == 0);
    }SECTION("LogicalWindowSize") {

        REQUIRE(apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() == 23);
        REQUIRE(apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() == 23);
        REQUIRE(apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() == 23);

    }SECTION("ActualGridSize") {
#if defined(USING_DPCPP)

        if (apParameters->IsUsingWindow()) {
            REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize() == 23);
        } else {
            REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize() == 32);
        }

#else
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize() == 23);

#endif

        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize() == 23);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize() == 23);

    }

    SECTION("ActualWindowSize") {
#if defined(USING_DPCPP)
        REQUIRE(apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize() == 32);

#else
        REQUIRE(apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize() == 23);

#endif

        REQUIRE(apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize() == 23);
        REQUIRE(apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize() == 23);

    }SECTION("ComputationGridSize") {

        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetXAxis().GetComputationAxisSize() == 15);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetYAxis().GetComputationAxisSize() == 15);
        REQUIRE(apGridBox->GetAfterSamplingAxis()->GetZAxis().GetComputationAxisSize() == 15);

    }
}

void TEST_CASE_SEISMIC_MODEL_HANDLER_APPROXIMATION(GridBox *apGridBox,
                                                   ComputationParameters *apParameters) {
    SECTION("Wave Allocation") {
        REQUIRE(apGridBox->Get(WAVE | GB_PRSS | CURR)->GetNativePointer() != nullptr);
        REQUIRE(apGridBox->Get(WAVE | GB_PRSS | PREV)->GetNativePointer() != nullptr);
        REQUIRE(apGridBox->Get(WAVE | GB_PRSS | NEXT)->GetNativePointer() != nullptr);
    }

    if (apParameters->GetApproximation() == ISOTROPIC) {
        SECTION("Parameters Allocation") {
            REQUIRE(apGridBox->Get(PARM | GB_VEL)->GetNativePointer() != nullptr);
            REQUIRE(apGridBox->Get(PARM | WIND | GB_VEL)->GetNativePointer() != nullptr);
        }
    }
}

void TEST_CASE_SEISMIC_MODEL_HANDLER(GridBox *apGridBox,
                                     ComputationParameters *apParameters,
                                     ConfigurationMap *apConfigurationMap) {
    /*
     * Environment setting (i.e. Backend setting initialization).
     */
    set_environment();

    auto model_handler = new SeismicModelHandler(apConfigurationMap);
    model_handler->SetComputationParameters(apParameters);

    auto memory_handler = new WaveFieldsMemoryHandler(apConfigurationMap);
    memory_handler->SetComputationParameters(apParameters);

    auto dependant_components_map = new ComponentsMap<DependentComponent>();
    dependant_components_map->Set(MEMORY_HANDLER, memory_handler);
    model_handler->SetDependentComponents(dependant_components_map);

    /*
     * Generates a dummy *.segy file
     */

//    generate_dummy_model("dummy_model");
//
//    map<std::string, std::string> file_names;
//    file_names["velocity"] = OPERATIONS_TEST_DATA_PATH "/dummy_model.segy";
//
//    auto grid_box = model_handler->ReadModel(file_names);
//
//    remove(OPERATIONS_TEST_DATA_PATH "/dummy_model.segy");
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Deprecated. Model Handler is not tested" << '\n';
//    TEST_CASE_SEISMIC_MODEL_HANDLER_CORE(grid_box, apParameters);
//    TEST_CASE_SEISMIC_MODEL_HANDLER_APPROXIMATION(grid_box, apParameters);

//    delete grid_box;
    delete model_handler;
    delete memory_handler;
    delete dependant_components_map;

    delete apGridBox;
    delete apParameters;
    delete apConfigurationMap;
}

/*
 * Isotropic Test Cases
 */

TEST_CASE("SeismicModelHandler - 2D - No Window - ISO", "[No Window],[2D],[ISO]") {
    TEST_CASE_SEISMIC_MODEL_HANDLER(
            generate_grid_box(OP_TU_2D, OP_TU_NO_WIND),
            generate_computation_parameters(OP_TU_NO_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}

TEST_CASE("SeismicModelHandler - 2D - Window - ISO", "[Window],[2D],[ISO]") {
    TEST_CASE_SEISMIC_MODEL_HANDLER(
            generate_grid_box(OP_TU_2D, OP_TU_INC_WIND),
            generate_computation_parameters(OP_TU_INC_WIND, ISOTROPIC),
            generate_average_case_configuration_map_wave());
}