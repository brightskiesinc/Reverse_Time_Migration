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

#include <iostream>

#include <bs/base/common/ExitCodes.hpp>

#include <bs/io/streams/helpers/InStreamHelper.hpp>
#include <bs/base/exceptions/Exceptions.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/utils/convertors/FloatingPointFormatter.hpp>
#include <bs/io/lookups/mappers/HeaderMapper.hpp>
#include <bs/io/lookups/mappers/SegyHeaderMapper.hpp>
#include <bs/io/data-units/helpers/TraceHelper.hpp>

using namespace bs::io::streams::helpers;
using namespace bs::io::dataunits;
using namespace bs::io::dataunits::helpers;
using namespace bs::io::lookups;
using namespace bs::base::exceptions;
using namespace bs::io::utils::convertors;


InStreamHelper::InStreamHelper(std::string &aFilePath)
        : mFilePath(aFilePath), mFileSize(-1) {}

InStreamHelper::~InStreamHelper() = default;

size_t
InStreamHelper::Open() {
    this->mInStream.open(this->mFilePath.c_str(), std::ifstream::in);
    if (this->mInStream.fail()) {
        throw bs::base::exceptions::FILE_NOT_FOUND_EXCEPTION();
    }
    return this->GetFileSize();
}

int
InStreamHelper::Close() {
    this->mInStream.close();
    return BS_BASE_RC_SUCCESS;
}

unsigned char *
InStreamHelper::ReadBytesBlock(size_t aStartPosition, size_t aBlockSize) {
    if (aStartPosition + aBlockSize > this->GetFileSize()) {
        throw INDEX_OUT_OF_BOUNDS_EXCEPTION();
    }
    auto buffer = new unsigned char[aBlockSize];
    memset(buffer, '\0', sizeof(unsigned char) * aBlockSize);
    this->mInStream.seekg(aStartPosition, std::fstream::beg);
    this->mInStream.read((char *) buffer, aBlockSize);

    return buffer;
}

unsigned char *
InStreamHelper::ReadTextHeader(size_t aStartPosition) {
    if (aStartPosition + IO_SIZE_TEXT_HEADER > this->GetFileSize()) {
        throw INDEX_OUT_OF_BOUNDS_EXCEPTION();
    }
    return this->ReadBytesBlock(aStartPosition, IO_SIZE_TEXT_HEADER);
}

BinaryHeaderLookup
InStreamHelper::ReadBinaryHeader(size_t aStartPosition) {
    if (aStartPosition + IO_SIZE_BINARY_HEADER > this->GetFileSize()) {
        throw INDEX_OUT_OF_BOUNDS_EXCEPTION();
    }
    auto bhl_buffer = this->ReadBytesBlock(aStartPosition, IO_SIZE_BINARY_HEADER);
    BinaryHeaderLookup bhl{};
    std::memcpy(&bhl, bhl_buffer, sizeof(BinaryHeaderLookup));
    delete bhl_buffer;
    return bhl;
}

TraceHeaderLookup
InStreamHelper::ReadTraceHeader(size_t aStartPosition) {
    if (aStartPosition + IO_SIZE_TRACE_HEADER > this->GetFileSize()) {
        throw INDEX_OUT_OF_BOUNDS_EXCEPTION();
    }
    auto thl_buffer = this->ReadBytesBlock(aStartPosition, IO_SIZE_TRACE_HEADER);
    TraceHeaderLookup thl{};
    std::memcpy(&thl, thl_buffer, sizeof(TraceHeaderLookup));
    delete thl_buffer;
    return thl;
}

Trace *
InStreamHelper::ReadFormattedTraceData(size_t aStartPosition,
                                       TraceHeaderLookup &aTraceHeaderLookup,
                                       BinaryHeaderLookup &aBinaryHeaderLookup) {
    auto trace_size = InStreamHelper::GetTraceDataSize(aTraceHeaderLookup, aBinaryHeaderLookup);

    if (aStartPosition + trace_size > this->GetFileSize()) {
        throw INDEX_OUT_OF_BOUNDS_EXCEPTION();
    }

    auto trace_data = new char[trace_size];
    this->mInStream.seekg(aStartPosition, std::fstream::beg);
    this->mInStream.read(trace_data, trace_size);
    size_t sample_number = InStreamHelper::GetSamplesNumber(aTraceHeaderLookup, aBinaryHeaderLookup);
    auto trace_data_formatted = new char[sample_number * sizeof(float)];
    FloatingPointFormatter::Format(trace_data, trace_data_formatted,
                                   trace_size,
                                   sample_number,
                                   NumbersConvertor::ToLittleEndian(aBinaryHeaderLookup.FORMAT),
                                   true);

    auto trace = new Trace(NumbersConvertor::ToLittleEndian(aTraceHeaderLookup.NS));
    trace->SetTraceData((float *) trace_data_formatted);
    delete[] trace_data;
    /* Weight trace data values according to the target formats. */
    TraceHelper::Weight(trace, aTraceHeaderLookup, aBinaryHeaderLookup);
    /* Set trace headers */
    HeaderMapper::MapHeaderToTrace((const char *) &aTraceHeaderLookup, *trace,
                                   SegyHeaderMapper::mLocationTable);
    // Set the number of samples to the appropriate one from either binary header or trace
    // header. Just in case it is not in trace header, since then the mapper won't set
    // it correctly.
    trace->SetTraceHeaderKeyValue(TraceHeaderKey::NS, sample_number);
    return trace;
}

size_t
InStreamHelper::GetFileSize() {
    if (this->mFileSize == -1) {
        size_t curr_offset = this->mInStream.tellg();
        this->mInStream.seekg(0, std::fstream::end);
        this->mFileSize = this->mInStream.tellg();
        this->mInStream.seekg(curr_offset, std::fstream::beg);

    }
    return this->mFileSize;
}

size_t
InStreamHelper::GetCurrentPosition() {
    return this->mInStream.tellg();
}

size_t
InStreamHelper::GetTraceDataSize(const TraceHeaderLookup &aTraceHeaderLookup,
                                 const BinaryHeaderLookup &aBinaryHeaderLookup) {
    auto format = NumbersConvertor::ToLittleEndian(aBinaryHeaderLookup.FORMAT);
    auto samples_number = InStreamHelper::GetSamplesNumber(aTraceHeaderLookup, aBinaryHeaderLookup);
    return FloatingPointFormatter::GetFloatArrayRealSize(samples_number, format);
}

size_t
InStreamHelper::GetSamplesNumber(const TraceHeaderLookup &aTraceHeaderLookup,
                                 const BinaryHeaderLookup &aBinaryHeaderLookup) {
    auto ns = NumbersConvertor::ToLittleEndian(aTraceHeaderLookup.NS);
    auto hns = NumbersConvertor::ToLittleEndian(aBinaryHeaderLookup.HNS);

    auto samples_number = ns;
    if (samples_number == 0) {
        samples_number = hns;
    }
    return samples_number;
}
