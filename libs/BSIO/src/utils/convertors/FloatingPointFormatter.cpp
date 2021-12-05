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

#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/utils/convertors/FloatingPointFormatter.hpp>
#include <bs/io/utils/checkers/Checker.hpp>

using namespace bs::base::exceptions;
using namespace bs::io::utils::convertors;
using namespace bs::io::utils::checkers;


int
FloatingPointFormatter::GetFloatArrayRealSize(unsigned short aSamplesNumber, unsigned short aFormatCode) {
    int size;
    if (aFormatCode == 1 || aFormatCode == 2 || aFormatCode == 4 || aFormatCode == 5) {
        size = 4 * aSamplesNumber;
    } else if (aFormatCode == 3) {
        size = 2 * aSamplesNumber;
    } else if (aFormatCode == 8) {
        size = aSamplesNumber;
    } else {
        throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
    }
    return size;
}

int
FloatingPointFormatter::Format(const char *apSrc, char *apDest,
                               size_t aSrcSize, size_t aSamplesNumber,
                               short aFormat, bool aFromFormat) {
    int rc;
    if (aFromFormat) {
        switch (aFormat) {
            case 1:
                /// Convert IBM float to native floats.
                rc = FloatingPointFormatter::FromIBM(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            case 2:
                /// Convert 4 byte two's complement integer to native floats.
                rc = FloatingPointFormatter::FromLong(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            case 3:
                /// Convert 2 byte two's complement integer to native floats.
                rc = FloatingPointFormatter::FromShort(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            case 5:
                /// Convert IEEE float to native floats.
                rc = FloatingPointFormatter::FromIEEE(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            case 8:
                /// Convert 1 byte two's complement integer  to native floats.
                rc = FloatingPointFormatter::FromChar(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            default:
                throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
        }
    } else {
        switch (aFormat) {
            case 1:
                /// Convert IBM float to native floats.
                rc = FloatingPointFormatter::ToIBM(apSrc, apDest, aSrcSize, aSamplesNumber);
                break;
            default:
                throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
        }
    }
    return rc;
}

int
FloatingPointFormatter::FromIBM(const char *apSrc, char *apDest,
                                size_t aSrcSize, size_t aSamplesNumber) {
    bool is_little_endian = Checker::IsLittleEndianMachine();
    size_t size_entry = sizeof(float);
    size_t size = aSrcSize / size_entry;
    unsigned int fconv, fmant, t;
    unsigned int offset = 0;
    for (int i = 0; i < size; ++i) {
        offset = i * size_entry;
        memcpy(&fconv, apSrc + offset, sizeof(unsigned int));
        if (is_little_endian) {
            fconv = (fconv << 24) | ((fconv >> 24) & 0xff) | ((fconv & 0xff00) << 8) |
                    ((fconv & 0xff0000) >> 8);
        }
        if (fconv) {
            fmant = 0x00ffffff & fconv;
            t = (int) ((0x7f000000 & fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) {
                --t;
                fmant <<= 1;
            }
            if (t > 254) {
                fconv = (0x80000000 & fconv) | 0x7f7fffff;
            } else if (t <= 0) {
                fconv = 0;
            } else {
                fconv = (0x80000000 & fconv) | (t << 23) | (0x007fffff & fmant);
            }
        }
        memcpy(apDest + offset, &fconv, sizeof(float));
    }
    return 1;
}

int
FloatingPointFormatter::ToIBM(const char *apSrc, char *apDest,
                              size_t aSrcSize, size_t aSamplesNumber) {
    bool is_little_endian = Checker::IsLittleEndianMachine();
    size_t size_entry = sizeof(float);
    size_t size = aSrcSize / size_entry;
    unsigned int fconv, fmant, t;
    unsigned int offset = 0;
    unsigned fr; /* fraction */
    int exp;     /* exponent */
    int sgn;     /* sign */
    for (int i = 0; i < size; ++i) {
        offset = i * size_entry;
        memcpy(&fconv, apSrc + offset, sizeof(unsigned int));
        if (fconv) {
            fr = fconv;             /* pick up value */
            sgn = fr >> 31;         /* save sign */
            fr <<= 1;               /* shift sign out */
            exp = fr >> 24;         /* save exponent */
            fr <<= 8;               /* shift exponent out */

            if (exp == 255) { /* infinity (or NAN) - map to largest */
                fr = 0xffffff00;
                exp = 0x7f;
                goto done;
            } else if (exp > 0) /* add assumed digit */
                fr = (fr >> 1) | 0x80000000;
            else if (fr == 0) /* short-circuit for zero */
                goto done;

            /* adjust exponent from base 2 offset 127 radix point after first digit
            to base 16 offset 64 radix point before first digit */
            exp += 130;
            fr >>= -exp & 3;
            exp = (exp + 3) >> 2;

            /* (re)normalize */
            while (fr < 0x10000000) { /* never executed for normalized input */
                --exp;
                fr <<= 4;
            }

            done:
            /* put the pieces back together and return it */
            fr = (fr >> 8) | (exp << 24) | (sgn << 31);
            fconv = fr;
        }
        // Swap endian after transformation.
        if (is_little_endian) {
            fconv = (fconv << 24) | ((fconv >> 24) & 0xff) | ((fconv & 0xff00) << 8) |
                    ((fconv & 0xff0000) >> 8);
        }
        memcpy(apDest + offset, &fconv, sizeof(float));
    }
    return 1;
}

int
FloatingPointFormatter::FromLong(const char *apSrc, char *apDest,
                                 size_t aSize, size_t aSamplesNumber) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
    return 1;
}

int
FloatingPointFormatter::FromShort(const char *apSrc, char *apDest,
                                  size_t aSize, size_t aSamplesNumber) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
    return 1;
}

int
FloatingPointFormatter::FromIEEE(const char *apSrc, char *apDest,
                                 size_t aSize, size_t aSamplesNumber) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
    return 1;
}

int
FloatingPointFormatter::FromChar(const char *apSrc, char *apDest,
                                 size_t aSize, size_t aSamplesNumber) {
    /// @todo To be removed
    /// {
    throw NOT_IMPLEMENTED_EXCEPTION();
    /// }
    return 1;
}
