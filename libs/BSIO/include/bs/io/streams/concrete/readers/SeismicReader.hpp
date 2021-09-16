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

#ifndef BS_IO_STREAMS_SEISMIC_READER_HPP
#define BS_IO_STREAMS_SEISMIC_READER_HPP

#include <bs/io/streams/primitive/Reader.hpp>
#include <unordered_map>

namespace bs {
    namespace io {
        namespace streams {

            enum class ReaderType {
                SEGY,
                JSON,
                SU,
                PSG,
                DSG
            };

            /**
             * @brief High-level wrapper for all seismic readers.
             */
            class SeismicReader : public Reader {
            public:
                /**
                 * @brief Constructor
                 */
                explicit SeismicReader(
                        ReaderType aType,
                        bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief
                 * Convert a string into its equivalent reader type enum.
                 *
                 * @param[in] aString
                 * The string to convert.
                 *
                 * @return
                 * The reader type.
                 */
                static ReaderType ToReaderType(const std::string &aString);

                /**
                 * @brief
                 * Convert a reader type to its equivalent string.
                 *
                 * @param[in] aType
                 * The reader type to convert.
                 *
                 * @return
                 * A string representation of the reader type supplied.
                 */
                static std::string ToString(ReaderType aType);

                /**
                 * @brief Destructor.
                 */
                ~SeismicReader() override;

                void AcquireConfiguration() override;

                std::string GetExtension() override;

                int Initialize(std::vector<std::string> &aGatherKeys,
                               std::vector<std::pair<std::string, dataunits::Gather::SortDirection>> &aSortingKeys,
                               std::vector<std::string> &aPaths) override;

                int Initialize(std::vector<dataunits::TraceHeaderKey> &aGatherKeys,
                               std::vector<std::pair<dataunits::TraceHeaderKey, dataunits::Gather::SortDirection>> &aSortingKeys,
                               std::vector<std::string> &aPaths) override;

                int Finalize() override;

                void SetHeaderOnlyMode(bool aEnableHeaderOnly) override;

                unsigned int GetNumberOfGathers() override;

                std::vector<std::vector<std::string>> GetIdentifiers() override;

                std::vector<dataunits::Gather *> ReadAll() override;

                std::vector<dataunits::Gather *> Read(std::vector<std::vector<std::string>> aHeaderValues) override;

                dataunits::Gather *Read(std::vector<std::string> aHeaderValues) override;

                dataunits::Gather *Read(unsigned int aIndex) override;

            private:
                /// The reader pointer.
                Reader *mpReader;
                /// The static mapping of reader types to strings.
                static std::unordered_map<std::string, ReaderType> mReaderMap;
            };
        }
    }
} //namespace bs

#endif //BS_IO_STREAMS_SEISMIC_READER_HPP
