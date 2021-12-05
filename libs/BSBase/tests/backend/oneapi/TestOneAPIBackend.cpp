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

#include <prerequisites/libraries/catch/catch.hpp>

#include <bs/base/backend/Backend.hpp>

using namespace std;
using namespace bs::base::backend;
using namespace sycl;


void TEST_CASE_ONE_API_BACKEND() {
    Backend backend;

    vector<uint> domainDims{16, 16, 4};
    vector<uint> blockDims{16, 16, 4};

    range<3> test_grid_dimensions(domainDims[0], domainDims[1], domainDims[2]);

    range<3> test_block_dimensions(blockDims[0], blockDims[1], blockDims[2]);

    size_t test_shared_memory = 512;
    uint test_stream = 0;


    auto r = backend.CreateKernelConfiguration(domainDims, blockDims, 512, test_stream);

    REQUIRE(test_grid_dimensions.get(0) == r.mGridDimensions.get(0));
    REQUIRE(test_grid_dimensions.get(1) == r.mGridDimensions.get(1));
    REQUIRE(test_grid_dimensions.get(2) == r.mGridDimensions.get(2));

    REQUIRE(test_block_dimensions.get(0) == r.mBlockDimensions.get(0));
    REQUIRE(test_block_dimensions.get(1) == r.mBlockDimensions.get(1));
    REQUIRE(test_block_dimensions.get(2) == r.mBlockDimensions.get(2));

    REQUIRE(test_shared_memory == r.mSharedMemory);

    REQUIRE(backend.GetExecutor(test_stream) == r.mExecutor);
}

TEST_CASE("ONE API BACKEND", "KERNEL CONFIGURATION CREATION") {
    TEST_CASE_ONE_API_BACKEND();
}
