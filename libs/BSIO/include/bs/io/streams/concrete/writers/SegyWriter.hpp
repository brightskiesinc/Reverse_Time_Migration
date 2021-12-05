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

#ifndef BS_IO_STREAMS_SEGY_WRITER_HPP
#define BS_IO_STREAMS_SEGY_WRITER_HPP

#include <bs/io/streams/primitive/Writer.hpp>
#include <bs/io/streams/helpers/OutStreamHelper.hpp>

namespace bs {
    namespace io {
        namespace streams {
            /**
             * @brief SEG-Y file format writer.
             */
            class SegyWriter : public Writer {
            public:
                /**
                 * @brief Constructor
                 */
                explicit SegyWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Destructor
                 */
                ~SegyWriter() override;

                /**
                 * @brief
                 * Acquires the component configurations from a given configurations map.
                 *
                 * @param[in] apConfigurationMap
                 * The configurations map to be used.
                 */
                void
                AcquireConfiguration() override;

                /**
                 * @brief Returns the file format extension of the current stream.
                 */
                std::string GetExtension() override;

                /**
                 * @brief
                 * Initializes the writer with the appropriate settings applied
                 * to the writer, and any preparations needed are done.
                 * Should be called once at the start.
                 *
                 * @param[in] aFilePath
                 * The path to be used, either directly or as a seed, for writing.
                 */
                int
                Initialize(std::string &aFilePath) override;

                /**
                 * @brief
                 * Does any final updates needed for consistency of the writer.
                 * Release all resources and close everything.
                 * Should be initialized again afterwards to be able to reuse it again.
                 */
                int
                Finalize() override;

                /**
                 * @brief
                 * Writes a group of gathers to the output stream of the writer.
                 *
                 * @param[in] aGathers
                 * List of gathers to be written.
                 *
                 * @return
                 * An error flag, if 0 that means operation was successful, otherwise indicate an error.
                 */
                int
                Write(std::vector<dataunits::Gather *> aGathers) override;

                /**
                 * @brief
                 * Writes a gather to the output stream of the writer.
                 *
                 * @param[in] aGather
                 * The gather to be written.
                 *
                 * @return
                 * An error flag, if 0 that means operation was successful, otherwise indicate an error.
                 */
                int
                Write(io::dataunits::Gather *aGather) override;

            private:
                /// File path.
                std::string mFilePath;
                /// File stream helper.
                streams::helpers::OutStreamHelper *mOutStreamHelpers;
                /// Boolean to check whether the target file should be written
                /// in little endian or not.
                bool mWriteLittleEndian;
                /// Check if the binary header is written.
                bool mBinaryHeaderWritten;
                /// The format to write the floating point data in.
                uint16_t mFormat;
            };

        } //streams
    } //thoth
} //namespace bs

#endif //BS_IO_STREAMS_SEGY_WRITER_HPP
