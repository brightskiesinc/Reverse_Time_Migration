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

#ifndef BS_IO_RANGE_BYTE_RANGE_HPP
#define BS_IO_RANGE_BYTE_RANGE_HPP

namespace bs {
    namespace io {
        namespace utils {
            namespace range {

                class EntryRange {
                public:
                    enum Type : char {
                        SHORT = 'h',
                        USHORT = 'u',
                        INT = 'i',
                        UINT = 'p',
                        LONG = 'l',
                        ULONG = 'v',
                        FLOAT = 'f',
                        DOUBLE = 'd',
                        CHAR = 's'
                    };

                public:
                    EntryRange() = default;

                    EntryRange(char aOffsetLength, int aStartPosition, int aConsecutiveBlocks = 1) {
                        this->mOffsetLength = aOffsetLength;
                        this->mStartPosition = aStartPosition;
                        this->mConsecutiveBlocks = aConsecutiveBlocks;
                    }

                    EntryRange(const EntryRange &aBR) {
                        mOffsetLength = aBR.mOffsetLength;
                        mStartPosition = aBR.mStartPosition;
                        mConsecutiveBlocks = aBR.mConsecutiveBlocks;
                    }

                    inline int GetStartPosition() const { return this->mStartPosition; }

                    inline char GetOffsetLength() const { return this->mOffsetLength; }

                    inline int GetConsecutiveBlocks() const { return this->mConsecutiveBlocks; }

                private:
                    char mOffsetLength;
                    int mStartPosition;
                    int mConsecutiveBlocks;
                };

            } //namespace ranges
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_RANGE_BYTE_RANGE_HPP
