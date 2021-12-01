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

#ifndef BS_IO_UTILS_SYNTHETIC_GENERATORS_SHOTS_META_DATA_GENERATOR_HPP
#define BS_IO_UTILS_SYNTHETIC_GENERATORS_SHOTS_META_DATA_GENERATOR_HPP

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <bs/io/utils/synthetic-generators/interface/MetaDataGenerator.hpp>

namespace bs {
    namespace io {
        namespace generators {
            /**
             * @brief
             * Struct used to contain 3D points used for the generation.
             */
            struct Point3D {
                int x;
                int y;
                int z;
            };

            /**
             * @brief
             * Meta data trace generator for the given shots.
             */
            class ShotsMetaDataGenerator : public MetaDataGenerator {
            public:
                /**
                 * @brief
                 * Constructor.
                 *
                 * @param[in] aJsonNode
                 * JSON node containing the information needed for the generation.
                 */
                explicit
                ShotsMetaDataGenerator(nlohmann::json aJsonNode);

                ~ShotsMetaDataGenerator() override;

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
                /**
                 * @brief
                 * Return the total number of sources.
                 *
                 * @return
                 * The total number of sources to be generated.
                 */
                uint GetSourceNumber() const;

                /**
                 * @brief
                 * Return the total number of receivers per source.
                 *
                 * @return
                 * The total number of receiver per each shot.
                 */
                uint GetReceiverNumber() const;

            private:
                /// The shots sampling.
                float mSamplingX;
                float mSamplingY;
                float mTimeSampling;
                /// The number of samples.
                uint mSampleNumber;
                /// The source settings.
                Point3D mSourceStart;
                Point3D mSourceEnd;
                Point3D mSourceIncrement;
                /// The receiver settings.
                Point3D mReceiverRelStart;
                Point3D mReceiverNumber;
                Point3D mReceiverIncrement;
                /// The origin coordinates in space.
                float mOriginX;
                float mOriginY;
                /// The gather key used.
                io::dataunits::TraceHeaderKey mGatherKey;
            };
        }
    }
}

#endif //BS_IO_UTILS_SYNTHETIC_GENERATORS_SHOTS_META_DATA_GENERATOR_HPP
