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

#include <stbx/parsers/ArgumentsParser.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <getopt.h>

using namespace std;
using namespace stbx::parsers;
using namespace bs::base::logger;

void ArgumentsParser::Parse(std::string &aParameterFile,
                            std::string &aConfigurationFile,
                            std::string &aCallbackFile,
                            std::string &aSystem,
                            std::string &aWritePath,
                            int argc, char **argv) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    int len = (int) string(WORKLOAD_PATH).length();
    string new_workload_path;

    int opt;
    while ((opt = getopt(argc, argv, ":m:p:c:w:e:h")) != -1) {
        switch (opt) {
            case 'm':
                new_workload_path = string(optarg);
                aParameterFile.replace(aParameterFile.begin(),
                                       aParameterFile.begin() + len,
                                       new_workload_path);

                aConfigurationFile.replace(aConfigurationFile.begin(),
                                           aConfigurationFile.begin() + len,
                                           new_workload_path);

                aCallbackFile.replace(aCallbackFile.begin(),
                                      aCallbackFile.begin() + len,
                                      new_workload_path);

                aSystem.replace(aSystem.begin(),
                                aSystem.begin() + len,
                                new_workload_path);
                break;
            case 'p':
                aParameterFile = string(optarg);
                break;
            case 'c':
                aCallbackFile = string(optarg);
                break;
            case 'e':
                aConfigurationFile = string(optarg);
                break;
            case 'w':
                aWritePath = string(optarg);
                break;
            case 'h':
                ArgumentsParser::PrintHelp();
                exit(EXIT_FAILURE);
            case ':':
                Logger->Error() << "Option needs a value" << '\n';
                ArgumentsParser::PrintHelp();
                exit(EXIT_FAILURE);
            case '?':
            default:
                Logger->Error() << "\"Invalid option entered..." << '\n';
                ArgumentsParser::PrintHelp();
                exit(EXIT_FAILURE);
        }
    }
    Logger->Info() << "Using files:" << '\n';
    Logger->Info() << "\t- Computation parameters: " << aParameterFile << '\n';
    Logger->Info() << "\t- Engine configurations: " << aConfigurationFile << '\n';
    Logger->Info() << "\t- Callback configurations: " << aCallbackFile << '\n';
    Logger->Info() << "\t- Write path: " << aWritePath << '\n';
}


void
ArgumentsParser::PrintHelp() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Usage:"
                   << "\t ./Engine <optional-flags>"

                   << "\nOptional flags:"

                   << "\n\t-m <workload-path>"
                      "\n\t\tWorkloads configurations path."
                      "\n\t\tDefault is \"./workloads/bp_model\""

                   << "\n\t-p <computation-parameter-file-path>"
                      "\n\t\tComputation parameter configurations path."
                      "\n\t\tDefault is \"./workloads/bp_model/computation_parameters.json\""

                   << "\n\t-e <engine-configurations-file-path>"
                      "\n\t\tEngine configurations configurations path."
                      "\n\t\tDefault is \"./workloads/bp_model/engine_configuration.json\""

                   << "\n\t-c <callbacks-configurations-file-path>"
                      "\n\t\tCallbacks configurations file path."
                      "\n\t\tDefault is \"./workloads/bp_model/callback_configuration.json\""

                   << "\n\t-w <write path>"
                      "\n\t\tResults write path."
                      "\n\t\tDefault is \"./results\""

                   << "\n\t-h"
                      "\n\t\tHelp window" << '\n';
}
