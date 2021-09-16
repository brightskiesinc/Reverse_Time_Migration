/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer.
 *
 * BS Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_TIMER_LIB_VARS_DEFINITIONS_HPP
#define BS_TIMER_LIB_VARS_DEFINITIONS_HPP

namespace bs {
    namespace timer {
        namespace libvars {
            namespace definitions {

#define BS_TIMER_EXT                     ".bs.timer"             /* Extension used for flushed files.*/

#define BS_TIMER_TU_MILLI                1e-3                    /* The conversion unit used for converting seconds to milliseconds. */
#define BS_TIMER_TU_MICRO                1e-6                    /* The conversion unit used for converting seconds to microseconds. */
#define BS_TIMER_TU_NANO                 1e-9                    /* The conversion unit used for converting seconds to nanoseconds. */
#define BS_TIMER_TU_STR_MILLI            "msec"
#define BS_TIMER_TU_STR_MICRO            "μsec"
#define BS_TIMER_TU_STR_NANO             "nsec"
#define BS_TIMER_TU_STR_SEC              "sec"


#define BS_TIMER_K_TIME_UNIT             "precision"             /* Key for time unit. */
#define BS_TIMER_K_PROPERTIES            "properties"            /* Key for properties. */
#define BS_TIMER_K_MAX_RUNTIME           "max_runtime"           /* Key for maximum runtime. */
#define BS_TIMER_K_MAX_BANDWIDTH         "max_bandwidth"         /* Key for maximum bandwidth. */
#define BS_TIMER_K_MAX_THROUGHPUT        "max_throughput"        /* Key for maximum throughput. */
#define BS_TIMER_K_MAX_GFLOPS            "max_gflops"            /* Key for maximum GFLOPS. */
#define BS_TIMER_K_MIN_RUNTIME           "min_runtime"           /* Key for minimum runtime. */
#define BS_TIMER_K_MIN_BANDWIDTH         "min_bandwidth"         /* Key for minimum bandwidth. */
#define BS_TIMER_K_MIN_THROUGHPUT        "min_throughput"        /* Key for minimum throughput. */
#define BS_TIMER_K_MIN_GFLOPS            "min_gflops"            /* Key for minimum GFLOPS. */
#define BS_TIMER_K_AVERAGE_RUNTIME       "average_runtime"       /* Key for average runtime. */
#define BS_TIMER_K_AVERAGE_BANDWIDTH     "average_bandwidth"     /* Key for average runtime. */
#define BS_TIMER_K_AVERAGE_THROUGHPUT    "average_throughput"    /* Key for average throughput. */
#define BS_TIMER_K_AVERAGE_GFLOPS        "average_gflops"        /* Key for average GFLOPS. */
#define BS_TIMER_K_VARIANCE              "variance"              /* Key for variance in runtimes. */
#define BS_TIMER_K_DEVIATION             "deviation"             /* Key for deviation in runtimes. */
#define BS_TIMER_K_TOTAL                 "total"                 /* Key for total runtime. */
#define BS_TIMER_K_OPERATIONS            "operations"            /* Key for number of operations. */

            } //namespace definitions
        } //namespace libvars
    } //namespace timer
}//namespace bs


#endif // BS_TIMER_LIB_VARS_DEFINITIONS_HPP