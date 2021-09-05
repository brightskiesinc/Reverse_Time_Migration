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

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>

#include <prerequisites/libraries/catch/catch.hpp>
#include <prerequisites/libraries/nlohmann/json.hpp>

using namespace stbx::generators;
using namespace std;


void TEST_CASE_CONFIGURATIONS_GENERATOR() {
    SECTION("WaveGetter")
    {
        nlohmann::json map = R"(
           {
  "wave": {
    "physics": "acoustic",
    "approximation": "isotropic",
    "equation-order": "first",
    "grid-sampling": "uniform"
  }
}
    )"_json;
        auto *configurations_generator = new ConfigurationsGenerator(map);

        SECTION("GetPhysics Function Testing")
        {
            PHYSICS physics = configurations_generator->GetPhysics();
            REQUIRE(physics == ACOUSTIC);
        }

        SECTION("GetEquationOrder Function Testing")
        {
            EQUATION_ORDER order = configurations_generator->GetEquationOrder();
            REQUIRE(order == FIRST);
        }

        SECTION("GetGridSampling Function Testing")
        {
            GRID_SAMPLING sampling = configurations_generator->GetGridSampling();
            REQUIRE(sampling == UNIFORM);
        }

        SECTION("GetApproximation")
        {
            APPROXIMATION approximation = configurations_generator->GetApproximation();
            REQUIRE(approximation == ISOTROPIC);
        }
    }

    SECTION("FileGetter Function Testing")
    {
        nlohmann::json map = R"(
         {
  "traces": {
    "min": 0,
    "max": 601,
    "sort-type": "CSR",
    "paths": [
      "path test"
    ]
  },
  "models": {
    "velocity": "velocity test",
    "density": "density test",
    "delta": "delta test",
    "epsilon": "epsilon test",
    "theta": "theta test",
    "phi": "phi test"
  },
  "modelling-file": "modelling-file test",
  "output-file": "output-file test",

  "wave": {
    "physics": "acoustic",
    "approximation": "isotropic",
    "equation-order": "first",
    "grid-sampling": "uniform"
  }
}
    )"_json;
        auto *configurations_generator = new ConfigurationsGenerator(map);

        SECTION("GetModelFiles Function Testing")
        {
            std::map<string, string> model_files = configurations_generator->GetModelFiles();
            REQUIRE(model_files["velocity"] == "velocity test");
            REQUIRE(model_files["density"] == "density test");
            REQUIRE(model_files["delta"] == "delta test");
            REQUIRE(model_files["theta"] == "theta test");
            REQUIRE(model_files["phi"] == "phi test");
        }

        SECTION("GetTraceFiles Function Testing")
        {
            vector <string> traces_files = configurations_generator->GetTraceFiles();

            REQUIRE(configurations_generator->GetTracesMin() == 0);
            REQUIRE(configurations_generator->GetTracesMax() == 601);
            REQUIRE(configurations_generator->GetTracesSortKey() == "CSR");
            REQUIRE(traces_files.back() == "path test");
        }
    }
}

TEST_CASE("ConfigurationsGenerator", "[Generator],[ConfigurationsGenerator]") {
TEST_CASE_CONFIGURATIONS_GENERATOR();

}
