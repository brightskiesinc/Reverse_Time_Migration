/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPERATIONS_LIB_CONFIGURATIONS_MAP_KEYS_HPP
#define OPERATIONS_LIB_CONFIGURATIONS_MAP_KEYS_HPP

namespace operations {
    namespace configurations {

#define OP_K_PROPRIETIES               "properties"
#define OP_K_USE_TOP_LAYER             "use-top-layer"
#define OP_K_SHOT_STRIDE               "shot-stride"
#define OP_K_REFLECT_COEFFICIENT       "reflect-coeff"
#define OP_K_SHIFT_RATIO               "shift-ratio"
#define OP_K_RELAX_COEFFICIENT         "relax-coeff"
#define OP_K_ZFP_TOLERANCE             "zfp-tolerance"
#define OP_K_ZFP_PARALLEL              "zfp-parallel"
#define OP_K_ZFP_RELATIVE              "zfp-relative"
#define OP_K_WRITE_PATH                "write-path"
#define OP_K_COMPRESSION               "compression"
#define OP_K_COMPRESSION_TYPE          "compression-type"
#define OP_K_BOUNDARY_SAVING           "boundary-saving"
#define OP_K_COMPENSATION              "compensation"
#define OP_K_COMPENSATION_NONE         "none"
#define OP_K_COMPENSATION_COMBINED     "combined"
#define OP_K_COMPENSATION_RECEIVER     "receiver"
#define OP_K_COMPENSATION_SOURCE       "source"
#define OP_K_COMPENSATION              "compensation"
#define OP_K_TYPE                      "type"
#define OP_K_HEADER_ONLY               "header-only"
#define OP_K_INTERPOLATION             "interpolation"
#define OP_K_NONE                      "none"
#define OP_K_SPLINE                    "spline"
#define OP_K_OUTPUT_FILE               "output-file"
#define OP_K_MAX_FREQ_AMP_PERCENT      "max-freq-amplitude-percentage"
#define OP_K_DIP_ANGLE                 "dip-angle"
#define OP_K_DEPTH_SAMPLING_SCALING    "depth-sampling-scaling"
#define OP_K_GRAIN_SIDE_LENGTH         "grain-side-length"

    } //namespace configuration
} //namespace operations

#endif //OPERATIONS_LIB_CONFIGURATIONS_MAP_KEYS_HPP
