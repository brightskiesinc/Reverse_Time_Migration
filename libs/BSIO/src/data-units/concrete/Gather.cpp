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

#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>

using namespace bs::io::dataunits;

Gather::Gather()
        : mUniqueKeys(), mTraces() {}

Gather::Gather(std::unordered_map<TraceHeaderKey, std::string> &aUniqueKeys,
               const std::vector<Trace *> &aTraces) {
    this->mUniqueKeys = std::move(aUniqueKeys);
    for (auto &trace : aTraces) {
        this->mTraces.push_back(trace);
    }
}

Gather::Gather(TraceHeaderKey aUniqueKey,
               const std::string &aUniqueKeyValue,
               const std::vector<Trace *> &aTraces) {
    this->mUniqueKeys.insert({aUniqueKey, aUniqueKeyValue});
    for (auto &trace : aTraces) {
        this->mTraces.push_back(trace);
    }
}

Gather::Gather(std::unordered_map<TraceHeaderKey, std::string> &aUniqueKeys)
        : mTraces() {
    this->mUniqueKeys = std::move(aUniqueKeys);
}

void
Gather::SortGather(const std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> &aSortingKeys) {
    if (!aSortingKeys.empty()) {
        sort(this->mTraces.begin(), this->mTraces.end(), trace_compare_t(aSortingKeys));
    }
}

trace_compare_t::trace_compare_t(
        const std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> &aSortingKeys) {
    for (auto &e : aSortingKeys) {
        mSortingKeys.push_back(e);
    }
    mKeysSize = aSortingKeys.size();
}

bool
trace_compare_t::operator()(Trace *aTrace_1, Trace *aTrace_2) const {
    int i = 0;
    bool swap = false;
    bool ascending;
    float trace_1_header, trace_2_header;

    do {
        trace_1_header = aTrace_1->GetTraceHeaderKeyValue<float>(mSortingKeys[i].first);
        trace_2_header = aTrace_2->GetTraceHeaderKeyValue<float>(mSortingKeys[i].first);
        if (trace_1_header != trace_2_header) {
            break;
        }
        i++;
    } while (i < mKeysSize);
    if (i < mKeysSize) {
        ascending = mSortingKeys[i].second;
        if (ascending == Gather::SortDirection::ASC) {
            swap = trace_1_header < trace_2_header;
        } else {
            swap = trace_1_header > trace_2_header;
        }
    }
    return swap;
}
