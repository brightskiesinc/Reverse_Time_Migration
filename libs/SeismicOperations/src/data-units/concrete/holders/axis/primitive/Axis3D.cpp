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

#include <operations/data-units/concrete/holders/axis/concrete/Axis3D.hpp>

using namespace operations::dataunits::axis;


template
class operations::dataunits::axis::Axis3D<unsigned short int>;

template
class operations::dataunits::axis::Axis3D<unsigned int>;

template
class operations::dataunits::axis::Axis3D<unsigned long int>;

template
Axis3D<unsigned short int>::Axis3D(unsigned short int aXSize, unsigned short int aYSize, unsigned short int aZSize);

template
Axis3D<unsigned int>::Axis3D(unsigned int aXSize, unsigned int aYSize, unsigned int aZSize);

template
Axis3D<unsigned long int>::Axis3D(unsigned long int aXSize, unsigned long int aYSize, unsigned long int aZSize);

template
Axis3D<unsigned short int>::Axis3D(Axis3D &aAxis3D);

template
Axis3D<unsigned int>::Axis3D(Axis3D &aAxis3D);

template
Axis3D<unsigned long int>::Axis3D(Axis3D &aAxis3D);

template
Axis3D<unsigned short int> &Axis3D<unsigned short int>::operator=(Axis3D<unsigned short int> &aAxis3D);

template
Axis3D<unsigned int> &Axis3D<unsigned int>::operator=(Axis3D<unsigned int> &aAxis3D);

template
Axis3D<unsigned long int> &Axis3D<unsigned long int>::operator=(Axis3D<unsigned long int> &aAxis3D);

template
Axis3D<unsigned short int>::~Axis3D();

template
Axis3D<unsigned int>::~Axis3D();

template
Axis3D<unsigned long int>::~Axis3D();


template<typename T>
Axis3D<T>::Axis3D(T aXSize, T aYSize, T aZSize) {
    this->mXAxis = RegularAxis<T>(aXSize);
    this->mYAxis = RegularAxis<T>(aYSize);
    this->mZAxis = RegularAxis<T>(aZSize);
    this->mAxisVector.push_back(&this->mXAxis);
    this->mAxisVector.push_back(&this->mYAxis);
    this->mAxisVector.push_back(&this->mZAxis);
}

template<typename T>
Axis3D<T>::Axis3D(Axis3D &aAxis3D) {
    this->mXAxis = RegularAxis<T>(aAxis3D.GetXAxis());
    this->mYAxis = RegularAxis<T>(aAxis3D.GetYAxis());
    this->mZAxis = RegularAxis<T>(aAxis3D.GetZAxis());
    this->mAxisVector.push_back(&this->mXAxis);
    this->mAxisVector.push_back(&this->mYAxis);
    this->mAxisVector.push_back(&this->mZAxis);
}

template<typename T>
Axis3D<T> &Axis3D<T>::operator=(Axis3D<T> &aAxis3D) {
    if (&aAxis3D != this) {
        this->mXAxis = RegularAxis<T>(aAxis3D.GetXAxis());
        this->mYAxis = RegularAxis<T>(aAxis3D.GetYAxis());
        this->mZAxis = RegularAxis<T>(aAxis3D.GetZAxis());
        this->mAxisVector.push_back(&this->mXAxis);
        this->mAxisVector.push_back(&this->mYAxis);
        this->mAxisVector.push_back(&this->mZAxis);
    }
    return *this;
}

template<typename T>
Axis3D<T>::~Axis3D() {
    this->mAxisVector.clear();
}
