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

#ifndef BS_IO_UTILS_SYNTHETIC_GENERATORS_PARAMETER_META_DATA_GENERATOR_HPP
#define BS_IO_UTILS_SYNTHETIC_GENERATORS_PARAMETER_META_DATA_GENERATOR_HPP

#include <bs/io/utils/synthetic-generators/interface/MetaDataGenerator.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

namespace bs {
    namespace io {
        namespace generators {
            class ParameterMetaDataGenerator : public MetaDataGenerator {
            public:
                explicit ParameterMetaDataGenerator(nlohmann::json aJsonNode);

                ~ParameterMetaDataGenerator() override = default;

                void
                SetGenerationKey(std::vector<io::dataunits::TraceHeaderKey> &aHeaderKeys) override;

                std::vector<io::dataunits::Gather *>
                GetAllTraces() override;

                std::vector<io::dataunits::Gather *>
                GetTraces(std::vector<std::vector<std::string>> aHeaderValues) override;

                std::vector<std::vector<std::string>>
                GetGatherUniqueValues() override;

                uint
                GetGatherNumber() override;

            private:
                /// The parameter model sampling.
                float mSamplingX;
                float mSamplingY;
                float mSamplingZ;
                /// The number of points
                uint mPointsX;
                uint mPointsY;
                uint mPointsZ;
                /// The origin coordinates in space.
                float mOriginX;
                float mOriginY;
                /// The gather key used.
                io::dataunits::TraceHeaderKey mGatherKey;
            };
        }
    }
}

#endif //BS_IO_UTILS_SYNTHETIC_GENERATORS_PARAMETER_META_DATA_GENERATOR_HPP
