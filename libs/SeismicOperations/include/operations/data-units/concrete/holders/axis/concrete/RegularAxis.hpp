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

#ifndef OPERATIONS_LIB_DATA_UNITS_AXIS_REGULAR_AXIS_HPP
#define OPERATIONS_LIB_DATA_UNITS_AXIS_REGULAR_AXIS_HPP

#include <operations/data-units/concrete/holders/axis/interface/Axis.hpp>


namespace operations {
    namespace dataunits {
        namespace axis {

            /**
             * @brief A Class to hold the metadata of an axis.
             * It uses regular sampling.
             * Template class, allowed data types are unsigned short, unsigned int, unsigned long.
             */
            template<typename T>
            class RegularAxis : public Axis<T> {
            public:
                /**
                 * @brief Default constructor.
                 *
                 * @note This should not be modified or deleted as it is needed
                 * for explicit instantiation without pointers.
                 */
                RegularAxis() = default;

                /**
                 * @brief explicit Default constructor.
                 */
                explicit RegularAxis(T mAxisSize);

                /**
                 * @brief Copy constructor.
                 */
                RegularAxis(const RegularAxis &aRegularAxis);

                /**
                 * @brief Default destructor.
                 */
                ~RegularAxis() = default;

                /**
                  * @brief Overloading the assignment operator
                  */
                RegularAxis<T> &operator=(const RegularAxis<T> &aRegularAxis);

                int
                AddBoundary(int aDirection, T aBoundaryLength) override;

                int
                AddHalfLengthPadding(int aDirection, T aHalfLengthPadding) override;

                int
                AddComputationalPadding(int aDirection, T aComputationalPadding) override;

                inline int
                GetRearHalfLengthPadding() const override { return this->mRearHalfLengthPadding; };

                inline int
                GetFrontHalfLengthPadding() const override { return this->mFrontHalfLengthPadding; };

                inline T
                GetRearBoundaryLength() const override { return this->mRearBoundaryLength; };

                inline T
                GetFrontBoundaryLength() const override { return this->mFrontBoundaryLength; };

                inline T
                GetRearComputationalPadding() const override { return this->mRearComputationalPadding; };

                inline T
                GetFrontComputationalPadding() const override { return this->mFrontComputationalPadding; };

                inline T
                GetAxisSize() const override { return this->mAxisSize; }

                inline void
                SetCellDimension(float aCellDimension) override { this->mCellDimension = aCellDimension; }

                inline float
                GetCellDimension() const override { return this->mCellDimension; }

                inline void SetReferencePoint(T aReferencePoint) { this->mReferencePoint = aReferencePoint; }

                inline T
                GetReferencePoint() const override { return this->mReferencePoint; }

                T
                GetLogicalAxisSize() override;

                T
                GetActualAxisSize() override;

                T
                GetComputationAxisSize() override;

            private:
                T mAxisSize;

                int mFrontHalfLengthPadding;
                int mRearHalfLengthPadding;

                T mFrontBoundaryLength;
                T mRearBoundaryLength;

                T mFrontComputationalPadding;
                T mRearComputationalPadding;

                float mCellDimension;
                T mReferencePoint;
            };
        }
    }
}
#endif //OPERATIONS_LIB_DATA_UNITS_AXIS_REGULAR_AXIS_HPP
