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

#include <stbx/generators/primitive/CallbacksGenerator.hpp>

#include <stbx/generators/common/Keys.hpp>
#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>
#include <operations/helpers/callbacks/concrete/WriterCallback.h>
#include <operations/helpers/callbacks/concrete/NormWriter.h>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <iostream>
#include <string>

using namespace stbx::generators;
using namespace operations::helpers::callbacks;
using namespace bs::base::logger;


CallbacksGenerator::CallbacksGenerator(const std::string &aWritePath, nlohmann::json &aMap) {
    this->mMap = aMap;
    this->mWritePath = aWritePath;
    this->mpCollection = new CallbackCollection();
}

CallbacksGenerator::~CallbacksGenerator() = default;

CallbackCollection *CallbacksGenerator::GenerateCallbacks() {
    this->GetNormCallback();
    this->GetWriterCallback();

    return this->mpCollection;
}

CallbacksGenerator::WritersBooleans CallbacksGenerator::GenerateWritersConfiguration() {
    struct CallbacksGenerator::WritersBooleans w;
    nlohmann::json map = this->mMap[K_WRITERS_CONFIGURATION];
    if (map[K_PARAMETERS][K_ENABLE].get<bool>()) {
        w.WriteParams = true;
        if (!map[K_PARAMETERS][K_OUTPUT].is_null()) {
            w.VecParams = map[K_PARAMETERS][K_OUTPUT].get<std::vector<std::string>>();
        }
    }
    if (map[K_FORWARD][K_ENABLE].get<bool>()) {
        w.WriteForward = true;
    }
    if (map[K_BACKWARD][K_ENABLE].get<bool>()) {
        w.WriteBackward = true;
    }
    if (map[K_REVERSE][K_ENABLE].get<bool>()) {
        w.WriteReverse = true;
    }
    if (map[K_MIGRATION][K_ENABLE].get<bool>()) {
        w.WriteMigration = true;
    }
    if (map[K_RE_EXTENDED_PARAMETERS][K_ENABLE].get<bool>()) {
        w.WriteReExtendedParams = true;
        if (!map[K_RE_EXTENDED_PARAMETERS][K_OUTPUT].is_null()) {
            w.VecReExtendedParams = map[K_RE_EXTENDED_PARAMETERS][K_OUTPUT].get<std::vector<std::string>>();
        }
    }
    if (map[K_SINGLE_SHOT_CORR][K_ENABLE].get<bool>()) {
        w.WriteSingleShotCorrelation = true;
    }
    if (map[K_EACH_STACKED_SHOT][K_ENABLE].get<bool>()) {
        w.WriteEachStackedShot = true;
    }
    if (map[K_TRACE_RAW][K_ENABLE].get<bool>()) {
        w.WriteTracesRaw = true;
    }
    if (map[K_TRACES_PREPROCESSED][K_ENABLE].get<bool>()) {
        w.WriteTracesPreprocessed = true;
    }
    return w;
}

void CallbacksGenerator::GetNormCallback() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    if (this->mMap[K_NORM][K_ENABLE].get<bool>()) {
        int show_each = 200;
        if (!this->mMap[K_NORM][K_SHOW_EACH].is_null()) {
            show_each = this->mMap[K_NORM][K_SHOW_EACH].get<int>();
        }
        this->mpCollection->RegisterCallback(new NormWriter(show_each,
                                                            true,
                                                            true,
                                                            true,
                                                            this->mWritePath));
        Logger->Info() << "Creating norm callback with show_each = " << show_each << '\n';
    }
}

void CallbacksGenerator::GetWriterCallback() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    if (this->mMap[K_WRITER][K_ENABLE].get<bool>()) {
        int show_each = 200;

        if (!this->mMap[K_WRITER][K_SHOW_EACH].is_null()) {
            show_each = this->mMap[K_WRITER][K_SHOW_EACH].get<int>();
        }
        std::vector<std::string> types;
        std::vector<std::string> underlying_configurations;
        if (!this->mMap[K_WRITER][K_ACTIVE_TYPES].is_null()) {
            auto types_map = this->mMap[K_WRITER][K_ACTIVE_TYPES];
            for (auto it = types_map.begin(); it != types_map.end(); ++it) {
                auto object = it.value();
                types.push_back(object.begin().key());
                underlying_configurations.push_back(object.begin().value().dump());
            }
        }
        CallbacksGenerator::WritersBooleans w = this->GenerateWritersConfiguration();
        Logger->Info() << "Creating writer callback with show_each = " << show_each << '\n';
        this->mpCollection->RegisterCallback(new WriterCallback(show_each,
                                                                w.WriteParams,
                                                                w.WriteForward,
                                                                w.WriteBackward,
                                                                w.WriteReverse,
                                                                w.WriteMigration,
                                                                w.WriteReExtendedParams,
                                                                w.WriteSingleShotCorrelation,
                                                                w.WriteEachStackedShot,
                                                                w.WriteTracesRaw,
                                                                w.WriteTracesPreprocessed,
                                                                w.VecParams,
                                                                w.VecReExtendedParams,
                                                                this->mWritePath,
                                                                types, underlying_configurations));
    }
}
