//
// Created by zeyad-osama on 29/11/2020.
//

#include <operations/configurations/concrete/JSONConfigurationMap.hpp>

#include <sstream>
#include <utility>

using namespace operations::configuration;
using json = nlohmann::json;

JSONConfigurationMap::JSONConfigurationMap(json aJson)
        : mJson(std::move(aJson)) {}

inline float JSONConfigurationMap::GetValue(const std::string &aSectionKey,
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
JSONConfigurationMap::GetValue(const std::string &aSectionKey, const std::string &aPropertyKey, bool aDefaultValue) {
    bool val = aDefaultValue;
    if (this->Contains(aSectionKey, aPropertyKey)) {
        val = this->mJson[aSectionKey][aPropertyKey].get<bool>();
    }
    return val;
}

int JSONConfigurationMap::Size() {
    return this->mJson.size();
}

bool JSONConfigurationMap::Contains(const std::string &aSectionKey) {
    return mJson.contains(aSectionKey) && mJson[aSectionKey].is_object();
}

bool JSONConfigurationMap::Contains(const std::string &aSectionKey, const std::string &aPropertyKey) {
    return this->Contains(aSectionKey) && mJson[aSectionKey].contains(aPropertyKey) &&
           !mJson[aSectionKey][aPropertyKey].is_null();
}

std::string JSONConfigurationMap::GetValue(const std::string &aPropertyKey,
                                           const std::string &aDefaultValue) {
    std::string val = aDefaultValue;
    if (this->Contains(aPropertyKey)) {
        val = this->mJson[aPropertyKey].get<std::string>();
    }
    return val;
}

void
JSONConfigurationMap::WriteValue(const std::string &aSectionKey, const std::string &aPropertyKey, std::string aValue) {
    this->mJson[aSectionKey][aPropertyKey] = aValue;
}







