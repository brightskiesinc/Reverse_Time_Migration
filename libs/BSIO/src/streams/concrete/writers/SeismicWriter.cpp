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

#include <bs/io/streams/concrete/writers/SeismicWriter.hpp>

#include <bs/io/streams/concrete/writers/CSVWriter.hpp>
#include <bs/io/streams/concrete/writers/SUWriter.hpp>
#include <bs/io/streams/concrete/writers/BinaryWriter.hpp>
#include <bs/io/streams/concrete/writers/ImageWriter.hpp>
#include <bs/io/streams/concrete/writers/SegyWriter.hpp>

#include <bs/base/exceptions/Exceptions.hpp>

using namespace std;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::configurations;
using namespace bs::base::exceptions;

std::unordered_map<std::string, WriterType> SeismicWriter::mWriterMap = {
        {"segy",   WriterType::SEGY},
        {"binary", WriterType::BINARY},
        {"csv",    WriterType::CSV},
        {"su",     WriterType::SU},
        {"image",  WriterType::IMAGE}
};

SeismicWriter::SeismicWriter(WriterType aType, ConfigurationMap *apConfigurationMap) {
    switch (aType) {
        case WriterType::SEGY:
            this->mpWriter = new SegyWriter(apConfigurationMap);
            break;
        case WriterType::BINARY:
            this->mpWriter = new BinaryWriter(apConfigurationMap);
            break;
        case WriterType::CSV:
            this->mpWriter = new CSVWriter(apConfigurationMap);
            break;
        case WriterType::SU:
            this->mpWriter = new SUWriter(apConfigurationMap);
            break;
        case WriterType::IMAGE:
            this->mpWriter = new ImageWriter(apConfigurationMap);
            break;
        default:
            throw UNSUPPORTED_FEATURE_EXCEPTION();
    }
}

WriterType SeismicWriter::ToWriterType(const std::string &aString) {
    if (mWriterMap.find(aString) == mWriterMap.end()) {
        std::string supported = "Invalid string : supported formats are [";
        for (const auto &it : mWriterMap) {
            supported += it.first + " ";
        }
        supported += "]";
        throw runtime_error(supported);
    }
    return mWriterMap[aString];
}

std::string SeismicWriter::ToString(WriterType aType) {
    std::string representation;
    for (const auto &it : mWriterMap) {
        if (it.second == aType) {
            representation = it.first;
            break;
        }
    }
    if (representation.empty()) {
        throw runtime_error("Unsupported writer type...");
    }
    return representation;
}

SeismicWriter::~SeismicWriter() {
    delete this->mpWriter;
}

void SeismicWriter::AcquireConfiguration() {
    this->mpWriter->AcquireConfiguration();
}

string SeismicWriter::GetExtension() {
    return this->mpWriter->GetExtension();
}

int SeismicWriter::Initialize(string &aFilePath) {
    return this->mpWriter->Initialize(aFilePath);
}

int SeismicWriter::Finalize() {
    return this->mpWriter->Finalize();
}

int SeismicWriter::Write(vector<dataunits::Gather *> aGathers) {
    return this->mpWriter->Write(aGathers);
}

int SeismicWriter::Write(io::dataunits::Gather *aGather) {
    return this->mpWriter->Write(aGather);
}
