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

#ifndef BS_IO_UTILS_CONVERTORS_FLOATING_POINT_FORMATTER_HPP
#define BS_IO_UTILS_CONVERTORS_FLOATING_POINT_FORMATTER_HPP

#include <cstring>

namespace bs {
    namespace io {
        namespace utils {
            namespace convertors {

                /**
                 * @brief Floating point formatter that work as a convertor from any floating type
                 * representation to another.
                 */
                class FloatingPointFormatter {
                public:
                    /**
                     * @brief Trace size in bytes given the NS from trace header and format code from
                     * binary header.
                     *
                     * @param[int] aSamplesNumber
                     * @param[int] aFormatCode
                     * @return Trace Size
                     */
                    static int
                    GetFloatArrayRealSize(unsigned short int aSamplesNumber, unsigned short int aFormatCode);

                    static int
                    Format(const char *apSrc, char *apDest,
                           size_t aSrcSize, size_t aSamplesNumber, short aFormat,
                           bool aFromFormat);

                private:
                    FloatingPointFormatter() = default;

                private:
                    /**
                     * @brief Converts between 32 bit IBM floating numbers to native floating number.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSrcSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    FromIBM(const char *apSrc, char *apDest, size_t aSrcSize, size_t aSamplesNumber);

                    /**
                     * @brief Converts between native floating number to 32 bit IBM floating numbers.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSrcSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    ToIBM(const char *apSrc, char *apDest, size_t aSrcSize, size_t aSamplesNumber);

                    /**
                     * @brief Converts between 64 bit IBM floating numbers to native floating number.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    FromLong(const char *apSrc, char *apDest, size_t aSize, size_t aSamplesNumber);

                    /**
                     * @brief Converts between 8 bit IBM floating numbers to native floating number.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    FromShort(const char *apSrc, char *apDest, size_t aSize, size_t aSamplesNumber);

                    /**
                     * @brief Converts between 8 bit IEEE and IEEE floating numbers.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    FromIEEE(const char *apSrc, char *apDest, size_t aSize, size_t aSamplesNumber);

                    /**
                     * @brief Converts between 8 bit character and IEEE floating numbers.
                     * @param[in] apSrc
                     * @param[out] apDest
                     * @param[in] aSize
                     * @return Flag. 1 if success and 0 if conversion failed.
                     */
                    static int
                    FromChar(const char *apSrc, char *apDest, size_t aSize, size_t aSamplesNumber);
                };
            } //namespace convertors
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_CONVERTORS_FLOATING_POINT_FORMATTER_HPP
