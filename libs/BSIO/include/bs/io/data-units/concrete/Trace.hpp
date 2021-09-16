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

#ifndef BS_IO_DATA_UNITS_TRACE_HPP
#define BS_IO_DATA_UNITS_TRACE_HPP

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>
#include <bs/base/common/assertions.h>

#include <unordered_map>
#include <memory>
#include <sstream>

namespace bs {
    namespace io {
        namespace dataunits {
            /**
             * @brief Trace Object contains TraceHeaderKey represented as a map.
             */
            class Trace {
            public:
                /**
                 * @brief Constructor
                 */
                explicit Trace(unsigned short aNS);

                /**
                 * @brief Move constructor, to move ownership of TraceData.
                 *
                 * @param aTrace
                 */
                Trace(Trace &&aTrace) noexcept;

                /**
                 * @brief Copy constructor.
                 */
                Trace(const Trace &aTrace);

                /**
                 * @brief Destructor
                 */
                ~Trace();

                /**
                 * @brief Assigment operator overloading.
                 */
                Trace &operator=(Trace &&aTrace) noexcept;

                /**
                 * @brief Getter for the Trace Header Value.
                 *
                 * @param[in] aTraceHeaderKey.
                 *
                 * @return Trace Header Value if available, else find() will
                 * point to the end of map.
                 */
                template<typename T>
                inline T GetTraceHeaderKeyValue(TraceHeaderKey aTraceHeaderKey) {
                    ASSERT_T_TEMPLATE(T);
                    std::stringstream ss;
                    ss << this->mTraceHeaderMap[aTraceHeaderKey];
                    T trace_header_value;
                    ss >> trace_header_value;
                    return trace_header_value;
                }

                /**
                 * @brief
                 * Check if a given trace header key exists.
                 *
                 * @param[in] aTraceHeaderKey
                 * The trace header key to check.
                 *
                 * @return
                 * True if an entry exists for it.
                 */
                bool HasTraceHeader(TraceHeaderKey aTraceHeaderKey) {
                    return this->mTraceHeaderMap.find(aTraceHeaderKey) != this->mTraceHeaderMap.end();
                }

                const std::unordered_map<TraceHeaderKey, std::string> *GetTraceHeaders() {
                    return &this->mTraceHeaderMap;
                }

                /**
                 * @brief Setter for the Trace Header Value.
                 *
                 * @param[in] aTraceHeaderKey
                 * Trace Header Key argument.
                 *
                 * @param[in] aValue
                 * Value to set for the corresponding key
                 */
                template<typename T>
                inline void SetTraceHeaderKeyValue(TraceHeaderKey aTraceHeaderKey, T aValue) {
                    ASSERT_T_TEMPLATE(T);
                    this->mTraceHeaderMap[aTraceHeaderKey] = std::to_string(aValue);
                }

                /**
                 * @brief Setter for trace data.
                 */
                inline void SetTraceData(float *apTraceData) {
                    return this->mpTraceData.reset(apTraceData);
                }

                /**
                 * @brief Getter for trace data.
                 *
                 * @return Pointer to trace data.
                 */
                inline float *GetTraceData() {
                    return this->mpTraceData.get();
                }

                /**
                 * @brief Getter for number of samples trace header.
                 *
                 * @return Number of samples value.
                 */
                inline unsigned short GetNumberOfSamples() {
                    TraceHeaderKey trace_header_key(TraceHeaderKey::NS);
                    return this->GetTraceHeaderKeyValue<unsigned short>(trace_header_key);
                }

                /**
                 * @brief Setter for any coordinate location.
                 * This will scale the given float based on the SCALCO header.
                 */
                void
                SetScaledCoordinateHeader(TraceHeaderKey aKey, float aLocation);

                /**
                 * @brief Getter for source location. Should hide all the preprocessing from the user.
                 *
                 * @return Source Location.
                 */
                float
                GetScaledCoordinateHeader(TraceHeaderKey aKey);

            private:
                /// @brief TraceHeaderKey, containing trace headers and their corresponding value.
                /// @key TraceHeaderKey
                /// @value string
                std::unordered_map<TraceHeaderKey, std::string> mTraceHeaderMap;

                /// @brief TraceData, containing trace data.
                std::unique_ptr<float[]> mpTraceData;
            };
        }//namespace dataunits
    } //namespace io
} //namespace bs

#endif //BS_IO_DATA_UNITS_TRACE_HPP
