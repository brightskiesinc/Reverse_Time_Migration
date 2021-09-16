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

#include <bs/io/indexers/IndexMap.hpp>

#include <bs/base/common/ExitCodes.hpp>

using namespace bs::io::indexers;
using namespace bs::io::dataunits;


IndexMap::IndexMap() = default;

IndexMap::~IndexMap() = default;

int
IndexMap::Reset() {
    this->mIndexMap.clear();
    return BS_BASE_RC_SUCCESS;
}

std::map<std::string, std::vector<size_t>>
IndexMap::Get(const std::string &aTraceHeaderKey, std::vector<std::string> &aTraceHeaderValues) {
    std::map<std::string, std::vector<size_t>> bytes;
    for (auto &it : aTraceHeaderValues) {
        bytes[it] = this->Get(aTraceHeaderKey, it);
    }
    return bytes;
}

int
IndexMap::Add(const std::string &aTraceHeaderKey,
              const std::string &aTraceHeaderValue,
              const size_t &aBytePosition) {
    this->mIndexMap[aTraceHeaderKey][aTraceHeaderValue].push_back(aBytePosition);
    return BS_BASE_RC_SUCCESS;
}

int
IndexMap::Add(const std::string &aTraceHeaderKey,
              const std::string &aTraceHeaderValue,
              const std::vector<size_t> &aBytePositions) {
    int rc = 0;
    for (auto const &it : aBytePositions) {
        rc += this->Add(aTraceHeaderKey, aTraceHeaderValue, it);
    }
    /* Check that all Add() functions returned BS_BASE_RC_SUCCESS signal. */
    return aBytePositions.empty() ? BS_BASE_RC_SUCCESS : (rc / aBytePositions.size()) == BS_BASE_RC_SUCCESS;
}