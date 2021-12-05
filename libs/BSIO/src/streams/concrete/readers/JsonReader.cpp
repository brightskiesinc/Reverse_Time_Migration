/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <bs/base/common/ExitCodes.hpp>
#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/streams/concrete/readers/JsonReader.hpp>
#include <bs/io/utils/convertors/KeysConvertor.hpp>
#include <bs/io/utils/synthetic-generators/concrete/ParameterMetaDataGenerator.hpp>
#include <bs/io/utils/synthetic-generators/concrete/ShotsMetaDataGenerator.hpp>
#include <bs/io/utils/synthetic-generators/concrete/PlaneReflectorGenerator.hpp>
#include <bs/io/configurations/MapKeys.h>

using namespace std;
using namespace bs::base::exceptions;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::io::generators;
using namespace bs::io::utils::convertors;

JsonReader::JsonReader(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mEnableHeaderOnly = false;
    this->mpMetaGenerator = nullptr;
}

JsonReader::~JsonReader() {
    delete this->mpMetaGenerator;
    for (auto const &ptr : this->mReflectorGenerators) {
        delete ptr;
    }
}

void
JsonReader::AcquireConfiguration() {
}

std::string
JsonReader::GetExtension() {
    return IO_K_EXT_JSON;
}

int
JsonReader::Initialize(std::vector<std::string> &aGatherKeys,
                       std::vector<std::pair<std::string, dataunits::Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) {
    for (auto &e : aGatherKeys) {
        auto gather_key = KeysConvertor::ToTraceHeaderKey(e);
        this->mGatherKeys.push_back(gather_key);
    }
    for (auto &e : aSortingKeys) {
        auto key = KeysConvertor::ToTraceHeaderKey(e.first);
        auto sort_dir = e.second;
        this->mSortingKeys.emplace_back(key, sort_dir);
    }
    this->mFilePath = aPaths[0];
    this->GenerateComponents();
    return BS_BASE_RC_SUCCESS;
}

int
JsonReader::Initialize(std::vector<TraceHeaderKey> &aGatherKeys,
                       std::vector<std::pair<TraceHeaderKey, dataunits::Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) {
    this->mGatherKeys = aGatherKeys;
    this->mSortingKeys = aSortingKeys;
    this->mFilePath = aPaths[0];
    this->GenerateComponents();
    return BS_BASE_RC_SUCCESS;
}

int
JsonReader::Finalize() {
    return BS_BASE_RC_SUCCESS;
}

void
JsonReader::SetHeaderOnlyMode(bool aEnableHeaderOnly) {
    this->mEnableHeaderOnly = aEnableHeaderOnly;
}

unsigned int
JsonReader::GetNumberOfGathers() {
    return this->mpMetaGenerator->GetGatherNumber();
}

std::vector<std::vector<std::string>>
JsonReader::GetIdentifiers() {
    std::vector<std::vector<std::string>> unique_identifiers;
    unique_identifiers = this->mpMetaGenerator->GetGatherUniqueValues();
    return unique_identifiers;
}

void
JsonReader::FillTraceData(std::vector<Gather *> &aGathers) {
    for (auto &g : aGathers) {
        auto traces = g->GetAllTraces();
        for (auto &t : traces) {
            float ix = t->GetTraceHeaderKeyValue<int>(TraceHeaderKey::SYN_X_IND);
            float iy = t->GetTraceHeaderKeyValue<int>(TraceHeaderKey::SYN_Y_IND);
            vector<pair<float, int>> reflector_pairs;
            for (int i = 0; i < this->mReflectorGenerators.size(); i++) {
                float depth = this->mReflectorGenerators[i]->GetReflectorDepth(ix, iy);
                reflector_pairs.emplace_back(depth, i);
            }
            sort(reflector_pairs.begin(), reflector_pairs.end(),
                 [](pair<float, int> a, pair<float, int> b) {
                     return a.first < b.first;
                 });
            int reflector_index = 0;
            for (int iz = 0; iz < t->GetNumberOfSamples(); iz++) {
                auto &reflector = reflector_pairs[reflector_index];
                if (iz < reflector.first) {
                    auto gen = this->mReflectorGenerators[reflector.second];
                    t->GetTraceData()[iz] = gen->GetBeforeValue();
                } else if (reflector_index == this->mReflectorGenerators.size() - 1) {
                    auto gen = this->mReflectorGenerators[reflector.second];
                    t->GetTraceData()[iz] = gen->GetAfterValue();
                } else {
                    while (reflector_index < this->mReflectorGenerators.size() - 1 &&
                           iz > reflector_pairs[reflector_index + 1].first) {
                        reflector_index++;
                    }
                    auto &reflector_before = reflector_pairs[reflector_index];
                    if (reflector_index == this->mReflectorGenerators.size() - 1) {
                        auto gen = this->mReflectorGenerators[reflector_before.second];
                        t->GetTraceData()[iz] = gen->GetAfterValue();
                    } else {
                        auto &reflector_after = reflector_pairs[reflector_index + 1];
                        auto value_before = this->mReflectorGenerators[reflector_before.second]->GetAfterValue();
                        auto value_after = this->mReflectorGenerators[reflector_after.second]->GetBeforeValue();
                        auto depth_before = reflector_before.first;
                        auto depth_after = reflector_after.first;
                        auto divisor = (depth_after - depth_before);
                        if (divisor == 0) {
                            t->GetTraceData()[iz] = value_after;
                        } else {
                            t->GetTraceData()[iz] = value_before +
                                                    ((value_after - value_before) * (iz - depth_before)) /
                                                    (depth_after - depth_before);
                        }
                    }
                }
            }
        }
    }
}

std::vector<Gather *>
JsonReader::ReadAll() {
    /* Vector to be returned. */
    std::vector<Gather *> return_gathers = this->mpMetaGenerator->GetAllTraces();
    if (!this->mEnableHeaderOnly) {
        this->FillTraceData(return_gathers);
    }
    return return_gathers;
}

std::vector<Gather *>
JsonReader::Read(std::vector<std::vector<std::string>> aHeaderValues) {
    /* Vector to be returned. */
    std::vector<Gather *> return_gathers = this->mpMetaGenerator->GetTraces(aHeaderValues);
    if (!this->mEnableHeaderOnly) {
        this->FillTraceData(return_gathers);
    }
    return return_gathers;
}

Gather *
JsonReader::Read(std::vector<std::string> aHeaderValues) {
    Gather *return_gather;
    return_gather = this->Read(vector<vector<string>>{aHeaderValues})[0];
    return return_gather;
}

Gather *
JsonReader::Read(unsigned int aIndex) {
    auto identifiers = this->GetIdentifiers();
    if (aIndex >= identifiers.size()) {
        throw ILLOGICAL_EXCEPTION();
    }
    return this->Read(identifiers[aIndex]);
}

void
JsonReader::GenerateComponents() {
    std::ifstream in(this->mFilePath);
    nlohmann::json descriptor;
    in >> descriptor;
    if (!descriptor.contains("meta-data")) {
        throw ILLOGICAL_EXCEPTION();
    }
    nlohmann::json meta_data = descriptor["meta-data"];
    std::string type = meta_data["type"];
    delete this->mpMetaGenerator;
    if (type == "parameter") {
        this->mpMetaGenerator = new ParameterMetaDataGenerator(meta_data);
    } else if (type == "traces") {
        this->mpMetaGenerator = new ShotsMetaDataGenerator(meta_data);
    } else {
        throw UNSUPPORTED_FEATURE_EXCEPTION();
    }
    this->mpMetaGenerator->SetGenerationKey(this->mGatherKeys);
    for (auto const &ptr : this->mReflectorGenerators) {
        delete ptr;
    }
    this->mReflectorGenerators.clear();
    if (descriptor.contains("data")) {
        nlohmann::json data = descriptor["data"];
        if (data.contains("reflector")) {
            nlohmann::json reflector_data = data["reflector"];
            for (auto it = reflector_data.begin(); it != reflector_data.end(); ++it) {
                auto object = it.value();
                std::string object_type = object["type"];
                if (object_type == "plane") {
                    auto gen = new PlaneReflectorGenerator();
                    gen->ParseValues(object);
                    this->mReflectorGenerators.push_back(gen);
                } else {
                    throw UNSUPPORTED_FEATURE_EXCEPTION();
                }
            }
        }
    }
}
