//
// Created by zeyad-osama on 02/11/2020.
//

#include <thoth/data-units/concrete/Gather.hpp>

#include <thoth/data-units/data-types/TraceHeaderKey.hpp>

#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>

using namespace thoth::dataunits;


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

void Gather::SortGather(const std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> &aSortingKeys) {
    sort(this->mTraces.begin(), this->mTraces.end(), trace_compare_t(aSortingKeys));
}

trace_compare_t::trace_compare_t(
        const std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> &aSortingKeys) {
    for (auto &e : aSortingKeys) {
        mSortingKeys.push_back(e);
    }
    mKeysSize = aSortingKeys.size();
}

bool trace_compare_t::operator()(Trace *aTrace_1, Trace *aTrace_2) const {
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
