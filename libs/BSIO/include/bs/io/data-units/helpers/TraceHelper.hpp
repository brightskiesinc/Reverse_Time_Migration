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

#ifndef BS_IO_DATA_UNITS_HELPERS_TRACE_HELPER_HPP
#define BS_IO_DATA_UNITS_HELPERS_TRACE_HELPER_HPP

#include <bs/io/data-units/concrete/Trace.hpp>
#include <bs/io/lookups/tables/TraceHeaderLookup.hpp>
#include <bs/io/lookups/tables/BinaryHeaderLookup.hpp>

namespace bs {
    namespace io {
        namespace dataunits {
            namespace helpers {

                /**
                 * @brief Trace helper to take any trace data unit and helps manipulate
                 * or get any regarded meta data from it.
                 */
                class TraceHelper {
                public:
                    static int
                    Weight(Trace *&apTrace,
                           lookups::TraceHeaderLookup &aTraceHeaderLookup,
                           lookups::BinaryHeaderLookup &aBinaryHeaderLookup);

                    static int
                    WeightData(Trace *&apTrace,
                               lookups::TraceHeaderLookup &aTraceHeaderLookup,
                               lookups::BinaryHeaderLookup &aBinaryHeaderLookup);
                };
            } //namespace helpers
        } //namespace dataunits
    } //namespace io
} //namespace bs

#endif //BS_IO_DATA_UNITS_HELPERS_TRACE_HELPER_HPP
