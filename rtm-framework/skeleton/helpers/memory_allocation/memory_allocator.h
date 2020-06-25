//
// Created by amrnasr on 20/10/2019.
//

#ifndef RTM_FRAMEWORK_MEMORY_ALLOCATOR_H
#define RTM_FRAMEWORK_MEMORY_ALLOCATOR_H

#include <skeleton/base/datatypes.h>

#include <string>

using namespace std;
// general note: in malloc and mm__malloc they use as byte_size (size_t )which
// equals unsigned long long
// void * malloc( size_t size );
/*!
 * Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 * @param name
 * A user given name for the pointer for tracking purposes.
 * @return
 * A void aligned pointer with the given size allocated.
 */
void *mem_allocate(const unsigned long long size_of_type,
                   const unsigned long long number_of_elements, string name);
/*!
 * Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 * @param name
 * A user given name for the pointer for tracking purposes.
 * @param half_length_padding
 * A padding to be added to the alignment.
 * @return
 * A void aligned pointer with the given size allocated.
 */
void *mem_allocate(const unsigned long long size_of_type,
                   const unsigned long long number_of_elements, string name,
                   uint half_length_padding);
/*!
 * Allocates aligned memory and returns an aligned pointer with the requested
 * size.
 * @param size_of_type
 * The size in bytes of a single object that this pointer should point to.
 * Normally given by sizeof(type).
 * @param number_of_elements
 * The number of elements that our pointer should contain.
 * @param name
 * A user given name for the pointer for tracking purposes.
 * @param half_length_padding
 * A padding to be added to the alignment.
 * @param masking_allocation_factor
 * An offset to differentiate and make sure of different pointer caching.
 * @return
 * A void aligned pointer with the given size allocated.
 */
void *mem_allocate(const unsigned long long size_of_type,
                   const unsigned long long number_of_elements, string name,
                   uint half_length_padding, uint masking_allocation_factor);
/*!
 * Frees an aligned memory block.
 * @param ptr
 * The aligned void pointer to be freed.
 */
void mem_free(void *ptr);

#endif // RTM_FRAMEWORK_MEMORY_ALLOCATOR_H
