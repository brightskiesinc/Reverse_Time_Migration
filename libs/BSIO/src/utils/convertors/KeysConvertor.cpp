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

#include <iostream>

#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/utils/convertors/KeysConvertor.hpp>

using namespace bs::io::utils::convertors;
using namespace bs::io::dataunits;
using namespace bs::base::exceptions;


TraceHeaderKey
KeysConvertor::ToTraceHeaderKey(std::string &aKey) {
    char key_char[aKey.length()];
    strcpy(key_char, aKey.c_str());
    for (int i = 0; i < aKey.length(); ++i) {
        key_char[i] = toupper(key_char[i]);
    }
    auto val = KeysConvertor::mTraceKeyMap.find(key_char);
    if (val == KeysConvertor::mTraceKeyMap.end()) {
        throw NO_KEY_FOUND_EXCEPTION();
    }
    return {val->second};
}

const std::unordered_map<std::string, TraceHeaderKey::Key> KeysConvertor::mTraceKeyMap = {
        {"TRACL",            TraceHeaderKey::TRACL},
        {"TRACR",            TraceHeaderKey::TRACR},
        {"FLDR",             TraceHeaderKey::FLDR},
        {"TRACF",            TraceHeaderKey::TRACF},
        {"EP",               TraceHeaderKey::EP},
        {"CDP",              TraceHeaderKey::CDP},
        {"CDPT",             TraceHeaderKey::CDPT},
        {"TRID",             TraceHeaderKey::TRID},
        {"NVS",              TraceHeaderKey::NVS},
        {"NHS",              TraceHeaderKey::NHS},
        {"DUSE",             TraceHeaderKey::DUSE},
        {"OFFSET",           TraceHeaderKey::OFFSET},
        {"GELEV",            TraceHeaderKey::GELEV},
        {"SELEV",            TraceHeaderKey::SELEV},
        {"SDEPTH",           TraceHeaderKey::SDEPTH},
        {"GDEL",             TraceHeaderKey::GDEL},
        {"SDEL",             TraceHeaderKey::SDEL},
        {"SWDEP",            TraceHeaderKey::SWDEP},
        {"DWDEP",            TraceHeaderKey::GWDEP},
        {"SCALEL",           TraceHeaderKey::SCALEL},
        {"SCALCO",           TraceHeaderKey::SCALCO},
        {"SX",               TraceHeaderKey::SX},
        {"SY",               TraceHeaderKey::SY},
        {"GX",               TraceHeaderKey::GX},
        {"GY",               TraceHeaderKey::GY},
        {"COINTIT",          TraceHeaderKey::COINTIT},
        {"WEVEL",            TraceHeaderKey::WEVEL},
        {"SWEVEL",           TraceHeaderKey::SWEVEL},
        {"SUT",              TraceHeaderKey::SUT},
        {"GUT",              TraceHeaderKey::GUT},
        {"SSTAT",            TraceHeaderKey::SSTAT},
        {"GSTAT",            TraceHeaderKey::GSTAT},
        {"TSTAT",            TraceHeaderKey::TSTAT},
        {"LAGA",             TraceHeaderKey::LAGA},
        {"LAGB",             TraceHeaderKey::LAGB},
        {"DELRT",            TraceHeaderKey::DELRT},
        {"MUTS",             TraceHeaderKey::MUTS},
        {"MUTE",             TraceHeaderKey::MUTE},
        {"DT",               TraceHeaderKey::DT},
        {"GAIN",             TraceHeaderKey::GAIN},
        {"IGC",              TraceHeaderKey::IGC},
        {"IGI",              TraceHeaderKey::IGI},
        {"CORR",             TraceHeaderKey::CORR},
        {"SFS",              TraceHeaderKey::SFS},
        {"SFE",              TraceHeaderKey::SFE},
        {"SLEN",             TraceHeaderKey::SLEN},
        {"STYP",             TraceHeaderKey::STYP},
        {"STAS",             TraceHeaderKey::STAS},
        {"STAE",             TraceHeaderKey::STAE},
        {"TATYP",            TraceHeaderKey::TATYP},
        {"AFILF",            TraceHeaderKey::AFILF},
        {"AFILS",            TraceHeaderKey::AFILS},
        {"NOFILF",           TraceHeaderKey::NOFILF},
        {"NOFILS",           TraceHeaderKey::NOFILS},
        {"LCF",              TraceHeaderKey::LCF},
        {"HCF",              TraceHeaderKey::HCF},
        {"LCS",              TraceHeaderKey::LCS},
        {"HCS",              TraceHeaderKey::HCS},
        {"YEAR",             TraceHeaderKey::YEAR},
        {"DAY",              TraceHeaderKey::DAY},
        {"HOUR",             TraceHeaderKey::HOUR},
        {"MINUTE",           TraceHeaderKey::MINUTE},
        {"SEC",              TraceHeaderKey::SEC},
        {"TIMBAS",           TraceHeaderKey::TIMBAS},
        {"TRWF",             TraceHeaderKey::TRWF},
        {"GRNORS",           TraceHeaderKey::GRNORS},
        {"GRNOFR",           TraceHeaderKey::GRNOFR},
        {"GRNLOF",           TraceHeaderKey::GRNLOF},
        {"GAPS",             TraceHeaderKey::GAPS},
        {"OTRAV",            TraceHeaderKey::OTRAV},
        {"D1",               TraceHeaderKey::D1},
        {"F1",               TraceHeaderKey::F1},
        {"D2",               TraceHeaderKey::D2},
        {"F2",               TraceHeaderKey::F2},
        {"UNGPOW",           TraceHeaderKey::UNGPOW},
        {"UNSCALE",          TraceHeaderKey::UNSCALE},
        {"NTR",              TraceHeaderKey::NTR},
        {"MARK",             TraceHeaderKey::MARK},
        {"SHORTPAD",         TraceHeaderKey::SHORTPAD},
        {"NS",               TraceHeaderKey::NS},
        {"DATA_BYTE_OFFSET", TraceHeaderKey::DATA_BYTE_OFFSET}
};
