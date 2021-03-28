//
// Created by amr on 03/01/2021.
//
#include "operations/components/independents/concrete/computation-kernels/isotropic/StaggeredComputationKernel.hpp"

#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include "operations/exceptions/NotImplementedException.h"

using namespace std;
using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

template void StaggeredComputationKernel::Compute<true, O_2>();

template void StaggeredComputationKernel::Compute<true, O_4>();

template void StaggeredComputationKernel::Compute<true, O_8>();

template void StaggeredComputationKernel::Compute<true, O_12>();

template void StaggeredComputationKernel::Compute<true, O_16>();

template void StaggeredComputationKernel::Compute<false, O_2>();

template void StaggeredComputationKernel::Compute<false, O_4>();

template void StaggeredComputationKernel::Compute<false, O_8>();

template void StaggeredComputationKernel::Compute<false, O_12>();

template void StaggeredComputationKernel::Compute<false, O_16>();

template void StaggeredComputationKernel::Compute<true, O_2>();

template void StaggeredComputationKernel::Compute<true, O_4>();

template void StaggeredComputationKernel::Compute<true, O_8>();

template void StaggeredComputationKernel::Compute<true, O_12>();

template void StaggeredComputationKernel::Compute<true, O_16>();

template void StaggeredComputationKernel::Compute<false, O_2>();

template void StaggeredComputationKernel::Compute<false, O_4>();

template void StaggeredComputationKernel::Compute<false, O_8>();

template void StaggeredComputationKernel::Compute<false, O_12>();

template void StaggeredComputationKernel::Compute<false, O_16>();


template<bool IS_FORWARD_, HALF_LENGTH HALF_LENGTH_>
void StaggeredComputationKernel::Compute() {
    throw exceptions::NotImplementedException();
}
