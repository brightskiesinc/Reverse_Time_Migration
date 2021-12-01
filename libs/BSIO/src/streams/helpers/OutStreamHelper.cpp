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

#include <bs/io/streams/helpers/OutStreamHelper.hpp>

using namespace bs::io::streams::helpers;


OutStreamHelper::OutStreamHelper(std::string &aFilePath, bool aModify)
        : mFilePath(aFilePath), mFileSize(0), mModify(aModify) {}

OutStreamHelper::~OutStreamHelper() = default;

size_t
OutStreamHelper::Open() {
    if (this->mModify) {
        this->mOutStream.open(this->mFilePath.c_str(), std::ofstream::out | std::ofstream::in);
        this->mFileSize = -1;
    } else {
        this->mOutStream.open(this->mFilePath.c_str(), std::ofstream::out);
    }
    if (this->mOutStream.fail()) {
        std::cerr << "Error opening file  " << this->mFilePath << std::endl;
        exit(EXIT_FAILURE);
    }
    return this->GetFileSize();
}

int
OutStreamHelper::Close() {
    this->mOutStream.close();
    return BS_BASE_RC_SUCCESS;
}

size_t
OutStreamHelper::WriteBytesBlock(const char *aData, size_t aBlockSize) {
    this->mOutStream.seekp(this->mFileSize, std::fstream::beg);
    this->mOutStream.write((char *) aData, sizeof(unsigned char) * aBlockSize);
    this->mFileSize += aBlockSize;
    return this->GetFileSize();
}

size_t
OutStreamHelper::WriteBytesBlock(const char *aData, size_t aBlockSize,
                                 size_t aStartingPosition) {
    this->mOutStream.seekp(aStartingPosition, std::fstream::beg);
    this->mOutStream.write((char *) aData, sizeof(unsigned char) * aBlockSize);
    size_t final_file_size = aStartingPosition + aBlockSize;
    if (final_file_size > this->mFileSize) {
        this->mFileSize = final_file_size;
    }
    return this->GetFileSize();
}

size_t
OutStreamHelper::GetFileSize() {
    if (this->mFileSize == -1) {
        size_t curr_offset = this->mOutStream.tellp();
        this->mOutStream.seekp(0, std::fstream::end);
        this->mFileSize = this->mOutStream.tellp();
        this->mOutStream.seekp(curr_offset, std::fstream::beg);
    }
    return this->mFileSize;
}
