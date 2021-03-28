//
// Created by ahmed-ayyad on 16/11/2020.
//

#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

#include <memory-manager/MemoryManager.h>

#include <cstring>

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
T *FrameBuffer<T>::GetHostPointer() {
    this->mpHostDataPointer = this->mpDataPointer;
    return this->mpHostDataPointer;
}

template<typename T>
void FrameBuffer<T>::SetNativePointer(T *pT) {
    this->mpDataPointer = pT;
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
