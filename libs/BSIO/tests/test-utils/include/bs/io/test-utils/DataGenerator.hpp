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

#ifndef TIMER_TESTS_UTILS_FUNCTIONS_GENERATOR_HPP
#define TIMER_TESTS_UTILS_FUNCTIONS_GENERATOR_HPP

#include <bs/io/data-units/concrete/Gather.hpp>

#include <vector>

namespace bs {
    namespace io {
        namespace testutils {

            /**
             * @brief Data generator class to generate dummy traces and gathers
             * for testing purposes.
             */
            class DataGenerator {
            public:
                /**
                 * @brief Generates single dummy trace.
                 * @param aNS
                 * @param aFLDR
                 * @return Generated trace
                 */
                static dataunits::Trace *
                GenerateTrace(uint16_t aNS, int32_t aFLDR);

                /**
                 * @brief Generates vector of dummy traces, by giving number of generated
                 * traces and a starting FLDR value.
                 * @param aNS
                 * @param aCount
                 * @param aStartFLDR
                 * @return Generated trace vector
                 */
                static std::vector<dataunits::Trace *>
                GenerateTraceVector(uint16_t aNS, int aCount, int32_t aStartFLDR);

                /**
                 * @brief Generates vector of dummy traces, by giving vector of FLDRs.
                 * @param aNS
                 * @param aFLDR
                 * @return Generated trace vector
                 */
                static std::vector<dataunits::Trace *>
                GenerateTraceVector(uint16_t aNS, const std::vector<int32_t> &aFLDR);

                /**
                 * @brief Generates single dummy gather, by giving number of generated
                 * traces inside and a starting FLDR value.
                 * @param aNS
                 * @param aCount
                 * @param aStartFLDR
                 * @return Generated gather.
                 */
                static dataunits::Gather *
                GenerateGather(uint16_t aNS, int aCount, int32_t aStartFLDR);

            private:
                DataGenerator();
            };

        } //namespace testutils
    } //namespace io
} //namespace bs

#endif //TIMER_TESTS_UTILS_FUNCTIONS_GENERATOR_HPP
