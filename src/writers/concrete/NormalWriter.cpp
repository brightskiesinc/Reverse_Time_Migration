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

#include <stbx/writers/concrete/NormalWriter.hpp>

using namespace std;
using namespace stbx::writers;
using namespace operations::utils::filters;
using namespace operations::utils::io;

void
NormalWriter::SpecifyRawMigration() {
    mRawMigration = mpMigrationData->GetResultAt(0)->GetData();
}

void
NormalWriter::PostProcess() {

}
