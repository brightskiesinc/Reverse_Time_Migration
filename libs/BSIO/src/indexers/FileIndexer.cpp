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

#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/indexers/FileIndexer.hpp>
#include <bs/io/lookups/mappers/SegyHeaderMapper.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/configurations/MapKeys.h>

using namespace bs::base::exceptions;
using namespace bs::io::indexers;
using namespace bs::io::dataunits;
using namespace bs::io::lookups;
using namespace bs::io::streams::helpers;
using namespace bs::io::utils::convertors;

FileIndexer::FileIndexer(std::string &aFilePath,
                         std::string &aKey)
        : mInFilePath(aFilePath),
          mOutFilePath(GenerateOutFilePathName(aFilePath, aKey)) {
    this->mInStreamHelper = nullptr;
}

FileIndexer::~FileIndexer() {
    delete this->mInStreamHelper;
}

size_t
FileIndexer::Initialize() {
    this->mInStreamHelper = new InStreamHelper(this->mInFilePath);
    return (this->mInStreamHelper->Open());
}

int
FileIndexer::Finalize() {
    return (this->mInStreamHelper->Close());
}

std::string
FileIndexer::GenerateOutFilePathName(std::string &aInFileName,
                                     std::string &key) {
    return aInFileName.substr(0,
                              aInFileName.size() - std::string(IO_K_EXT_SGY).length())
           + key
           + IO_K_EXT_SGY_INDEX;
}

IndexMap
FileIndexer::Index(const std::vector<TraceHeaderKey> &aTraceHeaderKeys) {

    std::string key = TraceHeaderKey::GatherKeysToString(aTraceHeaderKeys);
    std::string expected_file_path = GenerateOutFilePathName(this->mInFilePath,
                                                             key);
    auto in = new InStreamHelper(this->mOutFilePath);
    bool already_indexed = true;
    try {
        in->Open();
    } catch (FileNotFoundException &e) {
        already_indexed = false;
    }


    if (!already_indexed) {
        /* Read binary header in the given file.*/
        auto bhl = this->mInStreamHelper->ReadBinaryHeader(IO_POS_S_BINARY_HEADER);
        unsigned long long file_size = this->mInStreamHelper->GetFileSize();
        size_t start_pos = IO_POS_S_TRACE_HEADER;
        while (true) {
            if (start_pos + IO_SIZE_TRACE_HEADER >= file_size) {
                break;
            }
            /* Read trace header in the given file. */
            auto thl = this->mInStreamHelper->ReadTraceHeader(start_pos);

            std::unordered_map<dataunits::TraceHeaderKey::Key,
                    std::pair<size_t, NATIVE_TYPE>> location_table;
            for (const auto &thk :aTraceHeaderKeys) {
                TraceHeaderKey trh = thk;
                dataunits::TraceHeaderKey::Key k = trh.GetKey();
                std::pair<size_t, NATIVE_TYPE> offset_type = SegyHeaderMapper::mLocationTable[k];
                location_table[k] = offset_type;
            }
            Trace trace(0);
            HeaderMapper::MapHeaderToTrace(
                    (const char *) &thl, trace, location_table, true);
            auto &map = *trace.GetTraceHeaders();
            std::vector<std::string> values;
            values.reserve(aTraceHeaderKeys.size());
            for (const auto &it : aTraceHeaderKeys) {
                values.push_back(map.find(it)->second);
            }
            std::string value = TraceHeaderKey::GatherValuesToString(values);
            this->mIndexMap.Add(key, value, start_pos);

            /* Update stream position pointer. */
            start_pos += IO_SIZE_TRACE_HEADER + InStreamHelper::GetTraceDataSize(thl, bhl);
        }
        /** store index map */
        StoreMapToFile(aTraceHeaderKeys);
    } else {
        /** load index map */
        LoadMapFromFile(in, aTraceHeaderKeys);
        in->Close();
    }
    delete in;
    return this->mIndexMap;
}

void
FileIndexer::StoreMapToFile(const std::vector<TraceHeaderKey> &aTraceHeaderKeys) {

    std::string key = TraceHeaderKey::GatherKeysToString(aTraceHeaderKeys);
    auto out_helper = new OutStreamHelper(this->mOutFilePath);
    out_helper->Open();
    /** store length of IndexMap Key */
    size_t key_size = key.size();
    out_helper->WriteBytesBlock((const char *) &key_size, sizeof(size_t));
    /** store index map key */
    out_helper->WriteBytesBlock(key.c_str(), key_size);

    /** Get map of unique values and vector of byte positions */
    std::map<std::string, std::vector<size_t>> unique_values_map = this->mIndexMap.Get(key);
    /** store map size */
    size_t map_size = unique_values_map.size();
    out_helper->WriteBytesBlock((const char *) &map_size, sizeof(size_t));

    for (const auto &it : unique_values_map) {
        /** store length of each unique key string */
        size_t unique_key_size = it.first.size();
        out_helper->WriteBytesBlock((const char *) &unique_key_size, sizeof(size_t));
        /** store unique key */
        out_helper->WriteBytesBlock(it.first.c_str(), unique_key_size);
        /** store sizeof vector of byte positions */
        size_t bytes_vector_size = it.second.size();
        out_helper->WriteBytesBlock((const char *) &bytes_vector_size, sizeof(size_t));
        /** loop on the vector and store each byte position */
        for (const auto &byte : it.second) {
            out_helper->WriteBytesBlock((const char *) &byte, sizeof(size_t));
        }
    }
    out_helper->Close();
    delete out_helper;
}

void
FileIndexer::LoadMapFromFile(streams::helpers::InStreamHelper *aInStreamHelper,
                             const std::vector<dataunits::TraceHeaderKey> &aTraceHeaderKeys) {
    size_t position = 0;

    /** load index map key length */
    size_t key_size;
    auto buffer = aInStreamHelper->ReadBytesBlock(position, sizeof(size_t));
    std::memcpy(&key_size, buffer, sizeof(size_t));
    position += sizeof(size_t);
    delete[] buffer;

    /** load index map key */
    auto idx_map_key = aInStreamHelper->ReadBytesBlock(position, key_size);
    std::string key(&idx_map_key[0], idx_map_key + key_size);
    position += key_size;
    delete[] idx_map_key;

    /** load map of unique values and byte positions' vector size */
    size_t map_size;
    buffer = aInStreamHelper->ReadBytesBlock(position, sizeof(size_t));
    std::memcpy(&map_size, buffer, sizeof(size_t));
    position += sizeof(size_t);
    delete[] buffer;

    for (int i = 0; i < map_size; i++) {
        /** Load length of each unique key string */
        size_t value_size;
        buffer = aInStreamHelper->ReadBytesBlock(position, sizeof(size_t));
        std::memcpy(&value_size, buffer, sizeof(size_t));
        position += sizeof(size_t);
        delete[] buffer;

        /** Load unique key */
        auto temp = aInStreamHelper->ReadBytesBlock(position, value_size);
        std::string value(&temp[0], temp + value_size);
        position += value_size;
        delete[] temp;

        /** Load sizeof vector of byte positions */
        size_t bytes_vector_size;
        buffer = aInStreamHelper->ReadBytesBlock(position, sizeof(size_t));
        std::memcpy(&bytes_vector_size, buffer, sizeof(size_t));
        position += sizeof(size_t);
        delete[] buffer;

        /** loop on the vector and load each byte position */
        std::vector<size_t> byte_positions;
        byte_positions.reserve(bytes_vector_size);
        for (int j = 0; j < bytes_vector_size; j++) {
            size_t byte_position;
            buffer = aInStreamHelper->ReadBytesBlock(position, sizeof(size_t));
            std::memcpy(&byte_position, buffer, sizeof(size_t));
            byte_positions.push_back(byte_position);
            position += sizeof(size_t);
            delete[] buffer;
        }
        /** Add unique value and the vector of byte positions to the Index map */
        this->mIndexMap.Add(key, value, byte_positions);
    }
}
