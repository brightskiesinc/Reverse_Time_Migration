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

#include <bs/base/exceptions/Exceptions.hpp>
#include <bs/base/backend/Backend.hpp>

using namespace bs::base::exceptions;
using namespace bs::base::backend;


Backend::Backend() {
    throw NOT_IMPLEMENTED_EXCEPTION();
}

Backend::~Backend() = default;

KernelConfiguration
Backend::CreateKernelConfiguration(std::vector<uint> aDomainDims,
                                   std::vector<uint> aBlockDims,
                                   size_t aSharedMemoryBytes,
                                   uint aExecutorId) {
    throw NOT_IMPLEMENTED_EXCEPTION();
}

void
Backend::ErrorCheck(void *aResult, char *aFile, int aLine) {
    throw NOT_IMPLEMENTED_EXCEPTION();
}

uint
Backend::CreateExecutor() {
    throw NOT_IMPLEMENTED_EXCEPTION();
}

PoolType
Backend::GetExecutor(uint aExecutorID) {
    throw NOT_IMPLEMENTED_EXCEPTION();
}

void *
Backend::Sync() {
    throw NOT_IMPLEMENTED_EXCEPTION();
}
