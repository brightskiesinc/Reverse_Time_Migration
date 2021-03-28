//
// Created by amr on 03/01/2021.
//
#include <operations/backend/OneAPIBackend.hpp>

using namespace operations::backend;

OneAPIBackend::OneAPIBackend() {
    this->mDeviceQueue = nullptr;
    this->mOneAPIAlgorithm = SYCL_ALGORITHM::CPU;
}

OneAPIBackend::~OneAPIBackend() {
    delete this->mDeviceQueue;
}

void OneAPIBackend::SetDeviceQueue(cl::sycl::queue *aDeviceQueue) {
    this->mDeviceQueue = aDeviceQueue;
}

void OneAPIBackend::SetAlgorithm(SYCL_ALGORITHM aOneAPIAlgorithm) {
    this->mOneAPIAlgorithm = aOneAPIAlgorithm;
}
