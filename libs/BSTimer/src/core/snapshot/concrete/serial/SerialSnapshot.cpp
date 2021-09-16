/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer.
 *
 * BS Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SerialSnapshot.hpp"

#include <bs/base/common/ExitCodes.hpp>

using namespace bs::timer::core::snapshot;


SerialSnapshot::SerialSnapshot(SnapshotTarget aSnapshotTarget) {}

SerialSnapshot::~SerialSnapshot() = default;

double
SerialSnapshot::Start() {
    auto start = std::chrono::high_resolution_clock::now();
    this->mStartPoint = start;

    /// @todo Check on status code
    return BS_BASE_RC_SUCCESS;
}

double
SerialSnapshot::End() {
    auto end = std::chrono::high_resolution_clock::now();
    this->mEndPoint = end;

    /// @todo Check on status code
    return BS_BASE_RC_SUCCESS;
}

double
SerialSnapshot::Resolve() {
    std::chrono::duration<double> runtime = this->mEndPoint - this->mStartPoint;
    return runtime.count();
}
