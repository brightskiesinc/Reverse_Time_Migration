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

#include <bs/io/utils/convertors/StringsConvertor.hpp>

/// @todo To be removed
/// {
#include <bs/base/exceptions/Exceptions.hpp>

using namespace bs::base::exceptions;
/// }

using namespace bs::io::utils::convertors;


short
StringsConvertor::ToShort(const std::string &aStr) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
}

unsigned short
StringsConvertor::ToUShort(const std::string &aStr) {
    return std::stoul(aStr);
}

int
StringsConvertor::ToInt(const std::string &aStr) {
    return std::stoi(aStr);
}

unsigned int
StringsConvertor::ToUInt(const std::string &aStr) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
}

long
StringsConvertor::ToLong(const std::string &aStr) {
    return std::stol(aStr);
}

unsigned long
StringsConvertor::ToULong(const std::string &aStr) {
    return std::stoul(aStr);
}

unsigned char *
StringsConvertor::E2A(unsigned char *apSrc, size_t aSize) {
    for (size_t i = 0; i < aSize; i++) {
        apSrc[i] = E2A(apSrc[i]);
    }
    return apSrc;
}

unsigned char
StringsConvertor::E2A(unsigned char aSrc) {
    return StringsConvertor::mE2ATable[(int) (aSrc)];
}

unsigned char StringsConvertor::mE2ATable[256] = {
        0, 1, 2, 3, 236, 9, 202, 127, 226, 210, 211, 11, 12, 13, 14, 15, 16, 17, 18, 19, 239,
        197, 8, 203, 24, 25, 220, 216, 28, 29, 30, 31, 183, 184, 185, 187, 196, 10, 23, 27,
        204, 205, 207, 208, 209, 5, 6, 7, 217, 218, 22, 221, 222, 223, 224, 4, 227, 229, 233,
        235, 20, 21, 158, 26, 32, 201, 131, 132, 133, 160, 242, 134, 135, 164, 213, 046, 60,
        40, 43, 179, 38, 130, 136, 137, 138, 161, 140, 139, 141, 225, 33, 36, 42, 41, 59, 94,
        45, 47, 178, 142, 180, 181, 182, 143, 128, 165, 124, 44, 37, 95, 62, 63, 186, 144, 188,
        189, 190, 243, 192, 193, 194, 96, 58, 35, 64, 39, 61, 34, 195, 97, 98, 99, 100, 101, 102,
        103, 104, 105, 174, 175, 198, 199, 200, 241, 248, 106, 107, 108, 109, 110, 111, 112, 113,
        114, 166, 167, 145, 206, 146, 15, 230, 126, 115, 116, 117, 118, 119, 120, 121, 122, 173,
        168, 212, 91, 214, 215, 155, 156, 157, 250, 159, 21, 20, 172, 171, 252, 170, 254, 228,
        93, 191, 231, 123, 65, 66, 67, 68, 69, 70, 71, 72, 73, 232, 147, 148, 149, 162, 237, 125, 74, 75,
        76, 77, 78, 79, 80, 81, 82, 238, 150, 129, 151, 163, 152, 92, 240, 83, 84, 85, 86, 87, 88, 89, 90,
        253, 245, 153, 247, 246, 249, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 219, 251, 154, 244, 234, 255
};
