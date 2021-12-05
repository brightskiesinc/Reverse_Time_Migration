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

#include "OmpSnapshot.hpp"

using namespace bs::timer::core::snapshots;


OmpSnapshot::OmpSnapshot(SnapshotTarget aSnapshotTarget) {}

OmpSnapshot::~OmpSnapshot() = default;

double
OmpSnapshot::Start() {
    auto start = omp_get_wtime();
    this->mStart = start;
    return this->mStart;
}

double
OmpSnapshot::End() {
    auto end = omp_get_wtime();
    this->mEnd = end;
    return this->mEnd;
}

double
OmpSnapshot::Resolve() {
    return this->mEnd - this->mStart;
}
