/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <utility>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

using namespace bs::base::configurations;


JSONConfigurationMap::JSONConfigurationMap(nlohmann::json aJson)
        : mJson(std::move(aJson)) {}

inline float
JSONConfigurationMap::GetValue(const std::string &aSectionKey,
                               const std::string &aPropertyKey,
                               float aDefaultValue) {
    float val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<float>();
    }
    return val;
}

uint
JSONConfigurationMap::GetValue(const std::string &aSectionKey,
                               const std::string &aPropertyKey,
                               int aDefaultValue) {
    uint val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<uint>();
    }
    return val;
}

double
JSONConfigurationMap::GetValue(const std::string &aSectionKey,
                               const std::string &aPropertyKey,
                               double aDefaultValue) {
    double val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<double>();
    }
    return val;
}

std::string
JSONConfigurationMap::GetValue(const std::string &aSectionKey,
                               const std::string &aPropertyKey,
                               std::string aDefaultValue) {
    std::string val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<std::string>();
    }
    return val;
}

bool
JSONConfigurationMap::GetValue(const std::string &aSectionKey,
                               const std::string &aPropertyKey,
                               bool aDefaultValue) {
    bool val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<bool>();
    }
    return val;
}

size_t
JSONConfigurationMap::Size() {
    return this->mJson.size();
}

bool
JSONConfigurationMap::Contains(const std::string &aSectionKey) {
    return mJson.contains(aSectionKey) && mJson[aSectionKey].is_object();
}


bool
JSONConfigurationMap::Contains(const std::string &aSectionKey,
                               const std::string &aPropertyKey) {
    return this->Contains(aSectionKey) &&
           mJson[aSectionKey].contains(aPropertyKey) &&
           !mJson[aSectionKey][aPropertyKey].is_null();
}

std::string
JSONConfigurationMap::GetValue(const std::string &aPropertyKey,
                               const std::string &aDefaultValue) {
    std::string val = aDefaultValue;
    if (this->Contains(aPropertyKey)) {
        val = this->mJson[aPropertyKey].get<std::string>();
    }
    return val;
}

void
JSONConfigurationMap::WriteValue(const std::string &aSectionKey,
                                 const std::string &aPropertyKey,
                                 std::string aValue) {
    this->mJson[aSectionKey][aPropertyKey] = aValue;
}

std::string
JSONConfigurationMap::ToString() {
    return this->mJson.dump(4);
}

std::vector<bs::base::configurations::ConfigurationMap *>
JSONConfigurationMap::GetConfigurationArray(std::string &aSectionKey) {
    auto temp_map = this->mJson[aSectionKey];
    std::vector<bs::base::configurations::ConfigurationMap *> submaps;

    for (auto &m : temp_map) {
        submaps.push_back(new JSONConfigurationMap(m));
    }
    return submaps;
}

std::vector<bs::base::configurations::ConfigurationMap *>
JSONConfigurationMap::GetConfigurationArray(std::string &aPropertyKey,
                                            std::string &aSectionKey) {
    auto temp_map = this->mJson[aPropertyKey][aSectionKey];
    std::vector<bs::base::configurations::ConfigurationMap *> submaps;

    for (auto &m : temp_map) {
        submaps.push_back(new JSONConfigurationMap(m));
    }
    return submaps;
}

bool
JSONConfigurationMap::HasKey(const std::string &aSectionKey) {
    return mJson.contains(aSectionKey);
}

std::string JSONConfigurationMap::GetKeyValue(const std::string &aPropertyKey,
                                              const std::string &aDefaultValue) {
    std::string val = aDefaultValue;
    if (this->HasKey(aPropertyKey)) {
        val = this->mJson[aPropertyKey].get<std::string>();
    }
    return val;
}

