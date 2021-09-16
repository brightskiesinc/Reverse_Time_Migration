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

#ifndef OPERATIONS_LIB_BACKEND_ONEAPI_BACKEND_HPP
#define OPERATIONS_LIB_BACKEND_ONEAPI_BACKEND_HPP

#include <operations/common/Singleton.tpp>

#include <CL/sycl.hpp>
#include "dpc_common.hpp"

namespace operations {
    namespace backend {
        /**
         * @brief
         * The algorithm in effect to be used.
         */
        enum class SYCL_ALGORITHM {
            CPU, GPU, GPU_SHARED, GPU_SEMI_SHARED
        };

        /**
         * @brief
         * The backend used for all computations in the oneAPI technology.
         */
        class OneAPIBackend : public common::Singleton<OneAPIBackend> {
        public:
            friend class Singleton<OneAPIBackend>;

        public:
            /**
             * @brief
             * Get the Device queue in use for the DPC++ computations.
             *
             * @return
             * A pointer to the Device queue in use.
             */
            inline cl::sycl::queue *GetDeviceQueue() {
                return mDeviceQueue;
            }

            /**
             * @brief
             * Setter for the Device queue being in use.
             *
             * @param[in] aDeviceQueue
             */
            void SetDeviceQueue(cl::sycl::queue *aDeviceQueue);

            /**
             * @brief
             * Get the algorithm to be used in dpc++ computations.
             *
             * @return
             * The algorithm to be used in all dpc++ computations.
             */
            inline SYCL_ALGORITHM GetAlgorithm() {
                return mOneAPIAlgorithm;
            }

            /**
             * @brief
             * Setter for the algorithm to be used.
             *
             * @param[in] aOneAPIAlgorithm
             * The algorithm to be used.
             */
            void SetAlgorithm(SYCL_ALGORITHM aOneAPIAlgorithm);

            /**
             * @brief
             * Gets the recommended workgroup number.
             */
            inline size_t GetWorkgroupNumber() const {
                return mWorkgroupNumber;
            }

            /**
             * @brief
             * Gets the recommended workgroup size.
             */
            inline size_t GetWorkgroupSize() const {
                return mWorkgroupSize;
            }

        private:
            /**
             * @brief
             * Default Constructor.
             */
            OneAPIBackend();

            /**
             * @brief
             * Default Destructor.
             */
            ~OneAPIBackend();

        private:
            /// The Device queue.
            cl::sycl::queue *mDeviceQueue;
            /// The DPC++ underlying algorithm being used.
            SYCL_ALGORITHM mOneAPIAlgorithm;
            /// number of available workgroups for nd_range kernels
            size_t mWorkgroupNumber;
            /// size of workgroup for nd_range kernels
            size_t mWorkgroupSize;

        };
    } //namespace backend
} //namespace operations

#endif //OPERATIONS_LIB_BACKEND_ONEAPI_BACKEND_HPP
