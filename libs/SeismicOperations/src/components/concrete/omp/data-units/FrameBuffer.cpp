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

#include <cstring>

#include <bs/base/memory/MemoryManager.hpp>

#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

using namespace bs::base::memory;
using namespace operations::dataunits;

template
class operations::dataunits::FrameBuffer<float>;

template
class operations::dataunits::FrameBuffer<int>;

template
class operations::dataunits::FrameBuffer<uint>;

static uint MASK_ALLOC_FACTOR = 0;

template<typename T>
FrameBuffer<T>::FrameBuffer() {
    this->mpDataPointer = nullptr;
    this->mpHostDataPointer = nullptr;
    this->mAllocatedBytes = 0;
}

template<typename T>
FrameBuffer<T>::FrameBuffer(uint aSize) {
    this->Allocate(aSize);
    this->mpHostDataPointer = nullptr;
}

template<typename T>
FrameBuffer<T>::~FrameBuffer() {
    this->Free();
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, const std::string &aName) {
    this->mAllocatedBytes = sizeof(T) * aSize;
    this->mpDataPointer = (T *) mem_allocate(sizeof(T), aSize, aName);
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName) {
    this->mAllocatedBytes = sizeof(T) * (aSize + 16);
    this->mpDataPointer = (T *) mem_allocate(sizeof(T),
                                             aSize,
                                             aName,
                                             aHalfLength,
                                             MASK_ALLOC_FACTOR);
    MASK_ALLOC_FACTOR += 16;
}

template<typename T>
void FrameBuffer<T>::Free() {
    if (this->mpDataPointer != nullptr) {
        mem_free(this->mpDataPointer);
        this->mpDataPointer = nullptr;
        this->mpHostDataPointer = nullptr;
    }
    this->mAllocatedBytes = 0;
}

template<typename T>
T *FrameBuffer<T>::GetNativePointer() {
    return this->mpDataPointer;
}

template<typename T>
void FrameBuffer<T>::SetNativePointer(T *pT) {
    this->mpDataPointer = pT;
}

template<typename T>
T *FrameBuffer<T>::GetHostPointer() {
    this->mpHostDataPointer = this->mpDataPointer;
    return this->mpHostDataPointer;
}

template<typename T>
T *FrameBuffer<T>::GetDiskFlushPointer() {
    return this->GetHostPointer();
}

template<typename T>
void FrameBuffer<T>::ReflectOnNative() {
    /* For omp native is the same as host, so no need to reflect. */
}

void Device::MemSet(void *apDst, int aVal, uint aSize) {
    memset(apDst, aVal, aSize);
}

void Device::MemCpy(void *apDst, const void *apSrc, uint aSize, CopyDirection aCopyDirection) {
    memcpy(apDst, apSrc, aSize);
}
