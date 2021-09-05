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

#include <bs/io/streams/concrete/writers/SUWriter.hpp>

#include <bs/base/common/ExitCodes.hpp>
#include <bs/io/configurations/MapKeys.h>
#include <bs/io/lookups/tables/TraceHeaderLookup.hpp>
#include <bs/io/lookups/mappers/HeaderMapper.hpp>
#include <bs/io/lookups/mappers/SegyHeaderMapper.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/utils/convertors/KeysConvertor.hpp>
#include <bs/io/utils/checkers/Checker.hpp>

#include <iostream>
#include <sys/stat.h>
#include <cstring>

using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::io::lookups;
using namespace bs::io::utils::convertors;
using namespace bs::io::utils::checkers;

SUWriter::SUWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mWriteLittleEndian = false;
}

SUWriter::~SUWriter() = default;

void
SUWriter::AcquireConfiguration() {
    this->mWriteLittleEndian = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_LITTLE_ENDIAN, this->mWriteLittleEndian);
    this->mFilePath = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_PATH, this->mFilePath);
}

int
SUWriter::Initialize(std::string &aFilePath) {
    mkdir(this->mFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    this->mOutputStream = std::ofstream(this->mFilePath + aFilePath + this->GetExtension(),
                                        std::ios::out | std::ios::binary);
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
    }

    return BS_BASE_RC_SUCCESS;
}

int
SUWriter::Finalize() {
    this->mOutputStream.close();
    return 0;
}

std::string
SUWriter::GetExtension() {
    return IO_K_EXT_SU;
}

int
SUWriter::Write(io::dataunits::Gather *aGather) {
    bool swap_bytes = true;
    if ((this->mWriteLittleEndian && Checker::IsLittleEndianMachine()) ||
        (!this->mWriteLittleEndian && !Checker::IsLittleEndianMachine())) {
        swap_bytes = false;
    }
    for (int i = 0; i < aGather->GetNumberTraces(); ++i) {
        auto trace = aGather->GetTrace(i);
        auto trace_data = trace->GetTraceData();
        TraceHeaderLookup trace_header{};
        memset(&trace_header, 0, sizeof(trace_header));
        HeaderMapper::MapTraceToHeader((char *) &trace_header, *trace,
                                       SegyHeaderMapper::mLocationTable,
                                       swap_bytes);
        uint16_t ns = trace->GetNumberOfSamples();
        this->mOutputStream.write((char *) &trace_header, IO_SIZE_TRACE_HEADER);
        float *processed_data = trace_data;
        if (swap_bytes) {
            processed_data = new float[ns];
            memcpy(processed_data, trace_data, ns * sizeof(float));
            processed_data = NumbersConvertor::ToLittleEndian(processed_data, ns);
        }
        this->mOutputStream.write((char *) processed_data, ns * sizeof(float));
        if (swap_bytes) {
            delete[] processed_data;
        }
    }
    if (!this->mOutputStream.good()) {
        std::cout << "Error occurred at writing time!" << std::endl;
        return 1;
    }
    return 0;
}

int
SUWriter::Write(std::vector<dataunits::Gather *> aGathers) {
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }
    for (auto &e : aGathers) {
        this->Write(e);
    }
    return 0;
}
