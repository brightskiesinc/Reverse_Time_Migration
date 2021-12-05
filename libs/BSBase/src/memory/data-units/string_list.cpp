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

#include <stdlib.h>
#include <string.h>

#include <bs/base/memory/data-units/string_list.h>

namespace bs {
    namespace base {
        namespace memory {

            void init_slist(string_list *list, int size) {
                list->size = 0;
                list->strings_list = (char **) malloc(sizeof(char *) * size);
                list->max_size = size;
            }

            void add_slist(string_list *list, char *word) {
                if (list->max_size == 0) {
                    return;
                }
                if (list->size == list->max_size) {
                    list->max_size *= MULT_RATE;
                    list->strings_list =
                            (char **) realloc(list->strings_list, sizeof(char *) * list->max_size);
                }
                list->strings_list[list->size] =
                        (char *) malloc(sizeof(char) * (strlen(word) + 1));
                strncpy(list->strings_list[list->size], word, strlen(word) + 1);
                list->size++;
            }

            char slist_contains(string_list *list, char *word) {
                int i = 0;
                for (i = 0; i < list->size; i++) {
                    if (strcmp(word, list->strings_list[i]) == 0) {
                        return 1;
                    }
                }
                return 0;
            }

            void destroy_slist(string_list *list) {
                int i = 0;
                for (i = 0; i < list->size; i++) {
                    free(list->strings_list[i]);
                }
                free(list->strings_list);
                list->size = 0;
                list->max_size = 0;
            }

        } //namespace memory
    } //namespace base
} //namespace bs
