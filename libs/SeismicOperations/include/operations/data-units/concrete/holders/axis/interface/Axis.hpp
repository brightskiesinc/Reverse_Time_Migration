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

#ifndef OPERATIONS_LIB_DATA_UNITS_AXIS_AXIS_HPP
#define OPERATIONS_LIB_DATA_UNITS_AXIS_AXIS_HPP

#include <operations/common/DataTypes.h>

namespace operations {
    namespace dataunits {
        namespace axis {

            /**
             * @brief An abstract class to hold the meta-data of each axis.
             * Will ease further implementations and development.
             * This is a template class, data types can be unsigned short, unsigned int or unsigned long.
             */
            template<typename T>
            class Axis {
            public:

                /**
                 * @brief Function to add boundary to the axis front or rear.
                 *
                 * @param[in] aDirection
                 * The direction where to add the boundary. (OP_DIREC_FRONT or OP_DIREC_REAR)
                 *
                 * @param[in] aBoundaryLength
                 * The length of the boundary to be added.
                 *
                 * @return
                 * int status flag.
                 */
                virtual int
                AddBoundary(int aDirection, T aBoundaryLength) = 0;

                /**
                 * @brief Function to add half length to the axis.
                 * The same value must be added at both ends.
                 *
                 * @param[in] aHalfLengthPadding
                 * The half length to be added to the axis.
                 *
                 * @return
                 * int status flag.
                 */
                virtual int
                AddHalfLengthPadding(int aDirection, T aHalfLengthPadding) = 0;

                /**
                 * @brief Function to add padding to the axis front or rear.
                 *  Padding differs from one technology to another.
                 *
                 * @param[in] aDirection
                 * The direction where to add the padding. (OP_DIREC_FRONT or OP_DIREC_REAR)
                 *
                 * @param[in] aComputationalPadding
                 * The length of padding to be added.
                 *
                 * @return
                 * int status flag.
                 */
                virtual int
                AddComputationalPadding(int aDirection, T aComputationalPadding) = 0;

                /**
                 * @brief Getter for the Front Half Length/ Non Computational Padding
                 *
                 * @return
                 * int, The front Half Length/ Non Computational Padding
                 */
                virtual int
                GetFrontHalfLengthPadding() const = 0;

                /**
                 * @brief Getter for the Rear Half Length/ Non Computational Padding
                 *
                 * @return
                 * int, The rear Half Length/ Non Computational Padding
                 */
                virtual int
                GetRearHalfLengthPadding() const = 0;

                /**
                 * @brief Getter for the Rear Boundary Length.
                 *
                 * @return
                 * T, The Rear Boundary Length.
                 */
                virtual T
                GetRearBoundaryLength() const = 0;

                /**
                 * @brief Getter for the Front Boundary Length.
                 *
                 * @return
                 * T, The Front Boundary Length.
                 */
                virtual T
                GetFrontBoundaryLength() const = 0;

                /**
                 * @brief Getter for the Rear Computational Padding Length.
                 *
                 * @return
                 * T, The Rear Computational Padding Length.
                 */
                virtual T
                GetRearComputationalPadding() const = 0;

                /**
                 * @brief Getter for the Front Computational Padding Length.
                 *
                 * @return
                 * T, The Front Computational Padding Length.
                 */
                virtual T
                GetFrontComputationalPadding() const = 0;

                /**
                 * @brief Setter for the Cell Dimension without any additions.
                 *
                 * @Param[in] aCellDimension
                 * T, The cell dimension.
                 */

                virtual void
                SetCellDimension(float aCellDimension) = 0;

                /**
                 * @brief Getter for the Cell Dimension without any additions.
                 *
                 * @return
                 * T, The axis size.
                 */
                virtual float
                GetCellDimension() const = 0;

                /**
                 * @brief Setter for the Reference Point.
                 *
                 * @Param[in] aReferencePoint
                 * T, The Reference Point.
                 */
                virtual void
                SetReferencePoint(T aReferencePoint) = 0;

                /**
                 * @brief Getter for the Reference point without any additions.
                 *
                 * @return
                 * T, The axis size.
                 */
                virtual T
                GetReferencePoint() const = 0;


                /**
                 * @brief Getter for the original Axis Size without any additions.
                 *
                 * @return
                 * T, The axis size.
                 */
                virtual T
                GetAxisSize() const = 0;

                /**
                 * @brief Getter for the Logical Axis Size.
                 *  Logical Axis size = Axis size +
                 *                      Front Boundary + Rear Boundary +
                 *                      2 * Half Length
                 *
                 * @return
                 * T, The Logical axis size.
                 */
                virtual T
                GetLogicalAxisSize() = 0;

                /**
                 * @brief Getter for the Actual Axis Size.
                 *  Actual Axis size = Axis size +
                 *                      Front Boundary Length + Rear Boundary Length +
                 *                      2 * Half Length +
                 *                      Front Padding + Rear Padding
                 *
                 * @return
                 * T, The Actual axis size.
                 */
                virtual T
                GetActualAxisSize() = 0;

                /**
                 * @brief Getter for the Computation Axis Size.
                 *  Computation Axis size = Axis size +
                 *                      Front Boundary Length + Rear Boundary Length +
                 *                      Front Padding + Rear Padding
                 *
                 * @return
                 * T, The Computation axis size.
                 */
                virtual T
                GetComputationAxisSize() = 0;
            };
        }
    }
}


#endif //OPERATIONS_LIB_DATA_UNITS_AXIS_AXIS_HPP
