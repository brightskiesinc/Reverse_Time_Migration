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

#ifndef BS_IO_UTILS_SYNTHETIC_GENERATORS_PLANE_REFLECTOR_GENERATOR_HPP
#define BS_IO_UTILS_SYNTHETIC_GENERATORS_PLANE_REFLECTOR_GENERATOR_HPP

#include <bs/io/utils/synthetic-generators/interface/ReflectorDataGenerator.hpp>

namespace bs {
    namespace io {
        namespace generators {
            class PlaneReflectorGenerator : public ReflectorDataGenerator {
            public:
                PlaneReflectorGenerator();

                ~PlaneReflectorGenerator() override;

                void
                ParseValues(nlohmann::json &aJsonNode) override;

                float
                GetReflectorDepth(float x, float y) override;

                float
                GetBeforeValue() override;

                float
                GetAfterValue() override;

            private:
                /// The values before and after the reflector.
                float mValueBefore;
                float mValueAfter;
                /// Origin points.
                float mOriginX;
                float mOriginY;
                float mOriginZ;
                /// The slope angles of the plane.
                float mXSlopeAngle;
                float mYSlopeAngle;
            };
        }
    }
}

#endif //BS_IO_UTILS_SYNTHETIC_GENERATORS_PLANE_REFLECTOR_GENERATOR_HPP
