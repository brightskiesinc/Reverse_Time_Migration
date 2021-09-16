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

#ifndef BS_IO_UTILS_CHECKERS_CHECKER_HPP
#define BS_IO_UTILS_CHECKERS_CHECKER_HPP

namespace bs {
    namespace io {
        namespace utils {
            namespace checkers {

                class Checker {
                public:
                    /**
                     * @brief Return 0 for big endian, 1 for little endian.
                     */
                    static bool
                    IsLittleEndianMachine();

                private:
                    Checker() = default;
                };

            } //namespace checkers
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_CHECKERS_CHECKER_HPP
