/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_BASE_MEMORY_LOGGER_H
#define BS_BASE_MEMORY_LOGGER_H

namespace bs {
    namespace base {
        namespace memory {

/**
 * Initalize Logger options (Should only be called once at the start):
 * - console_enable : if false(0) disables output on console. Otherwise, will
 *output on console.
 * - file_enable : if false(0) disables output on the file named file_name.
 *Otherwise, will output on the file.
 * - file_name : The name of the file to write to. Will overwrite the file.
 *Won't matter if file_enable is false.
 **/
            void init_logger(char console_enable, char file_enable, char *file_name);

/*
 * - Prints the msg string to the valid streams.
 */
            void log_msg(char *msg);

/*
 * Closes the logger and releases all taken resources.
 */
            void close_logger(void);

/*
 * Flushs output streams
 */
            void log_flush(void);

        } //namespace memory
    } //namespace base
} //namespace bs

#endif
