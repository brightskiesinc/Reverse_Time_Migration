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

#ifndef BS_IO_INDEXERS_INDEX_MAP_HPP
#define BS_IO_INDEXERS_INDEX_MAP_HPP

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>

#include <vector>

namespace bs {
    namespace io {
        namespace indexers {

            class IndexMap {
            public:
                /**
                 * @brief Constructor.
                 */
                IndexMap();

                /**
                 * @brief Destructor.
                 */
                ~IndexMap();

                /**
                 * @brief Resets current map for reusable purposes.
                 * @return Status flag.
                 */
                int
                Reset();

                /**
                 * @brief Adds a byte position to the vector corresponding to the desired trace header key.
                 * @param[in] aTraceHeaderKey
                 * @param[in] aTraceHeaderValue
                 * @param[in] aBytePosition
                 * @return Status flag.
                 */
                int
                Add(const std::string &aTraceHeaderKey,
                    const std::string &aTraceHeaderValue,
                    const size_t &aBytePosition);

                /**
                 * @brief Adds a byte position vector corresponding to the desired trace header key.
                 * @param[in] aTraceHeaderKey
                 * @param[in] aTraceHeaderValue
                 * @param[in] aBytePositions
                 * @return Status flag.
                 */
                int
                Add(const std::string &aTraceHeaderKey,
                    const std::string &aTraceHeaderValue,
                    const std::vector<size_t> &aBytePositions);

                /**
                 * @brief Gets vector of byte positions corresponding to the desired trace header key and value.
                 * @param[in] aTraceHeaderKey
                 * @param[in] aTraceHeaderValue
                 * @return Byte positions vector.
                 */
                inline std::vector<size_t> &
                Get(const std::string &aTraceHeaderKey,
                    const std::string &aTraceHeaderValue) {
                    return this->mIndexMap[aTraceHeaderKey][aTraceHeaderValue];
                }

                /**
                 * @brief Gets map of byte positions corresponding to the desired trace header key and value.
                 * @param[in] aTraceHeaderKey
                 * @param[in] aTraceHeaderValue
                 * @return Byte positions map.
                 */
                std::map<std::string, std::vector<size_t>>
                Get(const std::string &aTraceHeaderKey,
                    std::vector<std::string> &aTraceHeaderValues);

                /**
                 * @brief Gets map of trace header values as keys and byte positions as values,
                 * corresponding to the desired trace header key.
                 *
                 * @param[in] aTraceHeaderKey
                 * @return Byte positions vector.
                 */
                inline std::map<std::string, std::vector<size_t>> &
                Get(const std::string &aTraceHeaderKey) { return this->mIndexMap[aTraceHeaderKey]; }

                /**
                 * @brief Gets complete map. (i.e. Each trace header key and it's corresponding byte positions vector.)
                 * @return Complete map.
                 */
                inline std::unordered_map<std::string, std::map<std::string, std::vector<size_t>>> &
                Get() { return this->mIndexMap; }

                inline bool
                IsEmpty() { return this->mIndexMap.empty(); }

            private:
                /// Index map, having trace header key as key and vector of byte positions
                /// in file corresponding to this trace header key.
                std::unordered_map<std::string, std::map<std::string, std::vector<size_t>>> mIndexMap;
            };

        } //namespace indexers
    } //namespace io
} //namespace bs

#endif //BS_IO_INDEXERS_INDEX_MAP_HPP
