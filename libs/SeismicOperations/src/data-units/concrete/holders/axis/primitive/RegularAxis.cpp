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

#include <operations/data-units/concrete/holders/axis/concrete/RegularAxis.hpp>

#include <bs/base/exceptions/Exceptions.hpp>

using namespace operations::dataunits::axis;
using namespace bs::base::exceptions;


template
class operations::dataunits::axis::RegularAxis<unsigned short int>;

template
class operations::dataunits::axis::RegularAxis<unsigned int>;

template
class operations::dataunits::axis::RegularAxis<unsigned long int>;

template RegularAxis<unsigned short int>::RegularAxis(unsigned short int);

template RegularAxis<unsigned int>::RegularAxis(unsigned int);

template RegularAxis<unsigned long int>::RegularAxis(unsigned long int);

template RegularAxis<unsigned short int>::RegularAxis(const RegularAxis &aRegularAxis);

template RegularAxis<unsigned int>::RegularAxis(const RegularAxis &aRegularAxis);

template RegularAxis<unsigned long int>::RegularAxis(const RegularAxis &aRegularAxis);

template RegularAxis<unsigned short int> &
RegularAxis<unsigned short int>::operator=(const RegularAxis<unsigned short int> &aRegularAxis);

template RegularAxis<unsigned int> &
RegularAxis<unsigned int>::operator=(const RegularAxis<unsigned int> &aRegularAxis);

template RegularAxis<unsigned long int> &
RegularAxis<unsigned long int>::operator=(const RegularAxis<unsigned long int> &aRegularAxis);

template int
RegularAxis<unsigned short int>::AddBoundary(int aDirection, unsigned short int);

template int
RegularAxis<unsigned int>::AddBoundary(int aDirection, unsigned int);

template int
RegularAxis<unsigned long int>::AddBoundary(int aDirection, unsigned long int);

template int
RegularAxis<unsigned short int>::AddHalfLengthPadding(int aDirection,
                                                      unsigned short int aHalfLengthPadding);

template int
RegularAxis<unsigned int>::AddHalfLengthPadding(int aDirection,
                                                unsigned int aHalfLengthPadding);

template int
RegularAxis<unsigned long int>::AddHalfLengthPadding(int aDirection,
                                                     unsigned long int aHalfLengthPadding);

template int
RegularAxis<unsigned short int>::AddComputationalPadding(int aDirection,
                                                         unsigned short aComputationalPadding);

template int
RegularAxis<unsigned int>::AddComputationalPadding(int aDirection, unsigned int aComputationalPadding);

template int
RegularAxis<unsigned long int>::AddComputationalPadding(int aDirection,
                                                        unsigned long int aComputationalPadding);

template unsigned short int
RegularAxis<unsigned short int>::GetLogicalAxisSize();

template unsigned int
RegularAxis<unsigned int>::GetLogicalAxisSize();

template unsigned long int
RegularAxis<unsigned long int>::GetLogicalAxisSize();

template unsigned short int
RegularAxis<unsigned short int>::GetActualAxisSize();

template unsigned int
RegularAxis<unsigned int>::GetActualAxisSize();

template unsigned long int
RegularAxis<unsigned long int>::GetActualAxisSize();

template unsigned short int
RegularAxis<unsigned short int>::GetComputationAxisSize();

template unsigned int
RegularAxis<unsigned int>::GetComputationAxisSize();

template unsigned long int
RegularAxis<unsigned long int>::GetComputationAxisSize();


template<typename T>
RegularAxis<T>::RegularAxis(T mAxisSize) :
        mAxisSize(mAxisSize),
        mFrontHalfLengthPadding(0),
        mRearHalfLengthPadding(0),
        mFrontBoundaryLength(0),
        mRearBoundaryLength(0),
        mFrontComputationalPadding(0),
        mRearComputationalPadding(0),
        mCellDimension(0),
        mReferencePoint(0) {}

template<typename T>
RegularAxis<T>::RegularAxis(const RegularAxis<T> &aRegularAxis):
        mAxisSize(aRegularAxis.GetAxisSize()),
        mFrontHalfLengthPadding(aRegularAxis.GetFrontHalfLengthPadding()),
        mRearHalfLengthPadding(aRegularAxis.GetRearHalfLengthPadding()),
        mFrontBoundaryLength(aRegularAxis.GetFrontBoundaryLength()),
        mRearBoundaryLength(aRegularAxis.GetRearBoundaryLength()),
        mFrontComputationalPadding(aRegularAxis.GetFrontComputationalPadding()),
        mRearComputationalPadding(aRegularAxis.GetRearComputationalPadding()),
        mReferencePoint(aRegularAxis.GetReferencePoint()),
        mCellDimension(aRegularAxis.GetCellDimension()) {}

template<typename T>
RegularAxis<T> &RegularAxis<T>::operator=(const RegularAxis<T> &aRegularAxis) {
    if (&aRegularAxis != this) {
        this->mAxisSize = aRegularAxis.GetAxisSize();
        this->mFrontHalfLengthPadding = aRegularAxis.GetFrontHalfLengthPadding();
        this->mRearHalfLengthPadding = aRegularAxis.GetRearHalfLengthPadding();
        this->mFrontBoundaryLength = aRegularAxis.mFrontBoundaryLength;
        this->mRearBoundaryLength = aRegularAxis.mRearBoundaryLength;
        this->mFrontComputationalPadding = aRegularAxis.GetFrontComputationalPadding();
        this->mRearComputationalPadding = aRegularAxis.GetRearComputationalPadding();
        this->mCellDimension = aRegularAxis.GetCellDimension();
        this->mReferencePoint = aRegularAxis.GetReferencePoint();

    }
    return *this;
}

template<typename T>
int RegularAxis<T>::AddBoundary(int aDirection, T aBoundaryLength) {
    if (is_direction_out_of_range(aDirection)) {
        throw DIRECTION_EXCEPTION();
    }

    if (aDirection == OP_DIREC_FRONT) {
        this->mFrontBoundaryLength = aBoundaryLength;
    } else if (aDirection == OP_DIREC_REAR) {
        this->mRearBoundaryLength = aBoundaryLength;
    } else if (aDirection == OP_DIREC_BOTH) {
        this->mFrontBoundaryLength = aBoundaryLength;
        this->mRearBoundaryLength = aBoundaryLength;
    }
    return 1;
}

template<typename T>
int RegularAxis<T>::AddHalfLengthPadding(int aDirection, T aHalfLengthPadding) {
    if (is_direction_out_of_range(aDirection)) {
        throw DIRECTION_EXCEPTION();
    }

    if (aDirection == OP_DIREC_FRONT) {
        this->mFrontHalfLengthPadding = aHalfLengthPadding;
    } else if (aDirection == OP_DIREC_REAR) {
        this->mRearHalfLengthPadding = aHalfLengthPadding;
    } else if (aDirection == OP_DIREC_BOTH) {
        this->mFrontHalfLengthPadding = aHalfLengthPadding;
        this->mRearHalfLengthPadding = aHalfLengthPadding;
    }
    return 1;
}


template<typename T>
int RegularAxis<T>::AddComputationalPadding(int aDirection, T aComputationalPadding) {
    if (is_direction_out_of_range(aDirection)) {
        throw DIRECTION_EXCEPTION();
    }

    if (aDirection == OP_DIREC_FRONT) {
        this->mFrontComputationalPadding = aComputationalPadding;
    } else if (aDirection == OP_DIREC_REAR) {
        this->mRearComputationalPadding = aComputationalPadding;
    } else if (aDirection == OP_DIREC_BOTH) {
        this->mFrontComputationalPadding = aComputationalPadding;
        this->mRearComputationalPadding = aComputationalPadding;
    }
    return 1;
}

template<typename T>
T RegularAxis<T>::GetLogicalAxisSize() {
    return this->mAxisSize +
           this->mFrontBoundaryLength + this->mRearBoundaryLength +
           this->mFrontHalfLengthPadding + this->mRearHalfLengthPadding;
}

template<typename T>
T RegularAxis<T>::GetActualAxisSize() {
    return this->mAxisSize +
           this->mFrontBoundaryLength + this->mRearBoundaryLength +
           this->mFrontHalfLengthPadding + this->mRearHalfLengthPadding +
           this->mFrontComputationalPadding + this->mRearComputationalPadding;
}

template<typename T>
T RegularAxis<T>::GetComputationAxisSize() {
    return this->mAxisSize +
           this->mFrontBoundaryLength + this->mRearBoundaryLength +
           this->mFrontComputationalPadding + this->mRearComputationalPadding;
}
