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

#ifndef BS_BASE_MEMORY_MEMORY_TRACKER_H
#define BS_BASE_MEMORY_MEMORY_TRACKER_H

namespace bs {
    namespace base {
        namespace memory {

/**
 * Not memory tracking, then not compiling !.
 **/
#ifdef MEM_TRACK

#include <stdlib.h>

#define malloc(size) omalloc(size, __FILE__, __LINE__, __FUNCTION__)
#define free(ptr) ofree(ptr, __FILE__, __LINE__, __FUNCTION__)

#ifndef MEM_C_ONLY

            void *operator new(size_t size);
            void *operator new[](size_t size);

            void operator delete(void *ptr);
            void operator delete[](void *ptr);

#endif

#define _mm_malloc(size, align)                                                \
  omm_malloc(size, align, __FILE__, __LINE__, __FUNCTION__)

#define _mm_free(ptr) omm_free(ptr, __FILE__, __LINE__, __FUNCTION__)

#define realloc(ptr, new_size)                                                 \
  orealloc(ptr, new_size, __FILE__, __LINE__, __FUNCTION__)

#define calloc(num, size) ocalloc(num, size, __FILE__, __LINE__, __FUNCTION__)

            /* Initializer function that will get executed before main */
            void memory_track_startup(void) __attribute__((constructor));

            /* Clean up function that will get executed after main */
            void memory_track_cleanup(void) __attribute__((destructor));
            /**
             * Gives a name to a pointer to keep track of it in the final output.
             **/
            void name_ptr(void *ptr, char *name);
            // Signal handler, will catch signals like segmentation fault and print the
            // stack trace of it.
            void seg_handler(int signum);

#ifdef __cplusplus
            extern "C" {
#endif
            // override malloc
            extern void *omalloc(size_t size, const char *file, int l, const char *func);
            // override realloc
            extern void *orealloc(void *ptr, size_t new_size, const char *file, int l,
                                  const char *func);
            // override calloc
            extern void *ocalloc(size_t num, size_t size, const char *file, int l,
                                 const char *func);
            // override free
            extern void ofree(void *ptr, const char *file, int l, const char *func);

#ifndef MEM_NO_INTEL
            // override _mm_alloc
            extern void *omm_malloc(size_t size, size_t align, char const *file, int l,
                                    char const *func);

            // override mem_free
            extern void omm_free(void *ptr, char const *file, int l, char const *func);
#endif
#ifdef __cplusplus
            }
#endif
            /**
             * Prints done data at time of checkpoint.
             **/
            void mem_checkpoint(char *name);

            /**
             * Registers c++ file to track.
             **/
            void register_cpp_file(char *name);

#else

/**
 * Empty functions to prevent code dependencies of outer programs.s
 **/
            inline void name_ptr(void *ptr, char *name) { ; }

            inline void mem_checkpoint(char *name) { ; }

            inline void register_cpp_file(char *name) { ; }

#endif

        } //namespace memory
    } //namespace base
} //namespace bs

#endif //BS_BASE_MEMORY_MEMORY_TRACKER_H
