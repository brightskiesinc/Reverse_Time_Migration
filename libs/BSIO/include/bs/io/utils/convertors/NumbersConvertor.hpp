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

#ifndef BS_IO_UTILS_CONVERTORS_NUMBERS_CONVERTOR_HPP
#define BS_IO_UTILS_CONVERTORS_NUMBERS_CONVERTOR_HPP

#include <cstring>

namespace bs {
    namespace io {
        namespace utils {
            namespace convertors {

                /**
                 * @brief Numbers convertor that work as a convertor from any representation type
                 * to another, be it byte format, or endianness for example.
                 */
                class NumbersConvertor {
                public:
                    /**
                     * @brief Takes pointer to char array (i.e. byte array) and converts each
                     * element of it to little endian according to the provided format.
                     *
                     * @param apSrc
                     * @param aSize
                     * @param aFormat
                     * @return Formatted array pointer.
                     */
                    static char *
                    ToLittleEndian(char *apSrc, size_t aSize, short aFormat);

                    /**
                     * @brief Takes pointer to short array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted short array pointer
                     */
                    static short *
                    ToLittleEndian(short *apSrc, size_t aSize);

                    /**
                     * @brief Takes short element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted short element.
                     */
                    static short
                    ToLittleEndian(short aSrc);

                    /**
                     * @brief Takes pointer to unsigned short array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted unsigned short array pointer
                     */
                    static unsigned short *
                    ToLittleEndian(unsigned short *apSrc, size_t aSize);

                    /**
                     * @brief Takes unsigned short element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted unsigned short element.
                     */
                    static unsigned short
                    ToLittleEndian(unsigned short aSrc);

                    /**
                     * @brief Takes pointer to int array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted int array pointer
                     */
                    static int *
                    ToLittleEndian(int *apSrc, size_t aSize);

                    /**
                     * @brief Takes int element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted int element.
                     */
                    static int
                    ToLittleEndian(int aSrc);

                    /**
                     * @brief Takes pointer to float array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted float array pointer
                     */
                    static float *
                    ToLittleEndian(float *apSrc, size_t aSize);

                    /**
                     * @brief Takes float element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted float element.
                     */
                    static float
                    ToLittleEndian(float aSrc);

                    /**
                     * @brief Takes pointer to int array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted int array pointer
                     */
                    static unsigned int *
                    ToLittleEndian(unsigned int *apSrc, size_t aSize);

                    /**
                     * @brief Takes unsigned int element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted unsigned int element.
                     */
                    static unsigned int
                    ToLittleEndian(unsigned int aSrc);

                    /**
                     * @brief Takes pointer to signed char array and converts each
                     * element of it to little endian.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted signed char array pointer
                     */
                    static signed char *
                    ToLittleEndian(signed char *apSrc, size_t aSize);

                    /**
                     * @brief Takes signed char element and converts it to little endian.
                     *
                     * @param[in] aSrc
                     * @return Converted signed char element.
                     */
                    static signed char
                    ToLittleEndian(signed char aSrc);

                private:
                    /**
                     * @brief Private constructor for preventing objects creation.
                     */
                    NumbersConvertor() = default;
                };
            } //namespace convertors
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_CONVERTORS_NUMBERS_CONVERTOR_HPP
