//
// Created by amr-nasr on 11/21/19.
//

#include "operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp"

#include "operations/components/independents/concrete/boundary-managers/StaggeredCPMLBoundaryManager.hpp"
#include <timer/Timer.h>

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>

#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c) (((a) * (b)) + (c)

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;

void StaggeredCPMLBoundaryManager::ApplyBoundary(uint kernel_id) {
    throw exceptions::NotImplementedException();
}

void StaggeredCPMLBoundaryManager::FillCPMLCoefficients(
        float *coeff_a, float *coeff_b, int boundary_length, float dh, float dt,
        float max_vel, float shift_ratio, float reflect_coeff, float relax_cp) {
    throw exceptions::NotImplementedException();
}

void StaggeredCPMLBoundaryManager::ZeroAuxiliaryVariables() {
    throw exceptions::NotImplementedException();
}
