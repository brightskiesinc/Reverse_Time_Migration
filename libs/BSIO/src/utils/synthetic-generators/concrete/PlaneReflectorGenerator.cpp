/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/utils/synthetic-generators/concrete/PlaneReflectorGenerator.hpp>

using namespace bs::io::generators;


PlaneReflectorGenerator::PlaneReflectorGenerator()
        : mValueBefore(0),
          mValueAfter(0),
          mOriginX(0),
          mOriginY(0),
          mOriginZ(0),
          mXSlopeAngle(0),
          mYSlopeAngle(0) {}

PlaneReflectorGenerator::~PlaneReflectorGenerator() = default;

void
PlaneReflectorGenerator::ParseValues(nlohmann::json &aJsonNode) {
    this->mValueBefore = aJsonNode["value"]["before"];
    this->mValueAfter = aJsonNode["value"]["after"];
    this->mOriginX = aJsonNode["origin"]["x-index"];
    this->mOriginY = aJsonNode["origin"]["y-index"];
    this->mOriginZ = aJsonNode["origin"]["z-index"];
    this->mXSlopeAngle = aJsonNode["slope"]["x-angle"];
    this->mYSlopeAngle = aJsonNode["slope"]["y-angle"];
}

float
PlaneReflectorGenerator::GetReflectorDepth(float x, float y) {
    return tanf(this->mXSlopeAngle * M_PI / 180) * (x - this->mOriginX)
           + tanf(this->mYSlopeAngle * M_PI / 180) * (y - this->mOriginY)
           + this->mOriginZ;
}

float
PlaneReflectorGenerator::GetAfterValue() {
    return this->mValueAfter;
}

float PlaneReflectorGenerator::GetBeforeValue() {
    return this->mValueBefore;
}
