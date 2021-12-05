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

#ifndef BS_BASE_BACKENDS_BACKEND_HPP
#define BS_BASE_BACKENDS_BACKEND_HPP

#include <unordered_map>
#include <vector>

#include <bs/base/common/Singleton.tpp>
#include <bs/base/backend/Technology.hpp>

namespace bs {
    namespace base {
        namespace backend {

#ifdef USING_DPCPP
            /**
             * @brief
             * The algorithm in effect to be used.
             */
            enum class SYCL_ALGORITHM {
                CPU, GPU, GPU_SHARED, GPU_SEMI_SHARED
            };
#endif

            class Backend : public common::Singleton<Backend> {
            public:
                friend class Singleton<Backend>;

            public:
                /**
                 * @brief
                 * Default constructor
                 */
                Backend();

                /**
                 * @brief
                 * Default destructor
                 */
                ~Backend();

                /**
                 * @brief
                 * Creates CUDA kernel launch configuration arguments
                 * @param[in] aDomainDims
                 * Dimensions of the domain to be mapped to the grid
                 * @param[in] aBlockDims
                 * Dimensions of the thread computation thread block
                 * @param[in] aSharedMemoryBytes
                 * Size of the shared memory associated with the kernel
                 * launch (in bytes)
                 * @param[in] aExecutorId
                 * Index of the stream associated with the kernel launch
                 * @return
                 * Returns a pointer to the kernel launch configuration arguments
                 * (Pointer of type KernelConfiguration)
                 */
                KernelConfiguration
                CreateKernelConfiguration(std::vector<uint> aDomainDims,
                                          std::vector<uint> aBlockDims,
                                          size_t aSharedMemoryBytes = 0,
                                          uint aExecutorId = 0);

                /**
                 * @brief
                 * CUDA error handling function
                 * @throw TechnologyException
                 * Check cudaError_t for success, throws exception otherwise
                 */
                void
                ErrorCheck(void *aResult, char *aFile, int aLine);

                /**
                 * @return Executor index in the executor pool map
                 */
                uint
                CreateExecutor();

                /**
                 * @brief
                 * Get the executor (CUDA stream, OneAPI queue, etc.)
                 * @param aExecutorID
                 * unsigned int representing the executor inside in the pool table
                 * @return executor object
                 */
                PoolType
                GetExecutor(uint aExecutorID);

                /**
                 * @brief Synchronization barrier across the device used
                 */
                void *
                Sync();

#ifdef USING_DPCPP

                /**
                 * @brief
                 * Get the Device queue in use for the DPC++ computations.
                 *
                 * @return
                 * A pointer to the Device queue in use.
                 */
                inline cl::sycl::queue *
                GetDeviceQueue() { return this->mDeviceQueue; }

                /**
                 * @brief
                 * Setter for the Device queue being in use.
                 *
                 * @param[in] aDeviceQueue
                 */
                void
                SetDeviceQueue(cl::sycl::queue *aDeviceQueue);

                /**
                 * @brief
                 * Get the algorithm to be used in dpc++ computations.
                 *
                 * @return
                 * The algorithm to be used in all dpc++ computations.
                 */
                inline SYCL_ALGORITHM
                GetAlgorithm() { return this->mOneAPIAlgorithm;}

                /**
                 * @brief
                 * Setter for the algorithm to be used.
                 *
                 * @param[in] aOneAPIAlgorithm
                 * The algorithm to be used.
                 */
                void
                SetAlgorithm(SYCL_ALGORITHM aOneAPIAlgorithm);

                /**
                 * @brief
                 * Gets the recommended workgroup number.
                 */
                inline size_t
                GetWorkgroupNumber() const { return this->mWorkgroupNumber; }

                /**
                 * @brief
                 * Gets the recommended workgroup size.
                 */
                inline size_t
                GetWorkgroupSize() const { return this->mWorkgroupSize; }

#endif

            private:
                std::unordered_map<uint, PoolType> mExecutorPool;
#ifdef USING_DPCPP
                /// The Device queue.
                cl::sycl::queue *mDeviceQueue;
                /// The DPC++ underlying algorithm being used.
                SYCL_ALGORITHM mOneAPIAlgorithm;
                /// number of available workgroups for nd_range kernels
                size_t mWorkgroupNumber;
                /// size of workgroup for nd_range kernels
                size_t mWorkgroupSize;
#endif
            };

        } // namespace backend
    } //namespace base
} //namespace bs

#endif //BS_BASE_BACKENDS_BACKEND_HPP
