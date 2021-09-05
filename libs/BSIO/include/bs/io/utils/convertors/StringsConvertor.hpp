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

#ifndef BS_IO_UTILS_CONVERTORS_STRINGS_CONVERTOR_HPP
#define BS_IO_UTILS_CONVERTORS_STRINGS_CONVERTOR_HPP

#include <string>

namespace bs {
    namespace io {
        namespace utils {
            namespace convertors {

                /**
                 * @brief Strings convertor that work as a convertor from any representation type
                 * to another, be it byte format, or endianness for example.
                 */
                class StringsConvertor {
                public:
                    /**
                     * @brief Takes a string and converts its content to short.
                     *
                     * @param[in] aStr
                     * @return Converted string to short.
                     */
                    static short
                    ToShort(const std::string &aStr);

                    /**
                     * @brief Takes a string and converts its content to unsigned short.
                     *
                     * @param[in] aStr
                     * @return Converted string to unsigned short.
                     */
                    static unsigned short
                    ToUShort(const std::string &aStr);

                    /**
                     * @brief Takes a string and converts its content to int.
                     *
                     * @param[in] aStr
                     * @return Converted string to int.
                     */
                    static int
                    ToInt(const std::string &aStr);

                    /**
                     * @brief Takes a string and converts its content to unsigned int.
                     *
                     * @param[in] aStr
                     * @return Converted string to unsigned int.
                     */
                    static unsigned int
                    ToUInt(const std::string &aStr);

                    /**
                     * @brief Takes a string and converts its content to long.
                     *
                     * @param[in] aStr
                     * @return Converted string to long.
                     */
                    static long
                    ToLong(const std::string &aStr);

                    /**
                     * @brief Takes a string and converts its content to unsigned long.
                     *
                     * @param[in] aStr
                     * @return Converted string to unsigned long.
                     */
                    static unsigned long
                    ToULong(const std::string &aStr);

                    /**
                     * @brief Takes pointer to unsigned char array and converts each
                     * element from EBCDIC to ASCII.
                     *
                     * @param[in] apSrc
                     * @param[in] aSize
                     * @return Converted unsigned char array pointer
                     */
                    static unsigned char *
                    E2A(unsigned char *apSrc, size_t aSize);

                    /**
                     * @brief Takes char element and converts it from EBCDIC to ASCII.
                     *
                     * @param[in] aSrc
                     * @return Converted char element.
                     */
                    static unsigned char
                    E2A(unsigned char aSrc);

                private:
                    /**
                     * @brief Private constructor for preventing objects creation.
                     */
                    StringsConvertor() = default;

                private:
                    static unsigned char mE2ATable[256];
                };
            } //namespace convertors
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_CONVERTORS_STRINGS_CONVERTOR_HPP
