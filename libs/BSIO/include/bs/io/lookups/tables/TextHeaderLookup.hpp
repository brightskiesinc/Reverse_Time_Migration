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

#ifndef BS_IO_LOOKUPS_TEXT_HEADERS_LOOKUP_HPP
#define BS_IO_LOOKUPS_TEXT_HEADERS_LOOKUP_HPP

namespace bs {
    namespace io {
        namespace lookups {

#define IO_SIZE_TEXT_HEADER             3200    /* Size of the text header in SEG-Y files */
#define IO_SIZE_EXT_TEXT_HEADER         3200    /* Size of the extended (i.e. If available) text header in SEG-Y files */
#define IO_POS_S_TEXT_HEADER            0       /* Start position of text header in SEG-Y and SU files */
#define IO_POS_E_TEXT_HEADER            3200    /* End position of text header in SEG-Y and SU files */
#define IO_POS_S_EXT_TEXT_HEADER        3600    /* Start position of extended text header in SEG-Y and SU files */
#define IO_POS_E_EXT_TEXT_HEADER        6800    /* End position of extended text header in SEG-Y and SU files */

        } //namespace lookups
    } //namespace io
} //namespace bs

#endif //BS_IO_LOOKUPS_TEXT_HEADERS_LOOKUP_HPP
