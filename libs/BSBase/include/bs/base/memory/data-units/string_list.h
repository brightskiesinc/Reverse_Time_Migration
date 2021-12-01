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

#ifndef BS_BASE_MEMORY_STRING_LIST_H
#define BS_BASE_MEMORY_STRING_LIST_H

#define MULT_RATE 2

namespace bs {
    namespace base {
        namespace memory {

            typedef struct {
                char **strings_list;
                int size;
                int max_size;
            } string_list;

            void init_slist(string_list *list, int size);

            void add_slist(string_list *list, char *word);

            char slist_contains(string_list *list, char *word);

            void destroy_slist(string_list *list);

        } //namespace memory
    } //namespace base
} //namespace bs

#endif //BS_BASE_MEMORY_STRING_LIST_H
