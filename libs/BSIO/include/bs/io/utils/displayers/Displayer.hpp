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

#ifndef BS_IO_UTILS_DISPLAYERS_DISPLAYER_H
#define BS_IO_UTILS_DISPLAYERS_DISPLAYER_H

namespace bs {
    namespace io {
        namespace utils {
            namespace displayers {

                class Displayer {
                public:
                    /**
                     * @brief Prints the text header extracted from the given SEG-Y file in the
                     * SEG-Y community format.
                     *
                     * @param apTextHeader
                     */
                    static void
                    PrintTextHeader(unsigned char *apTextHeader);

                private:
                    Displayer() = default;
                };

            } //namespace displayers
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_DISPLAYERS_DISPLAYER_H
