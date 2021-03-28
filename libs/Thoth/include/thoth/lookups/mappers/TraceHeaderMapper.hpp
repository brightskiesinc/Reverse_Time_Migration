//
// Created by zeyad-osama on 12/03/2021.
//

#ifndef THOTH_LOOKUPS_MAPPERS_TRACE_HEADER_MAPPER_HPP
#define THOTH_LOOKUPS_MAPPERS_TRACE_HEADER_MAPPER_HPP

#include <thoth/lookups/tables/TraceHeaderLookup.hpp>
#include <thoth/data-units/data-types/TraceHeaderKey.hpp>

#include <cstdio>

namespace thoth {
    namespace lookups {

        class TraceHeaderMapper {
        public:
            static size_t
            GetTraceHeaderValue(const dataunits::TraceHeaderKey::Key &aTraceHeaderKey,
                                const TraceHeaderLookup &aTraceHeaderLookup);
        };

    } //namespace lookups
} //namespace thoth

#endif //THOTH_LOOKUPS_MAPPERS_TRACE_HEADER_MAPPER_HPP
