//
// Created by pancee on 1/14/21.
//

#ifndef THOTH_LOOKUPS_SEISMIC_FILES_HEADERS_HPP
#define THOTH_LOOKUPS_SEISMIC_FILES_HEADERS_HPP

#include <thoth/data-units/data-types/TraceHeaderKey.hpp>

#include <thoth/utils/range/ByteRange.hpp>

#include <map>

namespace thoth {
    namespace lookups {

        class SeismicFilesHeaders {
        public:
            static const std::map<dataunits::TraceHeaderKey, utils::range::ByteRange> mTraceHeadersMap;
            static const std::map<dataunits::TraceHeaderKey, utils::range::ByteRange> mBinaryHeadersMap;

        public:
            static utils::range::ByteRange GetByteRangeByKey(dataunits::TraceHeaderKey aTraceHeaderKey) {
                return SeismicFilesHeaders::mTraceHeadersMap.find(aTraceHeaderKey)->second;
            }
        };
    }
}

#endif //THOTH_LOOKUPS_SEISMIC_FILES_HEADERS_HPP