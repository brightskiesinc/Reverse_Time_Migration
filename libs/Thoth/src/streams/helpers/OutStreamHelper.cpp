//
// Created by zeyad-osama on 11/03/2021.
//

#include <thoth/streams/helpers/OutStreamHelper.hpp>

#include <thoth/exceptions/Exceptions.hpp>
#include <thoth/common/ExitCodes.hpp>

#include <iostream>

using namespace thoth::streams::helpers;
using namespace thoth::exceptions;
using namespace thoth::common::exitcodes;


OutStreamHelper::OutStreamHelper(std::string &aFilePath)
        : mFilePath(aFilePath), mFileSize(-1) {}

OutStreamHelper::~OutStreamHelper() = default;

size_t OutStreamHelper::Open() {
    this->mOutStream.open(this->mFilePath.c_str(), std::ofstream::out);
    if (this->mOutStream.fail()) {
        std::cerr << "Error opening file  " << this->mFilePath << std::endl;
        exit(EXIT_FAILURE);
    }
    return this->GetFileSize();
}

int OutStreamHelper::Close() {
    this->mOutStream.close();
    return IO_RC_SUCCESS;
}

int OutStreamHelper::WriteBytesBlock(unsigned char *aData, size_t aBlockSize) {
    this->mOutStream.write((char *) aData, sizeof(unsigned char) * aBlockSize);
    this->mFileSize += aBlockSize;
    return this->GetFileSize();
}
