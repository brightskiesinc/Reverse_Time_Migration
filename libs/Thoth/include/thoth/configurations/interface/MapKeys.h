//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_CONFIGURATIONS_MAP_KEYS_HPP
#define THOTH_CONFIGURATIONS_MAP_KEYS_HPP

namespace thoth {
    namespace configuration {

/*
 * EXTENSIONS
 */

#define IO_K_EXT_BIN                ".bin"          /* Binary file format */
#define IO_K_EXT_CSV                ".csv"          /* CSV file format */
#define IO_K_EXT_IMG                ".png"          /* Image (i.e. PNG) file format */
#define IO_K_EXT_SGY                ".segy"         /* SEG-Y file format */
#define IO_K_EXT_TXT                ".json"         /* Text (i.e. JSON) file format */
#define IO_K_EXT_SU                 ".su"           /* SU file format */

/*
 * INDEX EXTENSIONS
 */

#define IO_K_EXT_SGY_INDEX          ".idxsegy"      /* SEG-Y index file format */


/*
 * CONFIGURATIONS MAP ATTRIBUTES
 */

#define IO_K_PROPERTIES             "properties"
#define IO_K_WRITE_PATH             "write-path"
#define IO_K_READ_PATH              "read-path"
#define IO_K_READ_INDEXED_PATH      "read-indexed-path"
#define IO_K_WRITE_LITTLE_ENDIAN    "write-little-endian"
#define IO_K_SYNTHETIC_METADATA     "synthetic-meta-data"
#define IO_K_TEXT_HEADERS_ONLY      "text-headers-only"
#define IO_K_TEXT_HEADERS_STORE     "text-headers-store"

    } //namespace configurations
} //namespace operations

#endif //THOTH_CONFIGURATIONS_MAP_KEYS_HPP
