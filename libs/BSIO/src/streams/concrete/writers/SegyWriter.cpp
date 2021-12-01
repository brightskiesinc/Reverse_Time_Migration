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

#include <bs/base/common/ExitCodes.hpp>

#include <bs/io/streams/concrete/writers/SegyWriter.hpp>
#include <bs/io/streams/helpers/OutStreamHelper.hpp>
#include <bs/io/lookups/tables/TextHeaderLookup.hpp>
#include <bs/io/lookups/tables/BinaryHeaderLookup.hpp>
#include <bs/io/lookups/tables/TraceHeaderLookup.hpp>
#include <bs/io/lookups/mappers/HeaderMapper.hpp>
#include <bs/io/lookups/mappers/SegyHeaderMapper.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/utils/convertors/FloatingPointFormatter.hpp>
#include <bs/io/common/Definitions.hpp>
#include <bs/io/configurations/MapKeys.h>

using namespace bs::io::streams;
using namespace bs::io::streams::helpers;
using namespace bs::io::lookups;
using namespace bs::io::utils::convertors;


SegyWriter::SegyWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mOutStreamHelpers = nullptr;
    this->mWriteLittleEndian = true;
    this->mBinaryHeaderWritten = false;
    this->mFormat = 1;
}

SegyWriter::~SegyWriter() {
    delete this->mOutStreamHelpers;
}

void
SegyWriter::AcquireConfiguration() {
    this->mWriteLittleEndian = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_LITTLE_ENDIAN, this->mWriteLittleEndian);
    this->mFormat = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_FLOAT_FORMAT, this->mFormat);
}

std::string
SegyWriter::GetExtension() {
    return IO_K_EXT_SGY;
}

int
SegyWriter::Initialize(std::string &aFilePath) {
    this->mFilePath = aFilePath + this->GetExtension();
    this->mOutStreamHelpers = new OutStreamHelper(this->mFilePath);
    this->mOutStreamHelpers->Open();
    char text_header[IO_SIZE_TEXT_HEADER];
    memset(text_header, 0, IO_SIZE_TEXT_HEADER);
    std::string temp = IO_DEF_BRIGHTSKIES_COPY_WRITES;
    memcpy(text_header, temp.c_str(), temp.size());
    text_header[temp.size()] = '\0';
    this->mOutStreamHelpers->WriteBytesBlock(text_header, IO_SIZE_TEXT_HEADER);
    this->mBinaryHeaderWritten = false;
    return BS_BASE_RC_SUCCESS;
}

int
SegyWriter::Finalize() {
    this->mOutStreamHelpers->Close();
    return BS_BASE_RC_SUCCESS;
}

int
SegyWriter::Write(std::vector<dataunits::Gather *> aGathers) {
    int rc = 0;
    for (const auto &it : aGathers) {
        rc += this->Write(it);
    }
    /* Check that all Write() functions returned BS_BASE_RC_SUCCESS signal. */
    return aGathers.empty() ? BS_BASE_RC_SUCCESS : (rc / aGathers.size()) == BS_BASE_RC_SUCCESS;
}

int
SegyWriter::Write(io::dataunits::Gather *aGather) {
    if (!this->mBinaryHeaderWritten) {
        BinaryHeaderLookup binary_header{};
        memset(&binary_header, 0, sizeof(binary_header));
        uint16_t format = this->mFormat;
        binary_header.FORMAT = NumbersConvertor::ToLittleEndian(format);
        uint16_t hdt = aGather->GetSamplingRate();
        binary_header.HDT = NumbersConvertor::ToLittleEndian(hdt);
        uint16_t hns = 0;
        if (aGather->GetNumberTraces() > 0) {
            hns = aGather->GetTrace(0)->GetNumberOfSamples();
        }
        binary_header.HNS = NumbersConvertor::ToLittleEndian(hns);
        this->mOutStreamHelpers->WriteBytesBlock((char *) &binary_header, IO_SIZE_BINARY_HEADER);
        this->mBinaryHeaderWritten = true;
    }
    int rc;
    for (auto &g : aGather->GetAllTraces()) {
        rc = 0;
        TraceHeaderLookup trace_header{};
        memset(&trace_header, 0, sizeof(trace_header));
        HeaderMapper::MapTraceToHeader((char *) &trace_header, *g,
                                       SegyHeaderMapper::mLocationTable);
        uint16_t ns = g->GetNumberOfSamples();
        /* Write header of trace. */
        rc += this->mOutStreamHelpers->WriteBytesBlock((char *) &trace_header,
                                                       IO_SIZE_TRACE_HEADER);
        /* Format and write data of trace. */
        auto trace_formatted_size = FloatingPointFormatter::GetFloatArrayRealSize(ns, this->mFormat);
        auto trace_data = new char[trace_formatted_size];
        FloatingPointFormatter::Format((char *) g->GetTraceData(), trace_data,
                                       ns * sizeof(float),
                                       ns,
                                       this->mFormat, false);
        this->mOutStreamHelpers->WriteBytesBlock(trace_data, trace_formatted_size);
        delete[] trace_data;
    }
    return rc;
}
