//
// Created by zeyad-osama on 01/02/2021.
//

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
    if (backend == nullptr){
        rc = 1;
    }else{
    backend->SetDeviceQueue(
            new sycl::queue(cpu_sel));
        rc = 0;
    }
#endif
            return rc;
        }

    } //namespace testutils
} //namespace operations
