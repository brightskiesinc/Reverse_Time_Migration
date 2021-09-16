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

#ifndef BS_IO_STREAMS_WRITER_HPP
#define BS_IO_STREAMS_WRITER_HPP

#include <bs/io/streams/interface/Stream.hpp>
#include <bs/base/configurations/interface/Configurable.hpp>
#include <bs/io/data-units/concrete/Trace.hpp>
#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>

#include <unordered_map>
#include <vector>

namespace bs {
    namespace io {
        namespace streams {
            /**
             * @brief
             * Writer interface for seismic data.
             * This should be configurable to allow each reader to extract its unique additional properties.
             * A simple flow should be like the following:
             * Writer.AcquireConfiguration(...);
             * Writer.Initialize(...);
             * Do all other operations afterwards.
             */
            class Writer : public Stream {
            public:
                /**
                 * @brief
                 * Virtual destructor to allow correct destruction of concrete classes.
                 */
                virtual ~Writer() = default;

                /**
                 * @brief
                 * Initializes the writer with the appropriate settings applied
                 * to the writer, and any preparations needed are done.
                 * Should be called once at the start.
                 *
                 * @param[in] aFilePath
                 * The path to be used, either directly or as a seed, for writing.
                 */
                virtual int
                Initialize(std::string &aFilePath) = 0;

                /**
                 * @brief
                 * Does any final updates needed for consistency of the writer.
                 * Release all resources and close everything.
                 * Should be initialized again afterwards to be able to reuse it again.
                 */
                virtual int
                Finalize() = 0;

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
                virtual int
                Write(std::vector<dataunits::Gather *> aGathers) = 0;

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
                virtual int
                Write(io::dataunits::Gather *aGather) = 0;
            };

        } //namespace streams
    } //namespace io
} //namespace bs

#endif //BS_IO_STREAMS_WRITER_HPP
