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

#ifndef OPERATIONS_LIB_DATA_UNITS_AXIS_3D_AXIS_HPP
#define OPERATIONS_LIB_DATA_UNITS_AXIS_3D_AXIS_HPP

#include <operations/data-units/concrete/holders/axis/concrete/RegularAxis.hpp>

#include <vector>

namespace operations {
    namespace dataunits {
        namespace axis {

            /**
             * @brief An wrapper class to hold the X, Y and Z axis.
             *
             */
            template<typename T>
            class Axis3D {
            public:
                /**
                 * @brief Constructor.
                 */
                Axis3D(T aXSize, T aYSize, T aZSize);

                /**
                 * @brief Copy constructor.
                 */
                Axis3D(Axis3D &aAxis3D);

                /**
                 * @brief Overloading Assignment Operator.
                 */
                Axis3D<T> &operator=(Axis3D<T> &aAxis3D);

                /**
                 * @brief Destructor.
                 */
                ~Axis3D();

                /**
                 * @brief Getter for the X axis.
                 *
                 * @return
                 * RegularAxis, The X Axis..
                 */
                inline RegularAxis<T> &
                GetXAxis() { return this->mXAxis; }

                /**
                 * @brief Getter for the Z Length.
                 *
                 * @return
                 * RegularAxis, The Z Length.
                 */
                inline RegularAxis<T> &
                GetZAxis() { return this->mZAxis; }

                /**
                 * @brief Getter for the Y axis.
                 *
                 * @return
                 * RegularAxis, The Y Length.
                 */
                inline RegularAxis<T> &
                GetYAxis() { return this->mYAxis; }

                /**
                 * @brief Setter for the X axis without any additions.
                 *
                 * @Param[in] axis size
                 *
                 */
                inline void SetXAxis(T aXAxis) { this->mXAxis = RegularAxis<T>(aXAxis); }

                /**
                 * @brief Setter for the Y axis without any additions.
                 *
                 * @Param[in] axis size
                 *
                 */
                inline void SetYAxis(T aYAxis) { this->mYAxis = RegularAxis<T>(aYAxis); }

                /**
                 * @brief Setter for the Z axis without any additions.
                 *
                 * @Param[in] axis size
                 *
                 */
                inline void SetZAxis(T aZAxis) { this->mZAxis = RegularAxis<T>(aZAxis); }

                /**
                 * @brief Getter for the vector of axis.
                 *
                 * @return
                 * std::vector, vector of RegularAxis.
                 */
                inline std::vector<RegularAxis<T> *>
                GetAxisVector() { return this->mAxisVector; }

            private:
                /// Axis for X direction.
                RegularAxis<T> mXAxis;
                /// Axis for Y direction.
                RegularAxis<T> mYAxis;
                /// Axis for Z direction.
                RegularAxis<T> mZAxis;
                /// All directions axes vector.
                std::vector<RegularAxis<T> *> mAxisVector;
            };
        }
    }
}


#endif //OPERATIONS_LIB_DATA_UNITS_AXIS_3D_AXIS_HPP
