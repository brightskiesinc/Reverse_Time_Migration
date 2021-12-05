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

#include <bs/io/lookups/SeismicFilesHeaders.hpp>

using namespace bs::io::lookups;
using namespace bs::io::dataunits;
using namespace bs::io::utils::range;


const std::map<TraceHeaderKey, EntryRange> SeismicFilesHeaders::mTraceHeadersMap = {
        {TraceHeaderKey::TRACL,            EntryRange(EntryRange::Type::UINT, 0)},
        {TraceHeaderKey::TRACR,            EntryRange(EntryRange::Type::UINT, 4)},
        {TraceHeaderKey::FLDR,             EntryRange(EntryRange::Type::UINT, 8)},
        {TraceHeaderKey::TRACF,            EntryRange(EntryRange::Type::UINT, 12)},
        {TraceHeaderKey::EP,               EntryRange(EntryRange::Type::UINT, 16)},
        {TraceHeaderKey::CDP,              EntryRange(EntryRange::Type::UINT, 20)},
        {TraceHeaderKey::CDPT,             EntryRange(EntryRange::Type::UINT, 24)},
        {TraceHeaderKey::TRID,             EntryRange(EntryRange::Type::SHORT, 28)},
        {TraceHeaderKey::NVS,              EntryRange(EntryRange::Type::SHORT, 30)},
        {TraceHeaderKey::NHS,              EntryRange(EntryRange::Type::SHORT, 32)},
        {TraceHeaderKey::DUSE,             EntryRange(EntryRange::Type::SHORT, 34)},
        {TraceHeaderKey::OFFSET,           EntryRange(EntryRange::Type::UINT, 36)},
        {TraceHeaderKey::GELEV,            EntryRange(EntryRange::Type::UINT, 40)},
        {TraceHeaderKey::SELEV,            EntryRange(EntryRange::Type::UINT, 44)},
        {TraceHeaderKey::SDEPTH,           EntryRange(EntryRange::Type::UINT, 48)},
        {TraceHeaderKey::GDEL,             EntryRange(EntryRange::Type::UINT, 52)},
        {TraceHeaderKey::SDEL,             EntryRange(EntryRange::Type::UINT, 56)},
        {TraceHeaderKey::SWDEP,            EntryRange(EntryRange::Type::UINT, 60)},
        {TraceHeaderKey::GWDEP,            EntryRange(EntryRange::Type::UINT, 64)},
        {TraceHeaderKey::SCALEL,           EntryRange(EntryRange::Type::SHORT, 68)},
        {TraceHeaderKey::SCALCO,           EntryRange(EntryRange::Type::SHORT, 70)},
        {TraceHeaderKey::SX,               EntryRange(EntryRange::Type::UINT, 72)},
        {TraceHeaderKey::SY,               EntryRange(EntryRange::Type::UINT, 76)},
        {TraceHeaderKey::GX,               EntryRange(EntryRange::Type::UINT, 80)},
        {TraceHeaderKey::GY,               EntryRange(EntryRange::Type::UINT, 84)},
        {TraceHeaderKey::COINTIT,          EntryRange(EntryRange::Type::SHORT, 88)},
        {TraceHeaderKey::WEVEL,            EntryRange(EntryRange::Type::SHORT, 90)},
        {TraceHeaderKey::SWEVEL,           EntryRange(EntryRange::Type::SHORT, 92)},
        {TraceHeaderKey::SUT,              EntryRange(EntryRange::Type::SHORT, 94)},
        {TraceHeaderKey::GUT,              EntryRange(EntryRange::Type::SHORT, 96)},
        {TraceHeaderKey::SSTAT,            EntryRange(EntryRange::Type::SHORT, 98)},
        {TraceHeaderKey::GSTAT,            EntryRange(EntryRange::Type::SHORT, 100)},
        {TraceHeaderKey::TSTAT,            EntryRange(EntryRange::Type::SHORT, 102)},
        {TraceHeaderKey::LAGA,             EntryRange(EntryRange::Type::SHORT, 104)},
        {TraceHeaderKey::LAGB,             EntryRange(EntryRange::Type::SHORT, 106)},
        {TraceHeaderKey::DELRT,            EntryRange(EntryRange::Type::SHORT, 108)},
        {TraceHeaderKey::MUTS,             EntryRange(EntryRange::Type::SHORT, 110)},
        {TraceHeaderKey::MUTE,             EntryRange(EntryRange::Type::SHORT, 112)},
        {TraceHeaderKey::NS,               EntryRange(EntryRange::Type::USHORT, 114)},
        {TraceHeaderKey::DT,               EntryRange(EntryRange::Type::USHORT, 116)},
        {TraceHeaderKey::GAIN,             EntryRange(EntryRange::Type::SHORT, 118)},
        {TraceHeaderKey::IGC,              EntryRange(EntryRange::Type::SHORT, 120)},
        {TraceHeaderKey::IGI,              EntryRange(EntryRange::Type::SHORT, 122)},
        {TraceHeaderKey::CORR,             EntryRange(EntryRange::Type::SHORT, 124)},
        {TraceHeaderKey::SFS,              EntryRange(EntryRange::Type::SHORT, 126)},
        {TraceHeaderKey::SFE,              EntryRange(EntryRange::Type::SHORT, 128)},
        {TraceHeaderKey::SLEN,             EntryRange(EntryRange::Type::SHORT, 130)},
        {TraceHeaderKey::STYP,             EntryRange(EntryRange::Type::SHORT, 132)},
        {TraceHeaderKey::STAS,             EntryRange(EntryRange::Type::SHORT, 134)},
        {TraceHeaderKey::STAE,             EntryRange(EntryRange::Type::SHORT, 136)},
        {TraceHeaderKey::TATYP,            EntryRange(EntryRange::Type::SHORT, 138)},
        {TraceHeaderKey::AFILF,            EntryRange(EntryRange::Type::SHORT, 140)},
        {TraceHeaderKey::AFILS,            EntryRange(EntryRange::Type::SHORT, 142)},
        {TraceHeaderKey::NOFILF,           EntryRange(EntryRange::Type::SHORT, 144)},
        {TraceHeaderKey::NOFILS,           EntryRange(EntryRange::Type::SHORT, 146)},
        {TraceHeaderKey::LCF,              EntryRange(EntryRange::Type::SHORT, 148)},
        {TraceHeaderKey::HCF,              EntryRange(EntryRange::Type::SHORT, 150)},
        {TraceHeaderKey::LCS,              EntryRange(EntryRange::Type::SHORT, 152)},
        {TraceHeaderKey::HCS,              EntryRange(EntryRange::Type::SHORT, 154)},
        {TraceHeaderKey::YEAR,             EntryRange(EntryRange::Type::SHORT, 156)},
        {TraceHeaderKey::DAY,              EntryRange(EntryRange::Type::SHORT, 158)},
        {TraceHeaderKey::HOUR,             EntryRange(EntryRange::Type::SHORT, 160)},
        {TraceHeaderKey::MINUTE,           EntryRange(EntryRange::Type::SHORT, 162)},
        {TraceHeaderKey::SEC,              EntryRange(EntryRange::Type::SHORT, 164)},
        {TraceHeaderKey::TIMBAS,           EntryRange(EntryRange::Type::SHORT, 166)},
        {TraceHeaderKey::TRWF,             EntryRange(EntryRange::Type::SHORT, 168)},
        {TraceHeaderKey::GRNORS,           EntryRange(EntryRange::Type::SHORT, 170)},
        {TraceHeaderKey::GRNOFR,           EntryRange(EntryRange::Type::SHORT, 172)},
        {TraceHeaderKey::GRNLOF,           EntryRange(EntryRange::Type::SHORT, 174)},
        {TraceHeaderKey::GAPS,             EntryRange(EntryRange::Type::SHORT, 176)},
        {TraceHeaderKey::OTRAV,            EntryRange(EntryRange::Type::SHORT, 178)},
        {TraceHeaderKey::ENSX,             EntryRange(EntryRange::Type::UINT, 180)},
        {TraceHeaderKey::ENSY,             EntryRange(EntryRange::Type::UINT, 184)},
        {TraceHeaderKey::INLINE,           EntryRange(EntryRange::Type::UINT, 188)},
        {TraceHeaderKey::CROSS,            EntryRange(EntryRange::Type::UINT, 192)},
        {TraceHeaderKey::SHOOTPOINT,       EntryRange(EntryRange::Type::UINT, 196)},
        {TraceHeaderKey::SHOOTPOINT_SCALE, EntryRange(EntryRange::Type::SHORT, 200)},
        {TraceHeaderKey::TRACE_UNIT,       EntryRange(EntryRange::Type::SHORT, 202)},
        {TraceHeaderKey::TRANSD_CONST,     EntryRange(EntryRange::Type::CHAR, 204, 6)},
        {TraceHeaderKey::TRANSD_UNIT,      EntryRange(EntryRange::Type::SHORT, 210)},
        {TraceHeaderKey::TRID,             EntryRange(EntryRange::Type::SHORT, 212)},
        {TraceHeaderKey::SCALE_TIME,       EntryRange(EntryRange::Type::SHORT, 214)},
        {TraceHeaderKey::SRC_ORIENT,       EntryRange(EntryRange::Type::SHORT, 216, 6)},
        {TraceHeaderKey::SRC_DIRECTION,    EntryRange(EntryRange::Type::CHAR, 218, 6)},
        {TraceHeaderKey::SRC_MEASUREMT,    EntryRange(EntryRange::Type::CHAR, 224, 6)},
        {TraceHeaderKey::SRC_UNIT,         EntryRange(EntryRange::Type::SHORT, 230)},
        {TraceHeaderKey::UNASSIGNED1,      EntryRange(EntryRange::Type::CHAR, 232, 6)}
};
