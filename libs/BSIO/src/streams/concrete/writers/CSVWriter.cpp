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

#include <bs/io/streams/concrete/writers/CSVWriter.hpp>

#include <bs/base/common/ExitCodes.hpp>
#include <bs/io/configurations/MapKeys.h>

#include <iostream>
#include <sys/stat.h>

using namespace bs::io::streams;


CSVWriter::CSVWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

CSVWriter::~CSVWriter() = default;

void
CSVWriter::AcquireConfiguration() {
    this->mFilePath = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_PATH, this->mFilePath);
}

int
CSVWriter::Initialize(std::string &aFilePath) {
    mkdir(this->mFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    this->mOutputStream = std::ofstream(this->mFilePath + aFilePath + this->GetExtension(),
                                        std::ios::out | std::ios::binary);
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
    }

    return BS_BASE_RC_SUCCESS;
}

int CSVWriter::Finalize() {
    this->mOutputStream.close();
    return 0;
}

std::string CSVWriter::GetExtension() {
    return IO_K_EXT_CSV;
}


int CSVWriter::Write(io::dataunits::Gather *aGather) {
    auto n_x = aGather->GetNumberTraces();
    auto n_z = aGather->GetTrace(0)->GetNumberOfSamples();
    this->mOutputStream << n_x << "," << n_z << "\n";
    for (int ix = 0; ix < n_x; ++ix) {
        auto trace = aGather->GetTrace(ix);
        auto trace_data = trace->GetTraceData();
        for (int iz = 0; iz < n_z; ++iz) {
            auto val = trace_data[iz];
            this->mOutputStream << val << ',';
        }
        this->mOutputStream << '\n';
    }
    if (!this->mOutputStream.good()) {
        std::cout << "Error occurred at writing time!" << std::endl;
        return 1;
    }
    return 0;
}

int CSVWriter::Write(std::vector<dataunits::Gather *> aGathers) {
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }
    for (auto &e : aGathers) {
        this->Write(e);
    }
    return 0;
}
