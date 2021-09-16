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

#ifndef BS_IO_UTILS_SYNTHETIC_GENERATORS_REFLECTOR_DATA_GENERATOR_HPP
#define BS_IO_UTILS_SYNTHETIC_GENERATORS_REFLECTOR_DATA_GENERATOR_HPP

#include <prerequisites/libraries/nlohmann/json.hpp>

namespace bs {
    namespace io {
        namespace generators {
            /**
             * @brief
             * Interface for all the reflector data generators.
             */
            class ReflectorDataGenerator {
            public:
                /**
                 * @brief
                 * Default constructor.
                 */
                ReflectorDataGenerator() = default;

                /**
                 * @brief
                 * Destructor.
                 */
                virtual ~ReflectorDataGenerator() = default;

                /**
                 * @brief
                 * Acquire the values from a given json node.
                 *
                 * @param[in] aJsonNode
                 * The json node to parse properties from.
                 */
                virtual void
                ParseValues(nlohmann::json &aJsonNode) = 0;

                /**
                 * @brief
                 * Calculate the reflector depth at a given point in space.
                 *
                 * @param[in] x
                 * The point in x.
                 *
                 * @param[in] y
                 * The point in y.
                 *
                 * @return
                 * The depth calculated of the reflector.
                 */
                virtual float
                GetReflectorDepth(float x, float y) = 0;

                /**
                 * @brief
                 * Get the value that is below the reflector.
                 *
                 * @return
                 * The value right below the reflector.
                 */
                virtual float
                GetAfterValue() = 0;

                /**
                 * @brief
                 * Get the value that is above the reflector.
                 *
                 * @return
                 * The value right above the reflector.
                 */
                virtual float
                GetBeforeValue() = 0;
            };
        }
    }
}

#endif //BS_IO_UTILS_SYNTHETIC_GENERATORS_REFLECTOR_DATA_GENERATOR_HPP
