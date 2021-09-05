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

#include <bs/io/utils/convertors/NumbersConvertor.hpp>

using namespace bs::io::utils::convertors;


char *
NumbersConvertor::ToLittleEndian(char *apSrc, size_t aSize, short aFormat) {
    int rc = 0;
    switch (aFormat) {
        case 1:
            /// Convert IBM float to native floats.
            NumbersConvertor::ToLittleEndian((int *) apSrc, aSize);
            break;
        case 2:
            /// Convert 4 byte two's complement integer to native floats.
            /// @todo To be done
            /// {
            ///  NumbersConvertor::ToLittleEndian((long *) apSrc, aSize);
            /// }
            break;
        case 3:
            /// Convert 2 byte two's complement integer to native floats.
            NumbersConvertor::ToLittleEndian((short *) apSrc, aSize);
            break;
        case 5:
            /// Convert IEEE float to native floats.
            NumbersConvertor::ToLittleEndian((short *) apSrc, aSize);
            break;
        case 8:
            /// Convert 1 byte two's complement integer  to native floats.
            NumbersConvertor::ToLittleEndian((signed char *) apSrc, aSize);
            break;
    }
    return apSrc;
}

short int *
NumbersConvertor::ToLittleEndian(short int *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

short int
NumbersConvertor::ToLittleEndian(short int aSrc) {
    short int tmp = aSrc >> 8 & 0xFF;
    return (aSrc << 8) | (tmp);
}

unsigned short int *
NumbersConvertor::ToLittleEndian(unsigned short int *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

unsigned short int
NumbersConvertor::ToLittleEndian(unsigned short int aSrc) {
    unsigned short int tmp = aSrc >> 8 & 0xFF;
    return (aSrc << 8) | (tmp);
}

int *
NumbersConvertor::ToLittleEndian(int *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

float *
NumbersConvertor::ToLittleEndian(float *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

int
NumbersConvertor::ToLittleEndian(int aSrc) {
    unsigned short int tmp1 = (aSrc >> 16 & 0x0000FFFF);
    unsigned short int tmp2 = (aSrc & 0x0000FFFF);
    tmp2 = NumbersConvertor::ToLittleEndian(tmp2);
    tmp1 = NumbersConvertor::ToLittleEndian(tmp1);

    int aDst = (int) tmp2;
    aDst = aDst << 16;
    aDst = aDst | (int) tmp1;

    return aDst;
}

float
NumbersConvertor::ToLittleEndian(float aSrc) {
    int int_src = *((int *) (&aSrc));
    unsigned short int tmp1 = (int_src >> 16 & 0x0000FFFF);
    unsigned short int tmp2 = (int_src & 0x0000FFFF);
    tmp2 = NumbersConvertor::ToLittleEndian(tmp2);
    tmp1 = NumbersConvertor::ToLittleEndian(tmp1);

    int aDst = (int) tmp2;
    aDst = aDst << 16;
    aDst = aDst | (int) tmp1;
    float dst = *((float *) (&aDst));
    return dst;
}

unsigned int *
NumbersConvertor::ToLittleEndian(unsigned int *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

unsigned int
NumbersConvertor::ToLittleEndian(unsigned int aSrc) {
    unsigned short int tmp1 = (aSrc >> 16 & 0x0000FFFF);
    unsigned short int tmp2 = (aSrc & 0x0000FFFF);
    tmp2 = NumbersConvertor::ToLittleEndian(tmp2);
    tmp1 = NumbersConvertor::ToLittleEndian(tmp1);

    auto aDst = (unsigned int) tmp2;
    aDst = aDst << 16;
    aDst = aDst | (unsigned int) tmp1;

    return aDst;
}

signed char *
NumbersConvertor::ToLittleEndian(signed char *apSrc, size_t aSize) {
    for (int i = 0; i < aSize; ++i) {
        apSrc[i] = ToLittleEndian(apSrc[i]);
    }
    return apSrc;
}

signed char
NumbersConvertor::ToLittleEndian(signed char aSrc) {
    return aSrc;
}
