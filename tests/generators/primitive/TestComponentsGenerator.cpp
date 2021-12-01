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

#include <string>

#include <prerequisites/libraries/catch/catch.hpp>
#include <prerequisites/libraries/nlohmann/json.hpp>

#include  <stbx/generators/primitive/ComponentsGenerator.hpp>

#include <stbx/test-utils/utils.h>

using namespace bs::base::exceptions;
using namespace bs::base::configurations;

using namespace stbx::generators;
using namespace stbx::testutils;

using namespace operations::common;
using namespace operations::components;

void TEST_CASE_COMPONENTS_GENERATOR() {
    nlohmann::json map = R"(
{
   "boundary-manager": {
      "type": "none",
      "properties": {
        "use-top-layer": false
      }
    },
    "migration-accommodator": {
      "type": "cross-correlation",
      "properties": {
        "compensation": "no"
      }
    },
    "forward-collector": {
      "type": "three"
    },
    "trace-manager": {
      "type": "segy",
      "properties": {
        "shot-stride" : 2
      }
    },
    "source-injector": {
      "type": "ricker"
    },
    "model-handler": {
      "type": "segy"
    },
    "trace-writer": {
          "type": "binary"
    },
    "modelling-configuration-parser": {
      "type": "text"
    }

}
)"_json;
    APPROXIMATION approximation = ISOTROPIC;
    EQUATION_ORDER order = SECOND;
    GRID_SAMPLING sampling = UNIFORM;

    auto components_generator = new ComponentsGenerator(map, order, sampling, approximation);

    SECTION("GenerateComputationKernel") {
        auto computation_kernel = components_generator->GenerateComputationKernel();
        REQUIRE(instanceof<ComputationKernel>(computation_kernel));
        delete computation_kernel;
    }

    SECTION("GenerateModelHandler") {
        auto model_handler = components_generator->GenerateModelHandler();
        REQUIRE(instanceof<ModelHandler>(model_handler));
        delete model_handler;
    }

    SECTION("GenerateSourceInjector") {
        auto source_injector = components_generator->GenerateSourceInjector();
        REQUIRE(instanceof<SourceInjector>(source_injector));
        delete source_injector;
    }

    SECTION("GenerateBoundaryManager") {
        auto boundary_manager = components_generator->GenerateBoundaryManager();
        REQUIRE(instanceof<BoundaryManager>(boundary_manager));
        delete boundary_manager;
    }

    SECTION("GenerateForwardCollector") {
        auto forward_collector = components_generator->GenerateForwardCollector(WRITE_PATH);
        REQUIRE(instanceof<ForwardCollector>(forward_collector));
    }

    SECTION("GenerateMigrationAccommodator") {
        auto migration_accommodator = components_generator->GenerateMigrationAccommodator();
        REQUIRE(instanceof<MigrationAccommodator>(migration_accommodator));
    }

    SECTION("GenerateTraceManager") {
        auto trace_manager = components_generator->GenerateTraceManager();
        REQUIRE(instanceof<TraceManager>(trace_manager));
        delete trace_manager;
    }

    SECTION("GenerateTraceWriter") {
        auto trace_writer = components_generator->GenerateTraceWriter();
        REQUIRE(instanceof<TraceWriter>(trace_writer));
        delete trace_writer;
    }
}

TEST_CASE("ComponentsGenerator Class", "[Generator],[ComponentsGenerator]") {
    TEST_CASE_COMPONENTS_GENERATOR();
}
