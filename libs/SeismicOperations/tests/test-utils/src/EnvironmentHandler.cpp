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


#include <operations/test-utils/EnvironmentHandler.hpp>

#ifdef USING_DPCPP

#include <operations/backend/OneAPIBackend.hpp>

using namespace operations::backend;
#endif

namespace operations {
    namespace testutils {

        int set_environment() {
            int rc = 0;
#ifdef USING_DPCPP
            sycl::cpu_selector cpu_sel;
            auto backend = OneAPIBackend::GetInstance();
            if (backend == nullptr) {
                rc = 1;
            } else {
                backend->SetDeviceQueue(
                        new sycl::queue(cpu_sel));
                rc = 0;
            }
#endif
            return rc;
        }

    } //namespace testutils
} //namespace operations
