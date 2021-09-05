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

#include "operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp"

#include <bs/base/memory/MemoryManager.hpp>

using namespace std;
using namespace operations::components::addons;
using namespace operations::dataunits;
using namespace bs::base::memory;


Extension::Extension() = default;

Extension::~Extension() = default;

void Extension::SetHalfLength(uint aHalfLength) {
    this->mHalfLength = aHalfLength;
}

void Extension::SetBoundaryLength(uint aBoundaryLength) {
    this->mBoundaryLength = aBoundaryLength;
}

void Extension::SetGridBox(GridBox *apGridBox) {
    this->mpGridBox = apGridBox;
}

void Extension::SetProperty(float *property, float *window_property) {
    this->mProperties = property;
    this->mpWindowProperties = window_property;
}

void Extension::ExtendProperty() {


    int nx = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    /**
     * The nx , ny and nz includes the inner domain + BOUND_LENGTH + HALF_LENGTH in
     * all dimensions and we want to extend the velocities at boundaries only with
     * the HALF_LENGTH excluded
     */
    int start_x = mHalfLength;
    int start_y = mHalfLength;
    int start_z = mHalfLength;


    int end_x = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize() - mHalfLength;
    int end_y = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() - mHalfLength;
    int end_z = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize() - mHalfLength;

    /**
     * Change the values of velocities at
     * boundaries (HALF_LENGTH excluded) to zeros.
     */
    VelocityExtensionHelper(this->mProperties,
                            start_x, start_y, start_z,
                            end_x, end_y, end_z,
                            nx, ny, nz,
                            mBoundaryLength);
}

void Extension::ReExtendProperty() {
    /**
     * Re-Extend the velocities in case of window model.
    */


    int nx = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();
    /**
     * The window size is a struct containing the window nx, ny and nz
     * with the HALF_LENGTH and BOUND_LENGTH in all dimensions.
     */
    int wnx = mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    /**
     * The window size is a struct containing the window nx, ny and nz
     * with the HALF_LENGTH and BOUND_LENGTH in all dimensions.
     */

    if (mProperties == mpWindowProperties) {
        /// No window model, no need to re-extend so return from function

        /**
         * The nx, ny and nz includes the inner domain + BOUND_LENGTH +HALF_LENGTH
         * in all dimensions and we want to extend the velocities at boundaries only
         * with the HALF_LENGTH excluded
         */
        int start_x = mHalfLength;
        int start_y = mHalfLength;
        int start_z = mHalfLength;


        int end_x = mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - mHalfLength;
        int end_y = mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - mHalfLength;
        int end_z = mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - mHalfLength;
        /**
         * No window model, no need to re-extend.
         * Just re-extend the top boundary.
         */
        this->TopLayerExtensionHelper(this->mProperties,
                                      start_x, start_y, start_z,
                                      end_x, end_y, end_z,
                                      nx, ny, nz,
                                      mBoundaryLength);

        return;
    } else {
        /// Window model.

        /**
         * We want to work in velocities inside window but with the HALF_LENGTH
         * excluded in all dimensions to reach the bound_length so it is applied in
         * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH.
         */
        int start_x = mHalfLength;
        int start_y = mHalfLength;
        int start_z = mHalfLength;

        int end_x = mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - mHalfLength;
        int end_y = mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - mHalfLength;
        int end_z = mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - mHalfLength;

        /// Extend the velocities at boundaries by zeros
        this->VelocityExtensionHelper(this->mpWindowProperties,
                                      start_x, start_y, start_z,
                                      end_x, end_y, end_z,
                                      wnx, wny, wnz,
                                      mBoundaryLength);

    }
}

void Extension::AdjustPropertyForBackward() {

    int nx = mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int ny = mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int nz = mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    /**
     * The window size is a struct containing the window nx, ny, and nz
     * with the HALF_LENGTH and BOUND_LENGTH in all dimensions.
     */

    int wnx = mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    int wny = mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    int wnz = mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    /**
     * The window size is a struct containing the window nx, ny, and nz
     * with the HALF_LENGTH and BOUND_LENGTH in all dimensions.
     */

    /**
     * We want to work in velocities inside window but with the HALF_LENGTH
     * excluded in all dimensions to reach the bound_length so it is applied in
     * start points by adding HALF_LENGTH also at end by subtract HALF_LENGTH.
     */
    int start_x = mHalfLength;
    int start_y = mHalfLength;
    int start_z = mHalfLength;

    int end_x = mpGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() - mHalfLength;
    int end_y = mpGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() - mHalfLength;
    int end_z = mpGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() - mHalfLength;


    if (ny == 1) {
        end_y = 1;
        start_y = 0;
    }
    this->TopLayerRemoverHelper(this->mpWindowProperties,
                                start_x, start_y, start_z,
                                end_x, end_y, end_z,
                                wnx, wny, wnz,
                                mBoundaryLength);
}