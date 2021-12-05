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

#include <bs/timer/core/snapshots/helpers/GenericSnapshot.hpp>

#include "../concrete/serial/SerialSnapshot.hpp"

#ifdef USE_OMP

#include "../concrete/omp/OmpSnapshot.hpp"

#endif

using namespace bs::timer::core::snapshots;


GenericSnapshot::GenericSnapshot(SnapshotTarget aSnapshotTarget) {
#ifdef USE_OMP
    this->mpSnapshot = new OmpSnapshot(aSnapshotTarget);
#else
    this->mpSnapshot = new SerialSnapshot(aSnapshotTarget);
#endif
}

GenericSnapshot::~GenericSnapshot() {
    delete this->mpSnapshot;
}

double
GenericSnapshot::Start() {
    return this->mpSnapshot->Start();
}

double
GenericSnapshot::End() {
    return this->mpSnapshot->End();
}

double
GenericSnapshot::Resolve() {
    return this->mpSnapshot->Resolve();
}
