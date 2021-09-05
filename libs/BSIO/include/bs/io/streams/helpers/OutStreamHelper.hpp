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

#ifndef BS_IO_STREAMS_HELPERS_OUT_FILE_HELPER_HPP
#define BS_IO_STREAMS_HELPERS_OUT_FILE_HELPER_HPP

#include <fstream>

namespace bs {
    namespace io {
        namespace streams {
            namespace helpers {

                /**
                 * @brief File helper to take any stream and helps manipulate or write any regarded
                 * data to it.
                 */
                class OutStreamHelper {
                public:
                    /**
                     * @brief Explicit constructor.
                     * @param[in] aFilePath
                     */
                    explicit OutStreamHelper(std::string &aFilePath, bool aModify = false);

                    /**
                     * @brief Destructor.
                     */
                    ~OutStreamHelper();

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
                     * @brief Writes a block of bytes in current stream.
                     *
                     * @param[in] aData
                     * @param[in] aStartPosition
                     * @param[in] aBlockSize
                     * @return Status flag.
                     */
                    size_t
                    WriteBytesBlock(const char *aData, size_t aBlockSize);

                    /**
                     * @brief
                     * Writes a block of bytes in current stream.
                     *
                     * @param[in] aData
                     * The data to write into the file.
                     *
                     * @param[in] aBlockSize
                     * The size of the data to write.
                     *
                     * @param[in] aStartingPosition
                     * The starting position in the file to write to.
                     *
                     * @return Status flag.
                     * The appropriate exit code.
                     */
                    size_t
                    WriteBytesBlock(const char *aData, size_t aBlockSize,
                                    size_t aStartingPosition);

                private:
                    /// File path.
                    std::string mFilePath;
                    /// File output stream.
                    std::ofstream mOutStream;
                    /// File size.
                    size_t mFileSize;
                    /// Modify file if true or overwrite.
                    bool mModify;
                };

            } //namespace helpers
        } //namespace streams
    } //namespace io
} //namespace bs

#endif //BS_IO_STREAMS_HELPERS_OUT_FILE_HELPER_HPP
