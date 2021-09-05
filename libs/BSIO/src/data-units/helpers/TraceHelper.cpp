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

#include <bs/io/data-units/helpers/TraceHelper.hpp>

#include <bs/base/common/ExitCodes.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>

#include <cmath>

using namespace bs::io::dataunits;
using namespace bs::io::dataunits::helpers;
using namespace bs::io::lookups;
using namespace bs::io::utils::convertors;


int
TraceHelper::Weight(Trace *&apTrace,
                    TraceHeaderLookup &aTraceHeaderLookup,
                    BinaryHeaderLookup &aBinaryHeaderLookup) {
    return TraceHelper::WeightData(apTrace, aTraceHeaderLookup, aBinaryHeaderLookup);
}

int
TraceHelper::WeightData(Trace *&apTrace,
                        TraceHeaderLookup &aTraceHeaderLookup,
                        BinaryHeaderLookup &aBinaryHeaderLookup) {
    auto format = NumbersConvertor::ToLittleEndian(aBinaryHeaderLookup.FORMAT);
    /* Scale data. */
    if (!(format == 1 || format == 5)) {
        auto trwf = NumbersConvertor::ToLittleEndian(aTraceHeaderLookup.TRWF);
        if (trwf != 0) {
            float scale = std::pow(2.0, -trwf);
            auto data = apTrace->GetTraceData();
            for (int i = 0; i < apTrace->GetNumberOfSamples(); ++i) {
                data[i] *= scale;
            }
        }
        trwf = 0;
        aTraceHeaderLookup.TRWF = NumbersConvertor::ToLittleEndian(trwf);
    }
    return BS_BASE_RC_SUCCESS;
}
