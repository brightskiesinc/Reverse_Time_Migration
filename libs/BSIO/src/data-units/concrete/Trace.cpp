/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/data-units/concrete/Trace.hpp>

using namespace bs::io::dataunits;


Trace::Trace(const unsigned short aNS) {
    this->mTraceHeaderMap[TraceHeaderKey(TraceHeaderKey::NS)] = std::to_string(aNS);
}

Trace::Trace(Trace &&aTrace) noexcept
        : mpTraceData(std::move(aTrace.mpTraceData)) {}

Trace::Trace(const Trace &aTrace) {}

Trace::~Trace() = default;

Trace &Trace::operator=(Trace &&aTrace) noexcept {
    this->mTraceHeaderMap = aTrace.mTraceHeaderMap;
    this->mpTraceData = std::move(aTrace.mpTraceData);
    return *this;
}

void Trace::SetScaledCoordinateHeader(TraceHeaderKey aKey, float aLocation) {
    float scale_coordinate = 0;
    if (this->HasTraceHeader(TraceHeaderKey::SCALCO)) {
        scale_coordinate = this->GetTraceHeaderKeyValue<int16_t>(TraceHeaderKey::SCALCO);
    }
    if (scale_coordinate == 0) {
        scale_coordinate = 1;
    }
    uint32_t val;
    if (scale_coordinate > 0) {
        val = (uint32_t) (aLocation / scale_coordinate);
    } else {
        scale_coordinate *= -1;
        val = (uint32_t) (aLocation * scale_coordinate);
    }
    this->SetTraceHeaderKeyValue(aKey, val);
}

float Trace::GetScaledCoordinateHeader(TraceHeaderKey aKey) {
    float scale_coordinate = 0;
    if (this->HasTraceHeader(TraceHeaderKey::SCALCO)) {
        scale_coordinate = this->GetTraceHeaderKeyValue<int16_t>(TraceHeaderKey::SCALCO);
    }
    if (scale_coordinate == 0) {
        scale_coordinate = 1;
    }
    float val;
    if (scale_coordinate > 0) {
        val = (float) this->GetTraceHeaderKeyValue<uint32_t>(aKey) * scale_coordinate;
    } else {
        scale_coordinate *= -1;
        val = (float) this->GetTraceHeaderKeyValue<uint32_t>(aKey) / scale_coordinate;
    }
    return val;
}
