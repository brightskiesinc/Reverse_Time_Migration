//
// Created by amr-nasr on 18/11/2019.
//

#include <operations/components/independents/concrete/boundary-managers/CPMLBoundaryManager.hpp>

#include <operations/exceptions/Exceptions.h>

using namespace operations::components;


template<int HALF_LENGTH_>
void CPMLBoundaryManager::ApplyAllCPML() {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}

// make them private to the class , extend to th 3d
template<int DIRECTION_>
void CPMLBoundaryManager::FillCPMLCoefficients() {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateFirstAuxiliary() {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}

template<int DIRECTION_, bool OPPOSITE_, int HALF_LENGTH_>
void CPMLBoundaryManager::CalculateCPMLValue() {
    /// @todo
    /// {
    throw exceptions::NotImplementedException();
    /// }
}
