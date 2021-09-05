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

#ifndef BS_IO_STREAMS_TEXT_READER_HPP
#define BS_IO_STREAMS_TEXT_READER_HPP

#include <bs/io/streams/primitive/Reader.hpp>
#include <bs/io/utils/synthetic-generators/interface/MetaDataGenerator.hpp>
#include <bs/io/utils/synthetic-generators/interface/ReflectorDataGenerator.hpp>

namespace bs {
    namespace io {
        namespace streams {
            /**
             * @brief
             */
            class JsonReader : public Reader {
            public:
                /**
                 * @brief Constructor
                 */
                explicit JsonReader(bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Default destructor.
                 */
                ~JsonReader() override;

                void
                AcquireConfiguration() override;

                std::string GetExtension() override;

                int
                Initialize(std::vector<std::string> &aGatherKeys,
                           std::vector<std::pair<std::string, dataunits::Gather::SortDirection>> &aSortingKeys,
                           std::vector<std::string> &aPaths) override;

                int
                Initialize(std::vector<dataunits::TraceHeaderKey> &aGatherKeys,
                           std::vector<std::pair<dataunits::TraceHeaderKey, dataunits::Gather::SortDirection>> &aSortingKeys,
                           std::vector<std::string> &aPaths) override;

                int
                Finalize() override;

                void
                SetHeaderOnlyMode(bool aEnableHeaderOnly) override;

                unsigned int
                GetNumberOfGathers() override;

                std::vector<std::vector<std::string>>
                GetIdentifiers() override;

                std::vector<dataunits::Gather *>
                ReadAll() override;

                std::vector<dataunits::Gather *>
                Read(std::vector<std::vector<std::string>> aHeaderValues) override;

                io::dataunits::Gather *
                Read(std::vector<std::string> aHeaderValues) override;

                io::dataunits::Gather *
                Read(unsigned int aIndex) override;

            private:
                /**
                 * @brief
                 * Generate the different generators and components used for the generation of data.
                 */
                void GenerateComponents();

                /**
                 * @brief
                 * Fill the traces of a given gather with actual data.
                 *
                 * @param[in] aGathers
                 * List of gathers to fill with data.
                 */
                void FillTraceData(std::vector<io::dataunits::Gather *> &aGathers);

            private:
                /// Files path
                std::string mFilePath;
                /// mGatherKeys: vector of keys to gather on
                std::vector<dataunits::TraceHeaderKey> mGatherKeys;
                /// mSortingKeys: Keys to sort gathers on, and sorting direction of each key
                /// Optional to have file(s) gathers sorted
                std::vector<std::pair<dataunits::TraceHeaderKey, dataunits::Gather::SortDirection>> mSortingKeys;
                /// Enable header only mode boolean
                bool mEnableHeaderOnly;
                /// The meta data generator.
                io::generators::MetaDataGenerator *mpMetaGenerator;
                /// The reflector generators.
                std::vector<io::generators::ReflectorDataGenerator *> mReflectorGenerators;
            };

        } //streams
    } //thoth
} //namespace bs

#endif //BS_IO_STREAMS_TEXT_READER_HPP
