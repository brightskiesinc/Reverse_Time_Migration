//
// Created by zeyad-osama on 02/11/2020.
//

#include <thoth/streams/concrete/writers/CSVWriter.hpp>

#include <thoth/common/ExitCodes.hpp>
#include <thoth/configurations/interface/MapKeys.h>

#include <iostream>
#include <sys/stat.h>

using namespace thoth::streams;
using namespace thoth::common::exitcodes;


CSVWriter::CSVWriter(thoth::configuration::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

void CSVWriter::AcquireConfiguration() {
    this->mFilePath = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_WRITE_PATH, this->mFilePath);
}

int CSVWriter::Initialize(std::string &aFilePath) {
    mkdir(this->mFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    this->mOutputStream = std::ofstream(this->mFilePath + aFilePath + this->GetExtension(),
                                        std::ios::out | std::ios::binary);
    if (!this->mOutputStream) {
        std::cout << "Cannot open file!" << std::endl;
    }

    return IO_RC_SUCCESS;
}

int CSVWriter::Write(thoth::dataunits::Gather *aGather) {
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

int CSVWriter::Finalize() {
    this->mOutputStream.close();
    return 0;
}

std::string CSVWriter::GetExtension() {
    return IO_K_EXT_CSV;
}
