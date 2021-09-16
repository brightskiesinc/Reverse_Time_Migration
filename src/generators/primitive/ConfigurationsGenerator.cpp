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

#include <operations/common/DataTypes.h>
#include <stbx/generators/common/Keys.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <iostream>
#include <string>

using namespace stbx::generators;
using namespace bs::base::logger;


ConfigurationsGenerator::ConfigurationsGenerator(nlohmann::json &aMap) {
    this->mMap = aMap;
}

PHYSICS
ConfigurationsGenerator::GetPhysics() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json wave = this->mMap[K_WAVE];
    PHYSICS physics = ACOUSTIC;
    if (wave[K_PHYSICS].get<std::string>() == "acoustic") {
        physics = ACOUSTIC;
        Logger->Info() << "Using Acoustic for physics" << '\n';
    } else {
        Logger->Error() << "Invalid value for physics key : supported values [ acoustic ]" << '\n';
        Logger->Info() << "Using Acoustic for physics" << '\n';
    }
    return physics;
}

EQUATION_ORDER
ConfigurationsGenerator::GetEquationOrder() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json wave = this->mMap[K_WAVE];
    EQUATION_ORDER order = SECOND;
    if (wave[K_EQUATION_ORDER].get<std::string>() == "second") {
        order = SECOND;
        Logger->Info() << "Using second order wave equation" << '\n';
    } else if (wave[K_EQUATION_ORDER].get<std::string>() == "first") {
        order = FIRST;
        Logger->Info() << "Using first order wave equation" << '\n';
    } else {
        Logger->Error() << "Invalid value for equation-order key : supported values [ second | first ]" << '\n';
        Logger->Info() << "Using second order wave equation" << '\n';
    }
    return order;
}


GRID_SAMPLING
ConfigurationsGenerator::GetGridSampling() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json wave = this->mMap[K_WAVE];
    GRID_SAMPLING sampling = UNIFORM;
    if (wave[K_GRID_SAMPLING].get<std::string>() == "uniform") {
        sampling = UNIFORM;
        Logger->Info() << "Using uniform grid sampling" << '\n';
    } else {
        Logger->Error() << "Invalid value for grid-sampling key : supported values [ uniform ]" << '\n';
        Logger->Info() << "Using uniform grid sampling" << '\n';
    }
    return sampling;
}

APPROXIMATION
ConfigurationsGenerator::GetApproximation() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json wave = this->mMap[K_WAVE];
    APPROXIMATION approximation = ISOTROPIC;
    if (wave[K_APPROXIMATION].get<std::string>() == "isotropic") {
        approximation = ISOTROPIC;
        Logger->Info() << "Using Isotropic as approximation" << '\n';
    } else if (wave[K_APPROXIMATION].get<std::string>() == "vti") {
        approximation = VTI;
        Logger->Info() << "Using VTI as approximation" << '\n';
    } else if (wave[K_APPROXIMATION].get<std::string>() == "tti") {
        approximation = TTI;
        Logger->Info() << "Using TTI as approximation" << '\n';
    } else {
        Logger->Error() << "Invalid value for approximation key : supported values [ isotropic | vti ]" << '\n';
        Logger->Info() << "Using Acoustic for Isotropic" << '\n';
    }
    return approximation;
}


std::map<std::string, std::string>
ConfigurationsGenerator::GetModelFiles() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    std::map<std::string, std::string> model_files;
    if (this->mMap[K_MODELS].empty()) {
        Logger->Error() << "No entry for models key : a value providing the filename"
                           " to the file that contains the directories of model files(each in "
                           "a line) must be provided" << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    nlohmann::json models = this->mMap[K_MODELS];
    if (!models[K_VELOCITY].is_null()) {
        model_files[K_VELOCITY] = models[K_VELOCITY].get<std::string>();
    }
    if (!models[K_DENSITY].is_null()) {
        model_files[K_DENSITY] = models[K_DENSITY].get<std::string>();
    }
    if (!models[K_DELTA].is_null()) {
        model_files[K_DELTA] = models[K_DELTA].get<std::string>();
    }
    if (!models[K_EPSILON].is_null()) {
        model_files[K_EPSILON] = models[K_EPSILON].get<std::string>();
    }
    if (!models[K_THETA].is_null()) {
        model_files[K_THETA] = models[K_THETA].get<std::string>();
    }
    if (!models[K_PHI].is_null()) {
        model_files[K_PHI] = models[K_PHI].get<std::string>();
    }
    Logger->Info() << "The following model files were detected : " << '\n';
    int index = 0;
    for (auto &model_file : model_files) {
        Logger->Info() << "\t" << ++index << ". " << model_file.first << "\t: " << model_file.second << '\n';
    }
    return model_files;
}

std::vector<std::string>
ConfigurationsGenerator::GetTraceFiles() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json traces = this->mMap[K_TRACES];
    std::vector<std::string> traces_files;
    if (traces.empty()) {
        Logger->Error() << "No entry for traces key : it is necessary to provide it, holding"
                           "the following keys : paths(list of file paths containing the data),"
                           " min(number, optional) , max(number, optional),"
                           " sort-type(string, optional)." << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    } else {
        Logger->Info() << "The following trace files were detected : " << '\n';
        for (int i = 0; i < traces[K_PATHS].size(); i++) {
            std::string path = traces[K_PATHS][i].get<std::string>();
            Logger->Info() << "\t" << (i + 1) << ". " << path << '\n';
            traces_files.push_back(path);
        }
    }
    return traces_files;
}

uint ConfigurationsGenerator::GetTracesMin() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json traces = this->mMap[K_TRACES];
    Logger->Info() << "Parsing minimum shot id..." << '\n';
    uint minimum_key;
    if (traces.empty()) {
        Logger->Error() << "No entry for traces key : it is necessary to provide it, holding"
                           "the following keys : paths(list of file paths containing the data),"
                           " min(number, optional) , max(number, optional),"
                           " sort-type(string, optional)." << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    } else {
        if (traces[K_MIN].is_null()) {
            minimum_key = 0;
        } else {
            try {
                minimum_key = traces[K_MIN].get<uint>();
            }
            catch (std::invalid_argument &e) {
                minimum_key = 0;
                Logger->Error() << "Couldn't parse the minimum shot ID to process... Setting to 0" << '\n';
            }
        }
    }
    Logger->Info() << "Minimum allowable ID : " << minimum_key << '\n';
    return minimum_key;
}

uint ConfigurationsGenerator::GetTracesMax() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json traces = this->mMap[K_TRACES];
    Logger->Info() << "Parsing maximum shot id..." << '\n';
    uint maximum_key;
    if (traces.empty()) {
        Logger->Error() << "No entry for traces key : it is necessary to provide it, holding"
                           "the following keys : paths(list of file paths containing the data),"
                           " min(number, optional) , max(number, optional),"
                           " sort-type(string, optional)." << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    } else {
        if (traces[K_MAX].is_null()) {
            maximum_key = std::numeric_limits<uint>::max();
        } else {
            try {
                maximum_key = traces[K_MAX].get<uint>();
            }
            catch (std::invalid_argument &e) {
                maximum_key = std::numeric_limits<uint>::max();
                Logger->Error() << "Couldn't parse the maximum shot ID to process... Setting to "
                                << std::numeric_limits<uint>::max() << '\n';
            }
        }
    }
    Logger->Info() << "Maximum allowable ID : " << maximum_key << '\n';
    return maximum_key;
}

std::string ConfigurationsGenerator::GetTracesSortKey() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json traces = this->mMap[K_TRACES];
    Logger->Info() << "Parsing sort type..." << '\n';
    std::string sort_key = "CSR";
    if (traces.empty()) {
        Logger->Error() << "No entry for traces key : it is necessary to provide it, holding"
                           "the following keys : paths(list of file paths containing the data),"
                           " min(number, optional) , max(number, optional),"
                           " sort-type(string, optional)." << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    } else {
        if (traces[K_SORT_TYPE].is_null()) {
            Logger->Error() << "Couldn't parse  sort type... Setting to CSR" << '\n';
        } else {
            sort_key = traces[K_SORT_TYPE].get<std::string>();
        }
    }
    return sort_key;
}
