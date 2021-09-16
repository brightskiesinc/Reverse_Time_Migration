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

#ifndef BS_IO_LOOKUPS_MAPPERS_HEADER_MAPPER_HPP
#define BS_IO_LOOKUPS_MAPPERS_HEADER_MAPPER_HPP

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>
#include <bs/io/data-units/concrete/Trace.hpp>
#include <unordered_map>

#include <cstdio>

namespace bs {
    namespace io {
        namespace lookups {
            /**
             * @brief
             * Enum representing the native type of different entries.
             */
            enum class NATIVE_TYPE {
                CHAR,
                UNSIGNED_CHAR,
                SHORT,
                UNSIGNED_SHORT,
                LONG,
                UNSIGNED_LONG,
                INT,
                UNSIGNED_INT,
                FLOAT,
                DOUBLE
            };

            size_t native_type_to_size(NATIVE_TYPE native_type);

            /**
             * @brief
             * Utility class for header mapping between raw byte pointers, and the trace object.
             */
            class HeaderMapper {
            public:
                /**
                 * @brief
                 * Map data from a raw byte pointer to a trace object according to a given map.
                 *
                 * @param[in] apRawData
                 * The byte pointer containing the raw data.
                 *
                 * @param[in] aTrace
                 * The trace to set the headers in.
                 *
                 * @param[in] aLocationTable
                 * The location table used, keys being trace header key enums,
                 * values are pairs of offsets in the raw data and their native type.
                 *
                 * @param[in] aSwap
                 * Whether to swap bytes or not(Transform endianness).
                 */
                static void MapHeaderToTrace(
                        const char *apRawData,
                        io::dataunits::Trace &aTrace,
                        std::unordered_map<dataunits::TraceHeaderKey::Key,
                                std::pair<size_t, NATIVE_TYPE>> &aLocationTable,
                        bool aSwap = true);

                /**
                 * @brief
                 * Map data from a trace object to a raw byte pointer according to a given map.
                 *
                 * @param[in] apRawData
                 * The byte pointer containing the raw data.
                 *
                 * @param[in] aTrace
                 * The trace to set the headers in.
                 *
                 * @param[in] aLocationTable
                 * The location table used, keys being trace header key enums,
                 * values are pairs of offsets in the raw data and their native type.
                 *
                 * @param[in] aSwap
                 * Whether to swap bytes or not(Transform endianness).
                 */
                static void MapTraceToHeader(
                        char *apRawData,
                        io::dataunits::Trace &aTrace,
                        std::unordered_map<dataunits::TraceHeaderKey::Key,
                                std::pair<size_t, NATIVE_TYPE>> &aLocationTable,
                        bool aSwap = true);
            };

        } //namespace lookups
    } //namespace io
} //namespace bs

#endif //BS_IO_LOOKUPS_MAPPERS_TRACE_HEADER_MAPPER_HPP
