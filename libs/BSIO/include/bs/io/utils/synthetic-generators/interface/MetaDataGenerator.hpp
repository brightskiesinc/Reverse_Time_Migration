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

#ifndef BS_IO_UTILS_SYNTHETIC_GENERATORS_META_DATA_GENERATOR_HPP
#define BS_IO_UTILS_SYNTHETIC_GENERATORS_META_DATA_GENERATOR_HPP

#include <vector>

#include <bs/io/data-units/concrete/Gather.hpp>

#define IO_K_SYN_CELL_SAMP          "cell-dimension"                /* string. */
#define IO_K_SYN_CELL_SAMP_X        "dx"                            /* string. */
#define IO_K_SYN_CELL_SAMP_Y        "dy"                            /* string. */
#define IO_K_SYN_CELL_SAMP_Z        "dz"                            /* string. */
#define IO_K_SYN_GRIZ_SIZE          "grid-size"                     /* string. */
#define IO_K_SYN_NX                 "nx"                            /* string. */
#define IO_K_SYN_NY                 "ny"                            /* string. */
#define IO_K_SYN_NZ                 "nz"                            /* string. */
#define IO_K_SYN_ORIGIN             "origin-coordinates"            /* string. */
#define IO_K_SYN_X                  "x"                             /* string. */
#define IO_K_SYN_Y                  "y"                             /* string. */
#define IO_K_SYN_Z                  "z"                             /* string. */
#define IO_K_SYN_X_INDEX            "x-index"                       /* string. */
#define IO_K_SYN_Y_INDEX            "y-index"                       /* string. */
#define IO_K_SYN_Z_INDEX            "z-index"                       /* string. */
#define IO_K_SYN_X_OFFSET           "x-offset"                      /* string. */
#define IO_K_SYN_Y_OFFSET           "y-offset"                      /* string. */
#define IO_K_SYN_Z_OFFSET           "z-offset"                      /* string. */
#define IO_K_SYN_SOURCE             "source"                        /* string. */
#define IO_K_SYN_START              "start"                         /* string. */
#define IO_K_SYN_END                "end"                           /* string. */
#define IO_K_SYN_INCREMENT          "increment"                     /* string. */
#define IO_K_SYN_NUMBER             "number"                        /* string. */
#define IO_K_SYN_SHOTS              "shots"                         /* string. */
#define IO_K_SYN_DT                 "time-sampling"                 /* string. */
#define IO_K_SYN_NS                 "sample-number"                 /* string. */
#define IO_K_SYN_REC_REL            "source-relative-receivers"     /* string. */

namespace bs {
    namespace io {
        namespace generators {
            /**
             * @brief
             * Interface for all meta-data generators to generate traces with correct meta-data.
             */
            class MetaDataGenerator {
            public:
                /**
                 * @brief
                 * Default constructor.
                 */
                MetaDataGenerator() = default;

                /**
                 * @brief
                 * Default destructor.
                 */
                virtual ~MetaDataGenerator() = default;

                /**
                 * @brief
                 * Set the generation keys used for the gather generation of meta-data.
                 *
                 * @param[in] aHeaderKeys
                 * The header keys to use for generation.
                 */
                virtual void
                SetGenerationKey(std::vector<io::dataunits::TraceHeaderKey> &aHeaderKeys) = 0;

                /**
                 * @brief
                 * Get all the possible gathers to be generated.
                 *
                 * @return
                 * Vector of gathers.
                 */
                virtual std::vector<io::dataunits::Gather *>
                GetAllTraces() = 0;

                /**
                 * @brief
                 * Get all the traces following a given subset of values.
                 *
                 * @param[in] aHeaderValues
                 * The header values to generate for.
                 *
                 * @return
                 * The gathers generated.
                 */
                virtual std::vector<io::dataunits::Gather *>
                GetTraces(std::vector<std::vector<std::string>> aHeaderValues) = 0;

                /**
                 * @brief
                 * Return all the possible identifiers based on the gather key provided.
                 *
                 * @return
                 * List of identifiers.
                 */
                virtual std::vector<std::vector<std::string>>
                GetGatherUniqueValues() = 0;

                /**
                 * @brief
                 * Get the total number of gathers that can be generated.
                 *
                 * @return
                 * The total number of gathers.
                 */
                virtual uint
                GetGatherNumber() = 0;
            };
        }
    }
}

#endif //BS_IO_UTILS_SYNTHETIC_GENERATORS_META_DATA_GENERATOR_HPP
