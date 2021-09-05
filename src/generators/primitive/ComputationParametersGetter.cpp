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

#include <stbx/generators/primitive/ComputationParametersGetter.hpp>

#include <stbx/generators/common/Keys.hpp>

#include <operations/common/DataTypes.h>

#include <prerequisites/libraries/nlohmann/json.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <iostream>

using namespace std;
using namespace stbx::generators;
using namespace bs::base::logger;
using json = nlohmann::json;


ComputationParametersGetter::ComputationParametersGetter(const json &aMap) : mMap(aMap) {}

Window ComputationParametersGetter::GetWindow() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Window w;
    json window_map = this->mMap["window"];
    if (window_map[K_ENABLE].get<bool>()) {
        w.use_window = 1;
        if (!window_map[K_LEFT].is_null()) {
            w.left_win = window_map[K_LEFT].get<int>();
            if (w.left_win < 0) {
                Logger->Error() << "Invalid value entered for left window in x-direction: "
                                   "must be positive..." << '\n';
                w.left_win = DEF_VAL;
            }
        }
        if (!window_map[K_RIGHT].is_null()) {
            w.right_win = window_map[K_RIGHT].get<int>();
            if (w.right_win < 0) {
                Logger->Error() << "Invalid value entered for right window in x-direction: "
                                   "must be positive..." << '\n';
                w.right_win = DEF_VAL;
            }
        }
        if (!window_map[K_DEPTH].is_null()) {
            w.depth_win = window_map[K_DEPTH].get<int>();
            if (w.depth_win < 0) {
                Logger->Error() << "Invalid value entered for depth window in x-direction: "
                                   "must be positive..." << '\n';
                w.depth_win = DEF_VAL;
            }
        }
        if (!window_map[K_FRONT].is_null()) {
            w.front_win = window_map[K_FRONT].get<int>();
            if (w.front_win < 0) {
                Logger->Error() << "Invalid value entered for front window in x-direction: "
                                   "must be positive..." << '\n';
                w.front_win = DEF_VAL;
            }
        }
        if (!window_map[K_BACK].is_null()) {
            w.back_win = window_map[K_BACK].get<int>();
            if (w.back_win < 0) {
                Logger->Error() << "Invalid value entered for back window in x-direction: "
                                   "must be positive..." << '\n';
                w.back_win = DEF_VAL;
            }
        }
    }
    return w;
}

StencilOrder ComputationParametersGetter::GetStencilOrder() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    int value = this->mMap[K_STENCIL_ORDER].get<int>();
    StencilOrder so;
    switch (value) {
        case 2:
            so.order = 2;
            so.half_length = O_2;
            break;
        case 4:
            so.order = 4;
            so.half_length = O_4;
            break;
        case 8:
            so.order = 8;
            so.half_length = O_8;
            break;
        case 12:
            so.order = 12;
            so.half_length = O_12;
            break;
        case 16:
            so.order = 16;
            so.half_length = O_16;
            break;
        default:
            Logger->Error() << "Invalid order value entered for stencil..." << '\n';
            Logger->Info() << "Supported values are : 2, 4, 8, 12, 16" << '\n';
            break;
    }
    return so;
}

int ComputationParametersGetter::GetBoundaryLength() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    int value = this->mMap[K_BOUNDARY_LENGTH].get<int>();
    if (value < 0) {
        Logger->Error() << "Invalid value entered for boundary length: "
                           "must be positive or zero..." << '\n';
        return DEF_VAL;
    }
    return value;
}

float ComputationParametersGetter::GetSourceFrequency() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto value = this->mMap[K_SOURCE_FREQUENCY].get<float>();
    if (value < 0) {
        Logger->Error() << "Invalid value entered for source frequency: must be positive..." << '\n';
        return DEF_VAL;
    }
    return value;
}

float ComputationParametersGetter::GetDTRelaxed() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto value = this->mMap[K_DT_RELAX].get<float>();
    if (value <= 0 || value > 1) {
        Logger->Error() << "Invalid value entered for dt relaxation coefficient: must be larger than 0"
                           " and less than or equal 1..." << '\n';
        return DEF_VAL;
    }
    return value;
}

int ComputationParametersGetter::GetBlock(const std::string &direction) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    json cache_blocking_map = this->mMap[K_CACHE_BLOCKING];
    string block = "block-" + direction;
    int value = cache_blocking_map[block].get<int>();
    if (value <= 0) {
        Logger->Error() << "Invalid value entered for block factor in "
                        << direction << "-direction : must be positive..." << '\n';
        return DEF_VAL;
    }
    return value;
}

int ComputationParametersGetter::GetIsotropicCircle() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    int value = this->mMap[K_ISOTROPIC_CIRCLE].get<int>();
    if (value < 0) {
        Logger->Error() << "Invalid value entered for isotropic circle coefficient: "
                           "Must be larger than 0" << '\n';
        return DEF_VAL;
    }
    return value;
}
