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

#include <operations/components/independents/concrete/forward-collectors/boundary-saver/BoundarySaver.h>

using namespace operations::components::helpers;
using namespace operations::common;
using namespace operations::dataunits;

BoundarySaver::BoundarySaver() : mKey(0), mBoundarySize(0),
                                 mpMainGridBox(nullptr),
                                 mpInternalGridBox(nullptr),
                                 mpComputationParameters(nullptr) {

}

void BoundarySaver::Initialize(dataunits::GridBox::Key aActiveKey,
                               dataunits::GridBox *apInternalGridBox,
                               dataunits::GridBox *apMainGridBox,
                               common::ComputationParameters *apParameters) {
    this->mKey = aActiveKey;
    this->mpInternalGridBox = apInternalGridBox;
    this->mpComputationParameters = apParameters;
    this->mpMainGridBox = apMainGridBox;
    uint half_length = this->mpComputationParameters->GetHalfLength();

    uint nxi = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetAxisSize();
    uint nyi = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetAxisSize();
    uint nzi = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetAxisSize();

    this->mBoundarySize =
            nxi * nyi * half_length * 2 + nzi * nyi * half_length * 2;
    if (nyi != 1) {
        this->mBoundarySize += nxi * nzi * half_length * 2;
    }
    this->mBackupBoundaries.Allocate(
            this->mBoundarySize * (this->mpMainGridBox->GetNT() + 1),
            "Backup Boundaries");
}

BoundarySaver::~BoundarySaver() {
    this->mBackupBoundaries.Free();
}
