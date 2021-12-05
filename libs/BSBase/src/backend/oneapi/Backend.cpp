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

#include <vector>

#include <bs/base/backend/Backend.hpp>

using namespace sycl;
using namespace bs::base::backend;


Backend::Backend() {
    PoolType defaultStream;
    this->mExecutorPool.emplace(0, defaultStream);
    this->mDeviceQueue = nullptr;
    this->mOneAPIAlgorithm = SYCL_ALGORITHM::CPU;
    this->mWorkgroupNumber = 0;
    this->mWorkgroupSize = 0;
}

Backend::~Backend() {
    delete this->mDeviceQueue;
}

KernelConfiguration
Backend::CreateKernelConfiguration(std::vector<uint> aDomainDims,
                                   std::vector<uint> aBlockDims,
                                   size_t aSharedMemoryBytes,
                                   uint aExecutorId) {
    for (auto i = aDomainDims.size(); i < 3; i++) {
        aDomainDims.push_back(1);
    }
    for (auto i = aBlockDims.size(); i < 3; i++) {
        aBlockDims.push_back(1);
    }

    KernelConfiguration args;

    sycl::range<3> GridDimensions{1, 1, 1};
    sycl::range<3> BlockDimensions{1, 1, 1};

    uint gridDim_x = aDomainDims[0];
    uint gridDim_y = aDomainDims[1];
    uint gridDim_z = aDomainDims[2];

    BlockDimensions[0] = aBlockDims[0];
    BlockDimensions[1] = aBlockDims[1];
    BlockDimensions[2] = aBlockDims[2];

    GridDimensions[0] = gridDim_x;
    GridDimensions[1] = gridDim_y;
    GridDimensions[2] = gridDim_z;

    args.mBlockDimensions = BlockDimensions;
    args.mGridDimensions = GridDimensions;
    args.mExecutor = this->mExecutorPool[aExecutorId];
    args.mSharedMemory = aSharedMemoryBytes;

    return args;
}

uint
Backend::CreateExecutor() {
    PoolType executor;
    auto executor_idx = this->mExecutorPool.size();
    this->mExecutorPool.emplace(executor_idx, executor);
    return executor_idx;
}

PoolType
Backend::GetExecutor(uint aExecutorID) {
    return this->mExecutorPool[aExecutorID];
}

void
Backend::SetDeviceQueue(cl::sycl::queue *aDeviceQueue) {
    this->mDeviceQueue = aDeviceQueue;
    if (this->mDeviceQueue->get_device().is_cpu()) {
        this->mWorkgroupNumber =
                this->mDeviceQueue->get_device().get_info<info::device::max_compute_units>();
        this->mWorkgroupSize =
                this->mDeviceQueue->get_device().get_info<info::device::native_vector_width_double>() * 2;
    } else {
        this->mWorkgroupNumber =
                this->mDeviceQueue->get_device().get_info<info::device::max_compute_units>() * 4;
        this->mWorkgroupSize =
                this->mDeviceQueue->get_device().get_info<info::device::max_work_group_size>();
    }
}

void
Backend::SetAlgorithm(SYCL_ALGORITHM aOneAPIAlgorithm) {
    this->mOneAPIAlgorithm = aOneAPIAlgorithm;
}
