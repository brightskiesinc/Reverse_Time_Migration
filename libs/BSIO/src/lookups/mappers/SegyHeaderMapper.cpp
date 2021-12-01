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

#include <bs/io/lookups/mappers/SegyHeaderMapper.hpp>

using namespace bs::io::lookups;
using namespace bs::io::dataunits;


std::unordered_map<bs::io::dataunits::TraceHeaderKey::Key,
        std::pair<size_t, NATIVE_TYPE>> SegyHeaderMapper::mLocationTable = {
        {TraceHeaderKey::TRACL,      {0,   NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::TRACR,      {4,   NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::FLDR,       {8,   NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::TRACF,      {12,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::EP,         {16,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::CDP,        {20,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::CDPT,       {24,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::TRACE_CODE, {28,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::NVS,        {30,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::NHS,        {32,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::DUSE,       {34,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::OFFSET,     {36,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::GELEV,      {40,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SELEV,      {44,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SDEPTH,     {48,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::GDEL,       {52,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SDEL,       {56,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SWDEP,      {60,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::GWDEP,      {64,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SCALEL,     {68,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SCALCO,     {70,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SX,         {72,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::SY,         {76,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::GX,         {80,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::GY,         {84,  NATIVE_TYPE::UNSIGNED_INT}},
        {TraceHeaderKey::COINTIT,    {88,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::WEVEL,      {90,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SWEVEL,     {92,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SUT,        {94,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GUT,        {96,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SSTAT,      {98,  NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GSTAT,      {100, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::TSTAT,      {102, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::LAGA,       {104, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::LAGB,       {106, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::DELRT,      {108, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::MUTS,       {110, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::MUTE,       {112, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::NS,         {114, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::DT,         {116, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GAIN,       {118, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::IGC,        {120, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::IGI,        {122, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::CORR,       {124, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SFS,        {126, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SFE,        {128, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SLEN,       {130, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::STYP,       {132, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::STAS,       {134, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::STAE,       {136, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::TATYP,      {138, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::AFILF,      {140, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::AFILS,      {142, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::NOFILF,     {144, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::NOFILS,     {146, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::LCF,        {148, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::HCF,        {150, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::LCS,        {152, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::HCS,        {154, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::YEAR,       {156, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::DAY,        {158, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::HOUR,       {160, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::MINUTE,     {162, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::SEC,        {164, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::TIMBAS,     {166, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::TRWF,       {168, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GRNORS,     {170, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GRNOFR,     {172, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GRNLOF,     {174, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::GAPS,       {176, NATIVE_TYPE::UNSIGNED_SHORT}},
        {TraceHeaderKey::OTRAV,      {178, NATIVE_TYPE::UNSIGNED_SHORT}},
};
