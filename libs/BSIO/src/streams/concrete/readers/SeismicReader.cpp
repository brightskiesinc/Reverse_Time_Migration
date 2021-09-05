/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/streams/concrete/readers/SeismicReader.hpp>

#include <bs/io/streams/concrete/readers/SegyReader.hpp>
#include <bs/io/streams/concrete/readers/JsonReader.hpp>

#include <bs/base/exceptions/Exceptions.hpp>

using namespace std;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using namespace bs::base::exceptions;


std::unordered_map<std::string, ReaderType> SeismicReader::mReaderMap = {
        {"segy", ReaderType::SEGY},
        {"json", ReaderType::JSON},
        {"su",   ReaderType::SU}
};

SeismicReader::SeismicReader(ReaderType aType, ConfigurationMap *apConfigurationMap) {
    switch (aType) {
        case ReaderType::SEGY:
            this->mpReader = new SegyReader(apConfigurationMap);
            break;
        case ReaderType::JSON:
            this->mpReader = new JsonReader(apConfigurationMap);
            break;
        default:
            throw UNSUPPORTED_FEATURE_EXCEPTION();
    }
}

SeismicReader::~SeismicReader() {
    delete this->mpReader;
}

ReaderType
SeismicReader::ToReaderType(const std::string &aString) {
    if (mReaderMap.find(aString) == mReaderMap.end()) {
        std::string supported = "Invalid string : supported formats are [";
        for (const auto &it : mReaderMap) {
            supported += it.first + " ";
        }
        supported += "]";
        throw runtime_error(supported);
    }
    return mReaderMap[aString];
}

std::string
SeismicReader::ToString(ReaderType aType) {
    std::string representation;
    for (const auto &it : mReaderMap) {
        if (it.second == aType) {
            representation = it.first;
            break;
        }
    }
    if (representation.empty()) {
        throw runtime_error("Unsupported reader type...");
    }
    return representation;
}

void
SeismicReader::AcquireConfiguration() {
    this->mpReader->AcquireConfiguration();
}

string SeismicReader::GetExtension() {
    return this->mpReader->GetExtension();
}

int
SeismicReader::Initialize(vector<std::string> &aGatherKeys,
                          vector<std::pair<std::string, Gather::SortDirection>> &aSortingKeys,
                          vector<std::string> &aPaths) {
    return this->mpReader->Initialize(aGatherKeys, aSortingKeys, aPaths);
}

int
SeismicReader::Initialize(vector<dataunits::TraceHeaderKey> &aGatherKeys,
                          vector<std::pair<TraceHeaderKey, Gather::SortDirection>> &aSortingKeys,
                          vector<std::string> &aPaths) {
    return this->mpReader->Initialize(aGatherKeys, aSortingKeys, aPaths);
}

int
SeismicReader::Finalize() {
    return this->mpReader->Finalize();
}

void
SeismicReader::SetHeaderOnlyMode(bool aEnableHeaderOnly) {
    this->mpReader->SetHeaderOnlyMode(aEnableHeaderOnly);
}

unsigned int
SeismicReader::GetNumberOfGathers() {
    return this->mpReader->GetNumberOfGathers();
}

vector<std::vector<std::string>>
SeismicReader::GetIdentifiers() {
    return this->mpReader->GetIdentifiers();
}

vector<Gather *>
SeismicReader::ReadAll() {
    return this->mpReader->ReadAll();
}

vector<Gather *>
SeismicReader::Read(std::vector<std::vector<std::string>> aHeaderValues) {
    return this->mpReader->Read(aHeaderValues);
}

Gather *
SeismicReader::Read(std::vector<std::string> aHeaderValues) {
    return this->mpReader->Read(aHeaderValues);
}

Gather *
SeismicReader::Read(unsigned int aIndex) {
    return this->mpReader->Read(aIndex);
}
