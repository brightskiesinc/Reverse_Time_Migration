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
#include <operations/backend/OneAPIBackend.hpp>


using namespace operations::backend;
using namespace cl::sycl;

OneAPIBackend::OneAPIBackend() {
    this->mDeviceQueue = nullptr;
    this->mOneAPIAlgorithm = SYCL_ALGORITHM::CPU;
    this->mWorkgroupNumber = 0;
    this->mWorkgroupSize = 0;
}

OneAPIBackend::~OneAPIBackend() {
    delete this->mDeviceQueue;
}

void OneAPIBackend::SetDeviceQueue(cl::sycl::queue *aDeviceQueue) {
    this->mDeviceQueue = aDeviceQueue;
    if (this->mDeviceQueue->get_device().is_cpu()) {
        mWorkgroupNumber =
                this->mDeviceQueue->get_device().get_info<info::device::max_compute_units>();
        mWorkgroupSize =
                this->mDeviceQueue->get_device().get_info<info::device::native_vector_width_double>() * 2;
    } else {
        mWorkgroupNumber =
                this->mDeviceQueue->get_device().get_info<info::device::max_compute_units>() * 4;
        mWorkgroupSize =
                this->mDeviceQueue->get_device().get_info<info::device::max_work_group_size>();
    }
}

void OneAPIBackend::SetAlgorithm(SYCL_ALGORITHM aOneAPIAlgorithm) {
    this->mOneAPIAlgorithm = aOneAPIAlgorithm;
}
