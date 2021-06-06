#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/FrameBuffer.hpp>
#include <memory-manager/MemoryManager.h>
#include <string.h>
#include <iostream>
#include <omp.h>

using namespace operations::dataunits;


static uint MASK_ALLOC_FACTOR = 0;

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
    int device_num = omp_get_default_device();
    if (omp_get_num_devices() <= 0) {
        printf(" ERROR: No device found.\n");
        exit(1);
    }

    mpDataPointer = (T *) omp_target_alloc(mAllocatedBytes, device_num);
    if (mpDataPointer == NULL) {
        printf(" ERROR: No space left on device.\n");
        exit(1);
    }

}

template<typename T>
void FrameBuffer<T>::Allocate(uint aSize, HALF_LENGTH aHalfLength, const std::string &aName) {
    mAllocatedBytes = sizeof(T) * (aSize + 16);
    int device_num = omp_get_default_device();
    if (omp_get_num_devices() <= 0) {
        printf(" ERROR: No device found.\n");
        exit(1);
    }

    mpDataPointer = (T *) omp_target_alloc(mAllocatedBytes, device_num);
    if (mpDataPointer == NULL) {
        printf(" ERROR: No space left on device.\n");
        exit(1);
    }

    MASK_ALLOC_FACTOR += 16;
}

template<typename T>
void FrameBuffer<T>::Free() {
    if (this->mpDataPointer != nullptr) {
        int device_num = omp_get_default_device();
        if (omp_get_num_devices() <= 0) {
            printf(" ERROR: No device found.\n");
        }
        omp_target_free(mpDataPointer, device_num);
        this->mpDataPointer = nullptr;
    }
    if (this->mpHostDataPointer != nullptr) {
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
    if (this->mpDataPointer != nullptr) {
        if (this->mpHostDataPointer == nullptr) {
            this->mpHostDataPointer = (T *) std::malloc(this->mAllocatedBytes);
        }
        Device::MemCpy(this->mpHostDataPointer, this->mpDataPointer, this->mAllocatedBytes,
                       Device::COPY_DEVICE_TO_HOST);
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
//	int device_num = omp_get_default_device();
//	unsigned char* p= (unsigned char *)dest;
//    #pragma omp target is_device_ptr(dest) device(device_num)
//    #pragma omp teams distribute parallel for
//      for(unsigned long int i = 0; i < size; ++i) *p++ = (unsigned char)value;
    char *h_dest = new char[size];
    memset(h_dest, value, size);
    Device::MemCpy(dest, h_dest, size, Device::COPY_HOST_TO_DEVICE);
    delete[] h_dest;

}

void Device::MemCpy(void *dest, const void *src, uint size, CopyDirection direction) {
    int host_num = omp_get_initial_device();
    int device_num = omp_get_default_device();

    if (direction == COPY_HOST_TO_HOST) {
        memcpy(dest, src, size);

    } else if (direction == COPY_HOST_TO_DEVICE) {

        if (omp_get_num_devices() <= 0) {
            printf(" ERROR: No device found.\n");
        }

        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, host_num);

    } else if (direction == COPY_DEVICE_TO_HOST) {

        if (omp_get_num_devices() <= 0) {
            printf(" ERROR: No device found.\n");
        }

        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, host_num, device_num);

    } else if (direction == COPY_DEVICE_TO_DEVICE) {

        if (omp_get_num_devices() <= 0) {
            printf(" ERROR: No device found.\n");
        }

        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, device_num);

    } else {

        if (omp_get_num_devices() <= 0) {
            printf(" ERROR: No device found.\n");
        }

        omp_target_memcpy((void *) dest, (void *) src, size, 0, 0, device_num, host_num);

    }

}
