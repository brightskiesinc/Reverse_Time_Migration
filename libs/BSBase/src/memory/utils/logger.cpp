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

#include <bs/base/memory/utils/logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace bs {
    namespace base {
        namespace memory {

#define INITIAL_BUFFER_SIZE 10000LLU
#define GROWTH_RATE 2
/**
 * Logger internal settings.
 **/
            static char console_enable = 0;
            static char file_enable = 0;
            static char file_n[1000] = "";
            static FILE *cur_file = NULL;
            static long long unsigned buffer_size;
            static long long unsigned current_size;
            static char *buffer = NULL;

            void init_logger(char ce, char fe, char *file_name) {
                // Initialize internal buffer.
                buffer_size = INITIAL_BUFFER_SIZE;
                current_size = 0;
                buffer = (char *) malloc(INITIAL_BUFFER_SIZE);
                strncpy(buffer, "", buffer_size);
                // Store settings.
                console_enable = ce;
                file_enable = fe;
                if (fe == 1) {
                    // Open file in case outputing to file.
                    cur_file = fopen(file_name, "w");
                    strncpy(file_n, file_name, 1000);
                }
            }

            void log_msg(char *message) {
                if (buffer == NULL) {
                    // Print messages.
                    if (console_enable) {
                        printf("%s", message);
                    }
                    if (file_enable) {
                        if (strcmp(file_n, "") == 0) {
                            printf("no file to output on this : %s", message);
                            return;
                        }
                        char close_after_usage = 0;
                        if (cur_file == NULL) {
                            cur_file = fopen(file_n, "a");
                            close_after_usage = 1;
                        }
                        fprintf(cur_file, "%s", message);
                        if (close_after_usage) {
                            fclose(cur_file);
                            cur_file = NULL;
                        }
                    }
                    return;
                }
                int len_msg = strlen(message);
                // Make buffer bigger if needed.
                while (len_msg + current_size + 1 >= buffer_size) {
                    buffer_size *= GROWTH_RATE;
                    char *temp_buffer = (char *) realloc(buffer, buffer_size);
                    if (temp_buffer != NULL) {
                        buffer = temp_buffer;
                    } else {
                        log_flush();
                    }
                }
                // Add to buffer.
                strncat(buffer + current_size, message, buffer_size - current_size - 1);
                current_size += len_msg;
            }

            void close_logger(void) {
                if (file_enable == 1) {
                    if (cur_file == NULL) {
                        cur_file = fopen(file_n, "a");
                    }
                }
                // Flush buffers.
                log_flush();
                // Close output file if file is opened.
                if (file_enable == 1) {
                    fclose(cur_file);
                }
                // Free Buffer Space.
                free(buffer);
                buffer = NULL;
                cur_file = NULL;
            }

            void log_flush(void) {
                // Print messages.
                if (console_enable) {
                    printf("%s", buffer);
                }
                if (file_enable) {
                    fprintf(cur_file, "%s", buffer);
                }
                // Reset buffer.
                current_size = 0;
                strncpy(buffer, "", buffer_size);
                // Flush output.
                if (console_enable) {
                    fflush(stdout);
                }
                if (file_enable) {
                    fflush(cur_file);
                }
            }

        } //namespace memory
    } //namespace base
} //namespace bs
