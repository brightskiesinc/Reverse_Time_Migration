//
// Created by amr-nasr on 18/11/2019.
//

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>

#include <operations/components/independents/concrete/boundary-managers/extensions/HomogenousExtension.hpp>
#include <timer/Timer.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>


#ifndef PWR2
#define PWR2(EXP) ((EXP) * (EXP))
#endif

#define fma(a, b, c) (((a) * (b)) + (c))

using namespace operations::components;
using namespace operations::dataunits;
using namespace operations::common;


template void CPMLBoundaryManager::FillCPMLCoefficients<1>();

template void CPMLBoundaryManager::FillCPMLCoefficients<2>();

template void CPMLBoundaryManager::FillCPMLCoefficients<3>();

template void CPMLBoundaryManager::ApplyAllCPML<O_2>();

template void CPMLBoundaryManager::ApplyAllCPML<O_4>();

template void CPMLBoundaryManager::ApplyAllCPML<O_8>();

template void CPMLBoundaryManager::ApplyAllCPML<O_12>();

template void CPMLBoundaryManager::ApplyAllCPML<O_16>();

template<int HALF_LENGTH_>
void CPMLBoundaryManager::ApplyAllCPML() {
    throw exceptions::NotImplementedException();
}

template<int DIRECTION_>
void CPMLBoundaryManager::FillCPMLCoefficients() {
    throw exceptions::NotImplementedException();
}

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateFirstAuxiliary() {
    throw exceptions::NotImplementedException();
}

template<int direction, bool OPPOSITE_, int half_length>
void CPMLBoundaryManager::CalculateCPMLValue() {
    throw exceptions::NotImplementedException();
}
