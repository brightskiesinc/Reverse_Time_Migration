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

#include <operations/common/DataTypes.h>

#include <operations/data-units/concrete/holders/FrameBuffer.hpp>
#include <operations/utils/checks/Checks.hpp>

#include <bs/base/exceptions/Exceptions.hpp>

#include <string.h>
#include <omp.h>

using namespace operations::dataunits;
using namespace operations::utils::checks;
using namespace bs::base::exceptions;


/** @note
 * For the omp offload compiler all the possibles values for the template should
 * be declared before usage, or it won't compile
 */

template
class operations::dataunits::FrameBuffer<float>;

template
class operations::dataunits::FrameBuffer<int>;

template
class operations::dataunits::FrameBuffer<uint>;

template FrameBuffer<float>::FrameBuffer();

template FrameBuffer<float>::FrameBuffer(uint size);

template FrameBuffer<float>::~FrameBuffer();

template void FrameBuffer<float>::Allocate(uint aSize, const std::string &aName);

template void FrameBuffer<float>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<float>::Free();

template float *FrameBuffer<float>::GetNativePointer();

template float *FrameBuffer<float>::GetHostPointer();

template void FrameBuffer<float>::SetNativePointer(float *ptr);

template void FrameBuffer<float>::ReflectOnNative();


template FrameBuffer<int>::FrameBuffer();

template FrameBuffer<int>::FrameBuffer(uint size);

template FrameBuffer<int>::~FrameBuffer();

template void FrameBuffer<int>::Allocate(uint aSize, const std::string &aName);

template void FrameBuffer<int>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<int>::Free();

template int *FrameBuffer<int>::GetNativePointer();

template int *FrameBuffer<int>::GetHostPointer();

template void FrameBuffer<int>::SetNativePointer(int *ptr);

template void FrameBuffer<int>::ReflectOnNative();


template FrameBuffer<uint>::FrameBuffer();

template FrameBuffer<uint>::FrameBuffer(uint size);

template FrameBuffer<uint>::~FrameBuffer();

template void FrameBuffer<uint>::Allocate(uint aSize, const std::string &aName);

template void FrameBuffer<uint>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<uint>::Free();

template uint *FrameBuffer<uint>::GetNativePointer();

template uint *FrameBuffer<uint>::GetHostPointer();

template void FrameBuffer<uint>::SetNativePointer(uint *ptr);

template void FrameBuffer<uint>::ReflectOnNative();


template<typename T>
FrameBuffer<T>::FrameBuffer() {
    mpDataPointer = nullptr;
    mpHostDataPointer = nullptr;
    mAllocatedBytes = 0;
}

template<typename T>
FrameBuffer<T>::FrameBuffer(uint size) {
    Allocate(size);
    mpHostDataPointer = nullptr;
}

template<typename T>
FrameBuffer<T>::~FrameBuffer() {
    Free();
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, const std::string &aName) {
    mAllocatedBytes = sizeof(T) * aSize;

    /* Finding gpu device. */

    int device_num = omp_get_default_device();
    if (is_device_not_exist()) {
        throw DEVICE_NOT_FOUND_EXCEPTION();
    }

    mpDataPointer = (T *) omp_target_alloc(mAllocatedBytes, device_num);

    if (no_space_exist(mpDataPointer)) {
        throw DEVICE_NO_SPACE_EXCEPTION();
    }
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName) {
    Allocate(aSize, aName);
}

template<typename T>
void FrameBuffer<T>::Free() {
    if (!is_null_ptr(this->mpDataPointer)) {

        /* Finding gpu device. */
        int device_num = omp_get_default_device();
        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }
        omp_target_free(mpDataPointer, device_num);
        this->mpDataPointer = nullptr;
    }
    if (!is_null_ptr(this->mpHostDataPointer)) {
        free(mpHostDataPointer);
        this->mpHostDataPointer = nullptr;
    }
    this->mAllocatedBytes = 0;
}

template<typename T>
T *FrameBuffer<T>::GetNativePointer() {
    return mpDataPointer;
}

template<typename T>
T *FrameBuffer<T>::GetHostPointer() {
    if (!is_null_ptr(this->mpDataPointer)) {
        if (is_null_ptr(this->mpHostDataPointer)) {
            this->mpHostDataPointer = (T *) std::malloc(this->mAllocatedBytes);
        }
        Device::MemCpy(this->mpHostDataPointer, this->mpDataPointer, this->mAllocatedBytes,
                       Device::COPY_DEVICE_TO_HOST);
    } else {
        throw NULL_POINTER_EXCEPTION();
    }
    return this->mpHostDataPointer;
}

template<typename T>
void FrameBuffer<T>::SetNativePointer(T *pT) {
    this->mpDataPointer = pT;
}

template<typename T>
void FrameBuffer<T>::ReflectOnNative() {
    Device::MemCpy(this->mpDataPointer, this->mpHostDataPointer, this->mAllocatedBytes, Device::COPY_HOST_TO_DEVICE);
}


void Device::MemSet(void *dest, int value, uint size) {
    char *h_dest = new char[size];
    memset(h_dest, value, size);
    Device::MemCpy(dest, h_dest, size, Device::COPY_HOST_TO_DEVICE);
    delete[] h_dest;

}

void Device::MemCpy(void *dest, const void *src, uint size, CopyDirection direction) {
    /* Finding the host CPU. */
    int host_num = omp_get_initial_device();

    /* Finding the GPU device. */
    int device_num = omp_get_default_device();

    if (direction == COPY_HOST_TO_HOST) {
        memcpy(dest, src, size);
    } else if (direction == COPY_HOST_TO_DEVICE) {
        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }
        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, host_num);
    } else if (direction == COPY_DEVICE_TO_HOST) {
        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }
        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, host_num, device_num);
    } else if (direction == COPY_DEVICE_TO_DEVICE) {
        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }
        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, device_num);
    } else {
        /* Default is considered to be copying from the host to the device,
         * so it would be the same cased as  COPY_HOST_TO_DEVICE. */
        if (is_device_not_exist()) {
            throw DEVICE_NOT_FOUND_EXCEPTION();
        }
        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, host_num);
    }
}
