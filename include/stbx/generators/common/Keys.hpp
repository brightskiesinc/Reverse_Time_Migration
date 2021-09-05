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
#ifndef SEISMIC_TOOLBOX_GENERATORS_KEYS_H
#define SEISMIC_TOOLBOX_GENERATORS_KEYS_H

#define DEF_VAL -1

/*
 * COMMON
 */

#define K_ENABLE                            "enable"
#define OP_K_TYPE                           "type"

/*
 * CONFIGURATIONS
 */

#define K_CALLBACKS                         "callbacks"
#define K_COMPUTATION_PARAMETERS            "computation-parameters"
#define K_TRACES                            "traces"
#define K_MODELS                            "models"
#define K_WAVE                              "wave"
#define K_PHYSICS                           "physics"
#define K_APPROXIMATION                     "approximation"
#define K_EQUATION_ORDER                    "equation-order"
#define K_SAMPLING                          "sampling"
#define K_GRID_SAMPLING                     "grid-sampling"
// Model Files
#define K_DENSITY                           "density"
#define K_DELTA                             "delta"
#define K_EPSILON                           "epsilon"
#define K_THETA                             "theta"
#define K_PHI                               "phi"
// Files
#define K_MIN                               "min"
#define K_MAX                               "max"
#define K_SORT_TYPE                         "sort-type"
#define K_PATHS                             "paths"
#define K_MODELLING_FILE                    "modelling-file"
#define K_OUTPUT_FILE                       "output-file"


/*
 * COMPUTATION PARAMETERS
 */
#define K_WINDOW                            "window"
#define K_LEFT                              "left"
#define K_RIGHT                             "right"
#define K_DEPTH                             "depth"
#define K_FRONT                             "front"
#define K_BACK                              "back"
#define K_STENCIL_ORDER                     "stencil-order"
#define K_BOUNDARY_LENGTH                   "boundary-length"
#define K_SOURCE_FREQUENCY                  "source-frequency"
#define K_DT_RELAX                          "dt-relax"
#define K_CACHE_BLOCKING                    "cache-blocking"
#define K_ISOTROPIC_CIRCLE                  "isotropic-radius"



/*
 * CALLBACKS
 */

#define K_SHOW_EACH                         "show-each"
#define K_NORM                              "norm"
#define K_BIN                               "bin"
#define K_CSV                               "csv"
#define K_SEGY                              "segy"
#define K_SU                                "su"
#define K_IMAGE                             "image"
#define K_NORM                              "norm"

#define K_PERC                              "percentile"
#define K_LITTLE_ENDIAN                     "little-endian"

#define K_WRITER                            "writer"
#define K_ACTIVE_TYPES                      "active-types"
#define K_WRITERS_CONFIGURATION             "writers-configuration"
#define K_PARAMETERS                        "parameters"
#define K_VELOCITY                          "velocity"
#define K_FORWARD                           "forward"
#define K_BACKWARD                          "backward"
#define K_REVERSE                           "reverse"
#define K_MIGRATION                         "migration"
#define K_RE_EXTENDED_PARAMETERS            "re-extended-parameters"
#define K_SINGLE_SHOT_CORR                  "single-shot-correlation"
#define K_EACH_STACKED_SHOT                 "each-stacked-shot"
#define K_TRACE_RAW                         "traces-raw"
#define K_TRACES_PREPROCESSED               "traces-preprocessed"
#define K_OUTPUT                            "output"

/*
 * COMPONENTS
 */

#define K_COMPONENTS                        "components"
#define K_COMPUTATION_KERNEL                "computation-kernel"
#define K_BOUNDARY_MANAGER                  "boundary-manager"
#define K_MIGRATION_ACCOMMODATOR            "migration-accommodator"
#define K_FORWARD_COLLECTOR                 "forward-collector"
#define K_TRACE_MANAGER                     "trace-manager"
#define K_SOURCE_INJECTOR                   "source-injector"
#define K_MODEL_HANDLER                     "model-handler"
#define K_TRACE_WRITER                      "trace-writer"
#define K_MODELLING_CONFIGURATION_PARSER    "modelling-configuration-parser"


/*
 * SYSTEM
 */

#define K_SYSTEM                            "system"
#define K_AGENT                             "agent"
#define K_WRITER                            "writer"
#define K_ALGORITHM                         "algorithm"
#define K_TIMER                             "timer"
#define K_TIMER_PROPERTIES                  "properties"
#define K_TIME_UNIT                         "precision"

/*
 * SUPPORTED VALUES
 */

#define K_SUPPORTED_VALUES_BOUNDARY_MANAGER "[ none | random | sponge | cpml ]"
#define K_SUPPORTED_VALUES_FORWARD_COLLECTOR "[ two | three ]"

#endif //SEISMIC_TOOLBOX_GENERATORS_KEYS_H
