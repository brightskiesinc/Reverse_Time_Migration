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
#include <sys/stat.h>

#include <bs/base/common/ExitCodes.hpp>

#include <bs/io/streams/concrete/writers/BinaryWriter.hpp>
#include <bs/io/configurations/MapKeys.h>

using namespace bs::io::streams;


BinaryWriter::BinaryWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

BinaryWriter::~BinaryWriter() = default;

void
BinaryWriter::AcquireConfiguration() {
    //TODO add option to dump headers in a separate file(Either in text format, or in SU format).
    this->mFilePath = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_PATH, this->mFilePath);
}

int
BinaryWriter::Initialize(std::string &aFilePath) {
    mkdir(this->mFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    this->mOutputStream = std::ofstream(this->mFilePath + aFilePath + this->GetExtension(),
                                        std::ios::out | std::ios::binary);
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
    }

    return BS_BASE_RC_SUCCESS;
}

int
BinaryWriter::Finalize() {
    this->mOutputStream.close();
    return 0;
}

std::string
BinaryWriter::GetExtension() {
    return IO_K_EXT_BIN;
}

int
BinaryWriter::Write(io::dataunits::Gather *aGather) {
    for (int i = 0; i < aGather->GetNumberTraces(); ++i) {
        auto trace = aGather->GetTrace(i);
        auto n_s = trace->GetNumberOfSamples();
        auto trace_data = trace->GetTraceData();
        this->mOutputStream.write((char *) trace_data, n_s * sizeof(float));
    }
    if (!this->mOutputStream.good()) {
        std::cout << "Error occurred at writing time!" << std::endl;
        return 1;
    }
    return 0;
}

int
BinaryWriter::Write(std::vector<dataunits::Gather *> aGathers) {
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }
    for (auto &e : aGathers) {
        this->Write(e);
    }
    return 0;
}
