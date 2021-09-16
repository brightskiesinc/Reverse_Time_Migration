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

#ifndef BS_IO_CONFIGURATIONS_MAP_KEYS_HPP
#define BS_IO_CONFIGURATIONS_MAP_KEYS_HPP

namespace bs {
    namespace io {
        namespace configuration {

/*
* EXTENSIONS
*/

#define IO_K_EXT_BIN                ".bin"                  /* Binary file format */
#define IO_K_EXT_CSV                ".csv"                  /* CSV file format */
#define IO_K_EXT_IMG                ".png"                  /* Image (i.e. PNG) file format */
#define IO_K_EXT_SGY                ".segy"                 /* SEG-Y file format */
#define IO_K_EXT_JSON               ".json"                 /* JSON file format */
#define IO_K_EXT_SU                 ".su"                   /* SU file format */
#define IO_K_EXT_DSG                ".dsg"                  /* DSG file format */
#define IO_K_EXT_PSG                ".psg"                  /* PSG format */

/*
 * INDEX EXTENSIONS
 */

#define IO_K_EXT_SGY_INDEX          ".bs.io.idx.segy"       /* SEG-Y index file format */


/*
 * CONFIGURATIONS MAP ATTRIBUTES
 */

#define IO_K_PROPERTIES             "properties"
#define IO_K_WRITE_PATH             "write-path"
#define IO_K_READ_PATH              "read-path"
#define IO_K_READ_INDEXED_PATH      "read-indexed-path"
#define IO_K_WRITE_LITTLE_ENDIAN    "write-little-endian"
#define IO_K_FLOAT_FORMAT           "floating-point-format"
#define IO_K_PERCENTILE             "percentile"
#define IO_K_SYNTHETIC_METADATA     "synthetic-meta-data"
#define IO_K_TEXT_HEADERS_ONLY      "text-headers-only"
#define IO_K_TEXT_HEADERS_STORE     "text-headers-store"
#define IO_K_POOL_UUID              "pool-uuid"
#define IO_K_CONTAINER_UUID         "container-uuid"
#define IO_K_INDEX_KEYS             "index-keys"


        } //namespace configurations
    } //namespace io
} //namespace bs

#endif //BS_IO_CONFIGURATIONS_MAP_KEYS_HPP
