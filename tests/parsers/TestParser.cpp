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

#include <stbx/parsers/Parser.hpp>

#include <stbx/test-utils/utils.h>

#include <prerequisites/libraries/catch/catch.hpp>

using namespace std;
using namespace stbx::parsers;
using namespace stbx::testutils;
using json = nlohmann::json;


void TEST_CASE_PARSER() {
    SECTION("Register Files Function Test")
    {
        auto *parser = Parser::GetInstance();

        string parameter_file =
                STBX_TEST_DATA_PATH "/workloads/computation_parameters.json";
        string configuration_file =
                STBX_TEST_DATA_PATH "/workloads/engine_configuration.json";
        string callback_file =
                STBX_TEST_DATA_PATH "/workloads/callback_configuration.json";
        string pipeline =
                STBX_TEST_DATA_PATH "/workloads/pipeline.json";

        json ground_truth_map = R"(
{
  "callbacks": {
    "bin": {
      "enable": true,
      "show-each": 200
    },
    "csv": {
      "enable": true,
      "show-each": 200
    },
    "image": {
      "enable": true,
      "percentile": 98.5,
      "show-each": 200
    },
    "norm": {
      "enable": true,
      "show-each": 200
    },
    "segy": {
      "enable": true,
      "show-each": 200
    },
    "su": {
      "enable": true,
      "little-endian": false,
      "show-each": 200
    },
    "writers": {
      "backward": {
        "enable": true
      },
      "each-stacked-shot": {
        "enable": true
      },
      "forward": {
        "enable": true
      },
      "migration": {
        "enable": true
      },
      "re-extended-velocity": {
        "enable": true
      },
      "reverse": {
        "enable": true
      },
      "single-shot-correlation": {
        "enable": true
      },
      "traces-preprocessed": {
        "enable": true
      },
      "traces-raw": {
        "enable": true
      },
      "velocity": {
        "enable": true
      }
    }
  },
  "components": {
    "boundary-manager": {
      "properties": {
        "use-top-layer": false
      },
      "type": "none"
    },
    "forward-collector": {
      "type": "three"
    },
    "migration-accommodator": {
      "properties": {
        "compensation": "no"
      },
      "type": "cross-correlation"
    },
    "model-handler": {
      "type": "segy"
    },
    "source-injector": {
      "type": "ricker"
    },
    "trace-manager": {
      "properties": {
        "shot-stride": 2
      },
      "type": "segy"
    }
  },
  "computation-parameters": {
    "algorithm": "cpu",
    "boundary-length": 20,
    "cache-blocking": {
      "block-x": 5500,
      "block-y": 1,
      "block-z": 55
    },
    "device": "none",
    "dt-relax": 0.9,
    "isotropic-radius": 5,
    "source-frequency": 20,
    "stencil-order": 8,
    "window": {
      "back": 0,
      "depth": 500,
      "enable": true,
      "front": 0,
      "left": 250,
      "right": 250
    }
  },
  "interpolation": {
    "type": "none"
  },
  "models": {
    "velocity": "data/iso/params/vel_z6.25m_x12.5m_exact.segy"
  },
  "system": {
    "agent": {
      "type": "normal"
    },
    "writer": {
      "type": "normal"
    }
  },
  "traces": {
    "max": 601,
    "min": 601,
    "paths": [
      "data/iso/shots/shots0601_0800.segy"
    ],
    "sort-type": "CSR"
  },
  "wave": {
    "approximation": "isotropic",
    "equation-order": "first",
    "grid-sampling": "uniform",
    "physics": "acoustic"
  }
}
)"_json;


        SECTION("GetInstance Function Test")
        {
            REQUIRE(instanceof<Parser>(parser));
        }

        SECTION("RegisterFile & GetFiles Functions Test")
        {
            parser->RegisterFile(parameter_file);
            parser->RegisterFile(configuration_file);
            parser->RegisterFile(callback_file);
            parser->RegisterFile(pipeline);

            vector<std::string> files = parser->GetFiles();

            REQUIRE(std::find(files.begin(), files.end(), parameter_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), configuration_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), callback_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), pipeline) != files.end());
        }

        SECTION("BuildMap Function Test")
        {
            json map = parser->BuildMap();

            REQUIRE(map == ground_truth_map);
        }

        SECTION("GetMap Function Test")
        {
            REQUIRE(parser->GetMap() == ground_truth_map);
        }

        SECTION("Parser Class Kill Function Test")
        {
            REQUIRE(Parser::Kill() == nullptr);
        }
    }

    SECTION("Register Folder Function Test")
    {
        auto *parser = Parser::GetInstance();

        string workloads =
                STBX_TEST_DATA_PATH "/workloads/";
        string parameter_file =
                STBX_TEST_DATA_PATH "/workloads/computation_parameters.json";
        string configuration_file =
                STBX_TEST_DATA_PATH "/workloads/engine_configuration.json";
        string callback_file =
                STBX_TEST_DATA_PATH "/workloads/callback_configuration.json";
        string pipeline =
                STBX_TEST_DATA_PATH "/workloads/pipeline.json";

        json ground_truth_map = R"(
{
  "callbacks": {
    "bin": {
      "enable": true,
      "show-each": 200
    },
    "csv": {
      "enable": true,
      "show-each": 200
    },
    "image": {
      "enable": true,
      "percentile": 98.5,
      "show-each": 200
    },
    "norm": {
      "enable": true,
      "show-each": 200
    },
    "segy": {
      "enable": true,
      "show-each": 200
    },
    "su": {
      "enable": true,
      "little-endian": false,
      "show-each": 200
    },
    "writers": {
      "backward": {
        "enable": true
      },
      "each-stacked-shot": {
        "enable": true
      },
      "forward": {
        "enable": true
      },
      "migration": {
        "enable": true
      },
      "re-extended-velocity": {
        "enable": true
      },
      "reverse": {
        "enable": true
      },
      "single-shot-correlation": {
        "enable": true
      },
      "traces-preprocessed": {
        "enable": true
      },
      "traces-raw": {
        "enable": true
      },
      "velocity": {
        "enable": true
      }
    }
  },
  "components": {
    "boundary-manager": {
      "properties": {
        "use-top-layer": false
      },
      "type": "none"
    },
    "forward-collector": {
      "type": "three"
    },
    "migration-accommodator": {
      "properties": {
        "compensation": "no"
      },
      "type": "cross-correlation"
    },
    "model-handler": {
      "type": "segy"
    },
    "source-injector": {
      "type": "ricker"
    },
    "trace-manager": {
      "properties": {
        "shot-stride": 2
      },
      "type": "segy"
    }
  },
  "computation-parameters": {
    "algorithm": "cpu",
    "boundary-length": 20,
    "cache-blocking": {
      "block-x": 5500,
      "block-y": 1,
      "block-z": 55
    },
    "device": "none",
    "dt-relax": 0.9,
    "isotropic-radius": 5,
    "source-frequency": 20,
    "stencil-order": 8,
    "window": {
      "back": 0,
      "depth": 500,
      "enable": true,
      "front": 0,
      "left": 250,
      "right": 250
    }
  },
  "interpolation": {
    "type": "none"
  },
  "models": {
    "velocity": "data/iso/params/vel_z6.25m_x12.5m_exact.segy"
  },
  "system": {
    "agent": {
      "type": "normal"
    },
    "writer": {
      "type": "normal"
    }
  },
  "traces": {
    "max": 601,
    "min": 601,
    "paths": [
      "data/iso/shots/shots0601_0800.segy"
    ],
    "sort-type": "CSR"
  },
  "wave": {
    "approximation": "isotropic",
    "equation-order": "first",
    "grid-sampling": "uniform",
    "physics": "acoustic"
  }
}
)"_json;

        SECTION("GetInstance Function Test")
        {
            REQUIRE(instanceof<Parser>(parser));
        }

        SECTION("RegisterFolder Function Test")
        {
            parser->RegisterFolder(workloads);
            vector<std::string> files = parser->GetFiles();

            REQUIRE(std::find(files.begin(), files.end(), parameter_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), configuration_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), callback_file) != files.end());
            REQUIRE(std::find(files.begin(), files.end(), pipeline) != files.end());
        }

        SECTION("BuildMap Function Test")
        {
            json map = parser->BuildMap();

            REQUIRE(map == ground_truth_map);
        }

        SECTION("GetMap Function Test")
        {
            REQUIRE(parser->GetMap() == ground_truth_map);
        }

        SECTION("Parser Class Kill Function Test")
        {
            REQUIRE(Parser::Kill() == nullptr);
        }
    }
}

TEST_CASE("Parser Class Tess", "[Parser]") {
    TEST_CASE_PARSER();

}
