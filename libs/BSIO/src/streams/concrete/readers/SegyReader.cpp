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
#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/streams/concrete/readers/SegyReader.hpp>
#include <bs/io/streams/helpers/InStreamHelper.hpp>
#include <bs/io/data-units/helpers/TraceHelper.hpp>
#include <bs/io/lookups/tables/TextHeaderLookup.hpp>
#include <bs/io/lookups/tables/BinaryHeaderLookup.hpp>
#include <bs/io/lookups/tables/TraceHeaderLookup.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>
#include <bs/io/utils/convertors/StringsConvertor.hpp>
#include <bs/io/utils/convertors/FloatingPointFormatter.hpp>
#include <bs/io/configurations/MapKeys.h>

#define IO_K_FIRST_OCCURRENCE   0   /* First occurrence position */

using namespace bs::base::exceptions;
using namespace bs::io::streams;
using namespace bs::io::streams::helpers;
using namespace bs::io::lookups;
using namespace bs::io::indexers;
using namespace bs::io::dataunits;
using namespace bs::io::dataunits::helpers;
using namespace bs::io::utils::convertors;


SegyReader::SegyReader(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mEnableHeaderOnly = false;
    this->mHasExtendedTextHeader = false;
    this->mStoreTextHeaders = false;
    this->mTextHeader = nullptr;
    this->mExtendedHeader = nullptr;
}

SegyReader::~SegyReader() {
    delete this->mTextHeader;
    if (this->mHasExtendedTextHeader) {
        delete this->mExtendedHeader;
    }
}

void
SegyReader::AcquireConfiguration() {
    this->mEnableHeaderOnly = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_TEXT_HEADERS_ONLY, this->mEnableHeaderOnly);
    this->mStoreTextHeaders = this->mpConfigurationMap->GetValue(
            IO_K_PROPERTIES, IO_K_TEXT_HEADERS_STORE, this->mStoreTextHeaders);
}

std::string
SegyReader::GetExtension() {
    return IO_K_EXT_SGY;
}

int
SegyReader::Initialize(std::vector<std::string> &aGatherKeys,
                       std::vector<std::pair<std::string, Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
}

int
SegyReader::Initialize(std::vector<TraceHeaderKey> &aGatherKeys,
                       std::vector<std::pair<TraceHeaderKey, dataunits::Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) {
    /* Reset All Variables to start from scratch */
    this->mInStreamHelpers.clear();
    this->mFileIndexers.clear();
    this->mIndexMaps.clear();
    this->mEnableHeaderOnly = false;
    this->mHasExtendedTextHeader = false;
    this->mStoreTextHeaders = false;
    this->mTextHeader = nullptr;
    this->mExtendedHeader = nullptr;

    /* Internal variables initializations. */
    this->mGatherKeys = aGatherKeys;
    this->mSortingKeys = aSortingKeys;
    this->mPaths = aPaths;

    /* Initialize streams. */
    for (auto &it : this->mPaths) {
        this->mInStreamHelpers.push_back(new InStreamHelper(it));
    }
    /* Open streams. */
    for (auto &it : this->mInStreamHelpers) {
        it->Open();
    }

    /* Read text header in the given file. */
    if (this->mStoreTextHeaders) {
        this->mTextHeader = this->mInStreamHelpers[IO_K_FIRST_OCCURRENCE]->ReadTextHeader(IO_POS_S_TEXT_HEADER);
    }

    /* Read binary header in the given file and check whether an
     * extended text header is available or not for later reading */
    this->mBinaryHeaderLookup = this->mInStreamHelpers[IO_K_FIRST_OCCURRENCE]->ReadBinaryHeader(IO_POS_S_BINARY_HEADER);
    this->mHasExtendedTextHeader = NumbersConvertor::ToLittleEndian(this->mBinaryHeaderLookup.EXT_HEAD);

    /* Read extended text header in the given file if found. */
    if (this->mHasExtendedTextHeader && this->mStoreTextHeaders) {
        this->mExtendedHeader = this->mInStreamHelpers[IO_K_FIRST_OCCURRENCE]->ReadTextHeader(IO_POS_S_EXT_TEXT_HEADER);
    }
    /* Index passed files. */
    this->Index();

    return BS_BASE_RC_SUCCESS;
}

int
SegyReader::Finalize() {
    int rc = 0;
    /* Close streams. */
    for (auto &it : this->mInStreamHelpers) {
        rc += it->Close();
    }
    /* Check that all Write() functions returned BS_BASE_RC_SUCCESS signal. */
    return (rc / this->mInStreamHelpers.size()) == BS_BASE_RC_SUCCESS;
}

void
SegyReader::SetHeaderOnlyMode(bool aEnableHeaderOnly) {
    this->mEnableHeaderOnly = aEnableHeaderOnly;
}

std::vector<Gather *>
SegyReader::ReadAll() {
    std::vector<Gather *> gathers;

    std::unordered_map<std::string, std::vector<dataunits::Trace *>> gather_map;
    auto format = NumbersConvertor::ToLittleEndian(this->mBinaryHeaderLookup.FORMAT);

    for (const auto &it : this->mInStreamHelpers) {
        unsigned long long file_size = it->GetFileSize();
        size_t start_pos = IO_POS_S_TRACE_HEADER;
        while (true) {
            if (start_pos + IO_SIZE_TRACE_HEADER >= file_size) {
                break;
            }
            /* Read trace header in the given file. */
            auto thl = it->ReadTraceHeader(start_pos);

            /* Read trace data in the given file. */
            auto trace = it->ReadFormattedTraceData(start_pos + IO_SIZE_TRACE_HEADER, thl, this->mBinaryHeaderLookup);

            auto &map = *trace->GetTraceHeaders();
            std::vector<std::string> values;
            values.reserve(this->mGatherKeys.size());
            for (const auto &i : this->mGatherKeys) {
                values.push_back(map.find(i)->second);
            }
            std::string value = TraceHeaderKey::GatherValuesToString(values);

            gather_map[value].push_back(trace);

            /* Update stream position pointer. */
            start_pos += IO_SIZE_TRACE_HEADER +
                         FloatingPointFormatter::GetFloatArrayRealSize(NumbersConvertor::ToLittleEndian(thl.NS),
                                                                       format);
        }
    }
    auto hdt = NumbersConvertor::ToLittleEndian(this->mBinaryHeaderLookup.HDT);
    for (auto const &traces : gather_map) {
        auto g = new Gather();
        std::string key = traces.first;
        std::vector<std::string> keys = TraceHeaderKey::StringToGatherValues(key);
        for (int i = 0; i < keys.size(); i++) {
            g->SetUniqueKeyValue(this->mGatherKeys[i],
                                 keys[i]);
        }
        g->AddTrace(traces.second);
        g->SetSamplingRate(hdt);
        gathers.push_back(g);
    }
    return gathers;
}

Gather *
SegyReader::Read(std::vector<std::string> aHeaderValues) {
    if (aHeaderValues.size() != this->mGatherKeys.size()) {
        return nullptr;
    }

    std::unordered_map<int, std::vector<dataunits::Trace *>> gather_map;
    auto gather = new Gather();
    gather->SetSamplingRate(NumbersConvertor::ToLittleEndian(this->mBinaryHeaderLookup.HDT));

    std::string key = TraceHeaderKey::GatherKeysToString(this->mGatherKeys);
    std::string value = TraceHeaderKey::GatherValuesToString(aHeaderValues);

    for (int ig = 0; ig < this->mIndexMaps.size(); ++ig) {
        auto bytes = this->mIndexMaps[ig].Get(key, value);
        if (!bytes.empty()) {
            auto stream = this->mInStreamHelpers[ig];
            for (auto &pos : bytes) {
                /* Read trace header in the given file. */
                auto thl = stream->ReadTraceHeader(pos);
                /* Read trace data in the given file. */
                auto trace = stream->ReadFormattedTraceData(pos + IO_SIZE_TRACE_HEADER, thl,
                                                            this->mBinaryHeaderLookup);
                gather->AddTrace(trace);
            }
        }
    }
    for (int i = 0; i < aHeaderValues.size(); i++) {
        gather->SetUniqueKeyValue(this->mGatherKeys[i],
                                  aHeaderValues[i]);
    }
    return gather;
}

std::vector<Gather *>
SegyReader::Read(std::vector<std::vector<std::string>> aHeaderValues) {
    std::vector<Gather *> results;
    for (const auto &header_value : aHeaderValues) {
        Gather *gather = this->Read(header_value);
        if (gather != nullptr) {
            results.push_back(gather);
        }
    }
    return results;
}

Gather *
SegyReader::Read(unsigned int aIndex) {
    auto identifiers = this->GetIdentifiers();
    if (aIndex >= identifiers.size()) {
        throw ILLOGICAL_EXCEPTION();
    }
    return this->Read(identifiers[aIndex]);
}

std::vector<std::vector<std::string>>
SegyReader::GetIdentifiers() {
    std::vector<std::vector<std::string>> keys;
    for (auto &mIndexMap : this->mIndexMaps) {
        auto map = mIndexMap.Get();
        std::string key = TraceHeaderKey::GatherKeysToString(this->mGatherKeys);
        for (const auto &entry : map[key]) {
            if (!entry.second.empty()) {
                std::vector<std::string> val = TraceHeaderKey::StringToGatherValues(entry.first);
                keys.push_back(val);
            }
        }
    }
    return keys;
}

unsigned int
SegyReader::GetNumberOfGathers() {
    unsigned int gather_number = 0;
    for (auto &mIndexMap : this->mIndexMaps) {
        auto map = mIndexMap.Get();
        std::string key = TraceHeaderKey::GatherKeysToString(this->mGatherKeys);
        for (const auto &entry : map[key]) {
            if (!entry.second.empty()) {
                gather_number++;
            }
        }
    }
    return gather_number;
}

bool
SegyReader::HasExtendedTextHeader() const {
    return this->mHasExtendedTextHeader;
}

unsigned char *
SegyReader::GetTextHeader() {
    unsigned char *text_header = nullptr;
    if (this->mTextHeader != nullptr) {
        text_header = StringsConvertor::E2A(this->mTextHeader, IO_SIZE_TEXT_HEADER);
    }
    return text_header;
}

unsigned char *
SegyReader::GetExtendedTextHeader() {
    unsigned char *text_header = nullptr;
    if (this->mTextHeader != nullptr) {
        text_header = StringsConvertor::E2A(this->mExtendedHeader, IO_SIZE_TEXT_HEADER);
    }
    return text_header;
}

int
SegyReader::Index() {
    this->mFileIndexers.reserve(this->mPaths.size());
    std::string key = TraceHeaderKey::GatherKeysToString(this->mGatherKeys);
    for (auto &it : this->mPaths) {
        this->mFileIndexers.push_back(FileIndexer(it, key));
    }
    for (auto &it : this->mFileIndexers) {
        it.Initialize();
        this->mIndexMaps.push_back(it.Index(this->mGatherKeys));
        it.Finalize();
    }
    return BS_BASE_RC_SUCCESS;
}
