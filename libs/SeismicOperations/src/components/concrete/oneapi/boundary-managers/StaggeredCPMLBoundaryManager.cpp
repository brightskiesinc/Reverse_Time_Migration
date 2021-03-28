//
// Created by amr on 03/01/2021.
//
#include <operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp>

#include <operations/exceptions/NotImplementedException.h>

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


void StaggeredCPMLBoundaryManager::ApplyBoundary(uint kernel_id) {
    throw exceptions::NotImplementedException();
}

void StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
        float *coeff_a, float *coeff_b, int boundary_length, float dh, float dt,
        float max_vel, float shift_ratio, float reflect_coeff, float relax_cp) {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}

// this function used to reset the auxiliary variables to zero
void StaggeredCPMLBoundaryManager::ZeroAuxiliaryVariables() {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}
