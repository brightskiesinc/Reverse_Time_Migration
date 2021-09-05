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

#ifndef BS_BASE_MEMORY_MEMORY_ALLOCATOR_H
#define BS_BASE_MEMORY_MEMORY_ALLOCATOR_H

#include <string>

namespace bs {
    namespace base {
        namespace memory {

/**
 * @generalnote
 * In malloc and mm__malloc they use as byte_size (size_t )
 * which equals unsigned long long void * malloc( size_t size );
 */

/**
 * @brief Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 *
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 *
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 *
 * @param name
 * A user given name for the pointer for tracking purposes.
 *
 * @return
 * A void aligned pointer with the given size allocated.
 */
            void *mem_allocate(unsigned long long size_of_type,
                               unsigned long long number_of_elements, const std::string &name);

/**
 * @brief Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 *
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 *
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 *
 * @param name
 * A user given name for the pointer for tracking purposes.
 *
 * @param half_length_padding
 * A padding to be added to the alignment.
 *
 * @return
 * A void aligned pointer with the given size allocated.
 */
            void *mem_allocate(unsigned long long size_of_type,
                               unsigned long long number_of_elements, const std::string &name,
                               uint half_length_padding);

/**
 * @brief Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 *
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 *
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 *
 * @param name
 * A user given name for the pointer for tracking purposes.
 *
 * @param half_length_padding
 * A padding to be added to the alignment.
 *
 * @param masking_allocation_factor
 * An offset to differentiate and make sure of different pointer caching.
 *
 * @return
 * A void aligned pointer with the given size allocated.
 */
            void *mem_allocate(unsigned long long size_of_type,
                               unsigned long long number_of_elements, const std::string &name,
                               uint half_length_padding, uint masking_allocation_factor);

/**
 * @brief Frees an aligned memory block.
 *
 * @param ptr
 * The aligned void pointer to be freed.
 */
            void mem_free(void *ptr);

        } //namespace memory
    } //namespace base
} //namespace bs

#endif //BS_BASE_MEMORY_MEMORY_ALLOCATOR_H
