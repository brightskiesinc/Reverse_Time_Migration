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

#ifndef BS_BASE_BACKENDS_TECHNOLOGY_HPP
#define BS_BASE_BACKENDS_TECHNOLOGY_HPP

#include <exception>
#include <string>


#ifdef USING_DPCPP

#include <CL/sycl.hpp>
#include <dpc_common.hpp>

#endif

namespace bs {
    namespace base {
        namespace backend {

#ifdef USING_DPCPP
            typedef sycl::queue PoolType;
#else
            typedef int PoolType;
#endif

            struct KernelConfiguration {
                /// Dimensions of the gpu grid in blocks (x, y, z)
#ifdef USING_DPCPP
                sycl::range<3> mGridDimensions{1, 1, 1};
                /// Dimensions of a single gpu computation block in threads (x, y, z)
                sycl::range<3> mBlockDimensions{1, 1, 1};
#endif
                size_t mSharedMemory;
                PoolType mExecutor;

                KernelConfiguration() {};
            };
        }   // namespace backend
    } //namespace base
} //namespace bs

#endif //BS_BASE_BACKENDS_TECHNOLOGY_HPP
