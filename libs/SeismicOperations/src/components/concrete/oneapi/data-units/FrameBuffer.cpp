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

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>

using namespace bs::base::backend;
using namespace operations::dataunits;

template
class operations::dataunits::FrameBuffer<float>;

template
class operations::dataunits::FrameBuffer<int>;

template
class operations::dataunits::FrameBuffer<uint>;

template FrameBuffer<float>::FrameBuffer();

template FrameBuffer<float>::FrameBuffer(uint aSize);

template FrameBuffer<float>::~FrameBuffer();

template void FrameBuffer<float>::Allocate(uint size, const std::string &aName);

template void FrameBuffer<float>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<float>::Free();

template float *FrameBuffer<float>::GetNativePointer();

template float *FrameBuffer<float>::GetHostPointer();

template float *FrameBuffer<float>::GetDiskFlushPointer();

template void FrameBuffer<float>::SetNativePointer(float *ptr);

template void FrameBuffer<float>::ReflectOnNative();

template FrameBuffer<int>::FrameBuffer();

template FrameBuffer<int>::FrameBuffer(uint aSize);

template FrameBuffer<int>::~FrameBuffer();

template void FrameBuffer<int>::Allocate(uint size, const std::string &aName);

template void FrameBuffer<int>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<int>::Free();

template int *FrameBuffer<int>::GetNativePointer();

template int *FrameBuffer<int>::GetHostPointer();

template int *FrameBuffer<int>::GetDiskFlushPointer();

template void FrameBuffer<int>::SetNativePointer(int *ptr);

template void FrameBuffer<int>::ReflectOnNative();

template FrameBuffer<uint>::FrameBuffer();

template FrameBuffer<uint>::FrameBuffer(uint aSize);

template FrameBuffer<uint>::~FrameBuffer();

template void FrameBuffer<uint>::Allocate(uint size, const std::string &aName);

template void FrameBuffer<uint>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName);

template void FrameBuffer<uint>::Free();

template uint *FrameBuffer<uint>::GetNativePointer();

template uint *FrameBuffer<uint>::GetHostPointer();

template uint *FrameBuffer<uint>::GetDiskFlushPointer();

template void FrameBuffer<uint>::SetNativePointer(uint *ptr);

template void FrameBuffer<uint>::ReflectOnNative();

template<typename T>
FrameBuffer<T>::FrameBuffer() {
    mpDataPointer = nullptr;
    mpHostDataPointer = nullptr;
    mAllocatedBytes = 0;
}

template<typename T>
FrameBuffer<T>::FrameBuffer(uint aSize) {
    Allocate(aSize);
    mpHostDataPointer = nullptr;
}

template<typename T>
FrameBuffer<T>::~FrameBuffer() {
    Free();
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, const std::string &aName) {
    mAllocatedBytes = sizeof(T) * aSize;
    auto dev = Backend::GetInstance()->GetDeviceQueue()->get_device();
    auto ctxt = Backend::GetInstance()->GetDeviceQueue()->get_context();

    mpDataPointer = (T *) malloc_device(mAllocatedBytes, dev, ctxt);
}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName) {
    mAllocatedBytes = sizeof(T) * aSize;
    auto dev = Backend::GetInstance()->GetDeviceQueue()->get_device();
    auto ctxt = Backend::GetInstance()->GetDeviceQueue()->get_context();

    mpDataPointer = (T *) malloc_device(mAllocatedBytes, dev, ctxt);
}

template<typename T>
void FrameBuffer<T>::Free() {
    if (mpDataPointer != nullptr) {
        sycl::free(mpDataPointer, *Backend::GetInstance()->GetDeviceQueue());
        mpDataPointer = nullptr;
        if (mpHostDataPointer != nullptr) {
            sycl::free(mpHostDataPointer, *Backend::GetInstance()->GetDeviceQueue());
            mpHostDataPointer = nullptr;
        }
    }
    mAllocatedBytes = 0;
}

template<typename T>
T *FrameBuffer<T>::GetNativePointer() {
    return mpDataPointer;
}

template<typename T>
T *FrameBuffer<T>::GetHostPointer() {
    if (mAllocatedBytes > 0) {
        auto ctxt = Backend::GetInstance()->GetDeviceQueue()->get_context();
        if (mpHostDataPointer == nullptr) {
            mpHostDataPointer = (T *) malloc_host(mAllocatedBytes, ctxt);
        }
        Device::MemCpy(mpHostDataPointer, mpDataPointer, mAllocatedBytes,
                       Device::COPY_DEVICE_TO_HOST);
    }
    return mpHostDataPointer;
}

template<typename T>
T *FrameBuffer<T>::GetDiskFlushPointer() {
    return this->GetHostPointer();
}

template<typename T>
void FrameBuffer<T>::SetNativePointer(T *pT) {
    mpDataPointer = pT;
}

template<typename T>
void FrameBuffer<T>::ReflectOnNative() {
    Device::MemCpy(mpDataPointer, mpHostDataPointer, mAllocatedBytes, Device::COPY_HOST_TO_DEVICE);
}


void Device::MemSet(void *apDst, int aVal, uint aSize) {
    Backend::GetInstance()->GetDeviceQueue()->submit([&](sycl::handler &cgh) {
        cgh.memset(apDst, aVal, aSize);
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}

void Device::MemCpy(void *apDst, const void *apSrc, uint aSize, CopyDirection aCopyDirection) {
    Backend::GetInstance()->GetDeviceQueue()->submit([&](sycl::handler &cgh) {
        cgh.memcpy(apDst, apSrc, aSize);
    });
    Backend::GetInstance()->GetDeviceQueue()->wait();
}
