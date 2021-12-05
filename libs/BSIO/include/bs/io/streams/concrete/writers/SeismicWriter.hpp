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

#ifndef BS_IO_STREAMS_SEISMIC_WRITER_HPP
#define BS_IO_STREAMS_SEISMIC_WRITER_HPP

#include <unordered_map>

#include <bs/io/streams/primitive/Writer.hpp>

namespace bs {
    namespace io {
        namespace streams {

            enum class WriterType {
                SEGY,
                BINARY,
                IMAGE,
                CSV,
                SU,
            };

            /**
             * @brief High-level wrapper for all seismic writers.
             */
            class SeismicWriter : public Writer {
            public:
                /**
                 * @brief Constructor
                 */
                explicit SeismicWriter(WriterType aType,
                                       bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Destructor
                 */
                ~SeismicWriter() override;

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
                static WriterType
                ToWriterType(const std::string &aString);

                /**
                 * @brief
                 * Convert a writer type to its equivalent string.
                 *
                 * @param[in] aType
                 * The writer type to convert.
                 *
                 * @return
                 * A string representation of the writer type supplied.
                 */
                static std::string
                ToString(WriterType aType);

                void
                AcquireConfiguration() override;

                std::string
                GetExtension() override;

                int
                Initialize(std::string &aFilePath) override;

                int
                Finalize() override;

                int
                Write(std::vector<dataunits::Gather *> aGathers) override;

                int
                Write(io::dataunits::Gather *aGather) override;

            private:
                /// The writer pointer.
                Writer *mpWriter;
                /// The static mapping of writer types to strings.
                static std::unordered_map<std::string, WriterType> mWriterMap;
            };
        } //streams
    } //thoth
} //namespace bs

#endif //BS_IO_STREAMS_SEISMIC_WRITER_HPP
