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

#ifndef BS_IO_STREAMS_HELPERS_IN_FILE_HELPER_HPP
#define BS_IO_STREAMS_HELPERS_IN_FILE_HELPER_HPP

#include <fstream>
#include <cstring>

#include <bs/io/data-units/concrete/Trace.hpp>
#include <bs/io/data-units/concrete/Gather.hpp>
#include <bs/io/lookups/tables/TextHeaderLookup.hpp>
#include <bs/io/lookups/tables/TraceHeaderLookup.hpp>
#include <bs/io/lookups/tables/BinaryHeaderLookup.hpp>

namespace bs {
    namespace io {
        namespace streams {
            namespace helpers {

                /**
                 * @brief File helper to take any stream and helps manipulate or get any regarded
                 * meta data from it.
                 */
                class InStreamHelper {
                public:
                    /**
                     * @brief Explicit constructor.
                     * @param[in] aFilePath
                     */
                    explicit InStreamHelper(std::string &aFilePath);

                    /**
                     * @brief Destructor.
                     */
                    ~InStreamHelper();

                    /**
                     * @brief Opens stream for the file regarded.
                     * @return File size.
                     */
                    size_t
                    Open();

                    /**
                     * @brief Release all resources and close everything.
                     */
                    int
                    Close();

                    /**
                     * @brief Gets file size for a given file path.
                     * @return size_t
                     */
                    size_t
                    GetFileSize();

                    /**
                     * @brief Get current position for a given file path.
                     * @return size_t
                     */
                    size_t
                    GetCurrentPosition();

                    /**
                     * @brief Reads a block of bytes from current stream.
                     *
                     * @param[in] aStartPosition
                     * @param[in] aBlockSize
                     * @return unsigned char *
                     */
                    unsigned char *
                    ReadBytesBlock(size_t aStartPosition, size_t aBlockSize);

                    /**
                     * @brief Reads a text header, be it the original text header or the extended text header
                     * from a given SEG-Y file, by passing the start byte position of it.
                     *
                     * @param[in] aStartPosition
                     * @return unsigned char *
                     */
                    unsigned char *
                    ReadTextHeader(size_t aStartPosition);

                    /**
                     * @brief Reads a binary header from a given SEG-Y file, by passing the start byte position of it.
                     * @param[in] aStartPosition
                     * @return BinaryHeaderLookup
                     */
                    lookups::BinaryHeaderLookup
                    ReadBinaryHeader(size_t aStartPosition);

                    /**
                     * @brief Reads a trace header from a given SEG-Y file, by passing the start byte position of it.
                     * @param[in] aStartPosition
                     * @return TraceHeaderLookup
                     */
                    lookups::TraceHeaderLookup
                    ReadTraceHeader(size_t aStartPosition);

                    /**
                     * @brief Reads a single trace from a given SEG-Y file, by passing the start byte position
                     * of the desired trace, the trace header for allocation purposes and the binary header for
                     * data formats purposes.
                     *
                     * @param[in] aStartPosition
                     * @param[in] aTraceHeaderLookup
                     * @param[in] aBinaryHeaderLookup
                     * @return Trace *
                     */
                    dataunits::Trace *
                    ReadFormattedTraceData(size_t aStartPosition,
                                           io::lookups::TraceHeaderLookup &aTraceHeaderLookup,
                                           io::lookups::BinaryHeaderLookup &aBinaryHeaderLookup);

                public:
                    /**
                     * @brief Gets trace data size given its header and binary header to
                     * determine whether to take NS or BHS and deduce data format.
                     *
                     * @param[in] aTraceHeaderLookup
                     * @param[in] aBinaryHeaderLookup
                     * @return
                     */
                    static size_t
                    GetTraceDataSize(const io::lookups::TraceHeaderLookup &aTraceHeaderLookup,
                                     const io::lookups::BinaryHeaderLookup &aBinaryHeaderLookup);

                    /**
                     * @brief Gets samples number in a trace given its header and binary header to
                     * determine whether to take NS or BHS.
                     *
                     * @param[in] aTraceHeaderLookup
                     * @param[in] aBinaryHeaderLookup
                     * @return
                     */
                    static size_t
                    GetSamplesNumber(const lookups::TraceHeaderLookup &aTraceHeaderLookup,
                                     const lookups::BinaryHeaderLookup &aBinaryHeaderLookup);

                private:
                    /// File path.
                    std::string mFilePath;
                    /// File input stream.
                    std::ifstream mInStream;
                    /// File size.
                    size_t mFileSize;
                };

            } //namespace helpers
        } //namespace streams
    } //namespace io
} //namespace bs

#endif //BS_IO_STREAMS_HELPERS_IN_FILE_HELPER_HPP
