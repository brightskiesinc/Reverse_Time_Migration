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

#ifndef BS_IO_LOOKUPS_SEISMIC_FILES_HEADERS_HPP
#define BS_IO_LOOKUPS_SEISMIC_FILES_HEADERS_HPP

#include <map>

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>
#include <bs/io/utils/range/EntryRange.hpp>

namespace bs {
    namespace io {
        namespace lookups {

            class SeismicFilesHeaders {
            public:
                static utils::range::EntryRange GetEntryRange(dataunits::TraceHeaderKey aTraceHeaderKey) {
                    return SeismicFilesHeaders::mTraceHeadersMap.find(aTraceHeaderKey)->second;
                }

                static std::map<dataunits::TraceHeaderKey, utils::range::EntryRange> GetTraceHeadersMap() {
                    return SeismicFilesHeaders::mTraceHeadersMap;
                }

            private:
                static const std::map<dataunits::TraceHeaderKey, utils::range::EntryRange> mTraceHeadersMap;
                static const std::map<dataunits::TraceHeaderKey, utils::range::EntryRange> mBinaryHeadersMap;
            };
        }
    }
} //namespace bs

#endif //BS_IO_LOOKUPS_SEISMIC_FILES_HEADERS_HPP