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

#ifndef BS_IO_DATA_UNITS_DATA_TYPES_TRACE_HEADER_HPP
#define BS_IO_DATA_UNITS_DATA_TYPES_TRACE_HEADER_HPP

#include <string>
#include <unordered_map>
#include <map>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <vector>
#include <sstream>

namespace bs {
    namespace io {
        namespace dataunits {
            /**
             * @brief Trace Header
             */
            class TraceHeaderKey {
            public:
                /**
                * @brief Enum with the different Trace Headers.
                */
                enum Key : char {
                    TRACL,
                    TRACR,
                    FLDR,
                    TRACF,
                    EP,
                    CDP,
                    CDPT,
                    TRACE_CODE,
                    NVS,
                    NHS,
                    DUSE,
                    OFFSET,
                    GELEV,
                    SELEV,
                    SDEPTH,
                    GDEL,
                    SDEL,
                    SWDEP,
                    GWDEP,
                    SCALEL,
                    SCALCO,
                    SX,
                    SY,
                    GX,
                    GY,
                    COINTIT,
                    WEVEL,
                    SWEVEL,
                    SUT,
                    GUT,
                    SSTAT,
                    GSTAT,
                    TSTAT,
                    LAGA,
                    LAGB,
                    DELRT,
                    MUTS,
                    MUTE,
                    NS,
                    DT,
                    GAIN,
                    IGC,
                    IGI,
                    CORR,
                    SFS,
                    SFE,
                    SLEN,
                    STYP,
                    STAS,
                    STAE,
                    TATYP,
                    AFILF,
                    AFILS,
                    NOFILF,
                    NOFILS,
                    LCF,
                    HCF,
                    LCS,
                    HCS,
                    YEAR,
                    DAY,
                    HOUR,
                    MINUTE,
                    SEC,
                    TIMBAS,
                    TRWF,
                    GRNORS,
                    GRNOFR,
                    GRNLOF,
                    GAPS,
                    OTRAV,

                    /* EXTENDED. */

                    ENSX,
                    ENSY,
                    INLINE,
                    CROSS,
                    SHOOTPOINT,
                    SHOOTPOINT_SCALE,
                    TRACE_UNIT,
                    TRANSD_CONST,
                    TRANSD_UNIT,
                    TRID,
                    SCALE_TIME,
                    SRC_ORIENT,
                    SRC_DIRECTION,
                    SRC_MEASUREMT,
                    SRC_UNIT,
                    UNASSIGNED1,
                    D1,
                    F1,
                    D2,
                    F2,
                    UNGPOW,
                    UNSCALE,
                    NTR,
                    MARK,
                    SHORTPAD,

                    /** INTERNALLY USED TRACE HEADERS **/
                    SYN_X_IND,
                    SYN_Y_IND,
                    DATA_BYTE_OFFSET,
                };

            public:
                /**
                 * @brief Trace Header constructor.
                 */
                TraceHeaderKey(Key aKey) {
                    this->mKey = aKey;
                }

                /**
                 * @brief Copy constructor, to copy char  representing one of the enum to the class.
                 *
                 * @param[in] aType
                 * The char mValue representing one of the enums.
                 */
                explicit constexpr TraceHeaderKey(char aValue)
                        : mKey(static_cast<TraceHeaderKey::Key>(aValue)) {}

                /**
                 * @brief Trace Header Destructor.
                 */
                ~TraceHeaderKey() = default;

                Key GetKey() const {
                    return mKey;
                }

                /**
                 * @brief Equality operator overloading.
                 *
                 * @param[in] aType
                 * Comparison target.
                 *
                 * @return True if they contain the same type.
                 */
                constexpr bool operator==(TraceHeaderKey aTraceHeaderAttribute) const {
                    return this->mKey == aTraceHeaderAttribute.mKey;
                }

                /**
                 * @brief Inequality operator overloading.
                 *
                 * @param[in] aTraceHeaderAttribute
                 * Comparison target.
                 *
                 * @return True if they do not contain the same type.
                 */
                constexpr bool operator!=(TraceHeaderKey aTraceHeaderAttribute) const {
                    return this->mKey != aTraceHeaderAttribute.mKey;
                }

                /**
                 * @brief Greater equality operator overloading.
                 *
                 * @param[in] aTraceHeaderAttribute
                 * Comparison target.
                 *
                 * @return True if aTraceHeaderAttribute is greater than current object.
                 */
                constexpr bool operator>(TraceHeaderKey aTraceHeaderAttribute) const {
                    return this->mKey > aTraceHeaderAttribute.mKey;
                }

                /**
                 * @brief Smaller equality operator overloading.
                 *
                 * @param[in] aTraceHeaderAttribute
                 * Comparison target.
                 *
                 * @return True if aTraceHeaderAttribute is smaller than current object.
                 */
                constexpr bool operator<(TraceHeaderKey aTraceHeaderAttribute) const {
                    return this->mKey < aTraceHeaderAttribute.mKey;
                }

                /**
                 * @brief Allow switch and comparisons.
                 * @return The enum mValue.
                 */
                explicit operator Key() const { return this->mKey; }  // Allow switch and comparisons.

                /**
                 * @brief Prevent usage of if (aTraceHeaderAttribute)
                 * @return Nothing as it's deleted.
                 */
                explicit operator bool() = delete;

                static std::string GatherKeysToString(const std::vector<TraceHeaderKey> &aGatherKeys) {
                    std::string key;
                    for (const auto &it : aGatherKeys) {
                        key += it.ToString() + "_";
                    }
                    return key;
                }

                static std::string GatherValuesToString(const std::vector<std::string> &aGatherValues) {
                    std::string values;
                    for (const auto &it : aGatherValues) {
                        values += it + "_";
                    }
                    return values;
                }

                static std::vector<std::string> StringToGatherValues(const std::string &aKey) {
                    std::vector<std::string> values;
                    std::stringstream check1(aKey);

                    std::string temp;

                    while (getline(check1, temp, '_')) {
                        values.push_back(temp);
                    }

                    return values;
                }

                /**
                 * @brief Convert trace header to string.
                 * @return A string representation of the trace header.
                 */
                std::string ToString() const {
                    static const std::unordered_map<TraceHeaderKey::Key, std::string> ENUM_TO_STRING = {
                            {TRACL,            "TRACL"},
                            {TRACR,            "TRACR"},
                            {FLDR,             "FLDR"},
                            {TRACF,            "TRACF"},
                            {EP,               "EP"},
                            {CDP,              "CDP"},
                            {CDPT,             "CDPT"},
                            {NVS,              "NVS"},
                            {NHS,              "NHS"},
                            {DUSE,             "DUSE"},
                            {OFFSET,           "OFFSET"},
                            {GELEV,            "GELEV"},
                            {SELEV,            "SELEV"},
                            {SDEPTH,           "SDEPTH"},
                            {GDEL,             "GDEL"},
                            {SDEL,             "SDEL"},
                            {SWDEP,            "SWDEP"},
                            {GWDEP,            "DWDEP"},
                            {SCALEL,           "SCALEL"},
                            {SCALCO,           "SCALCO"},
                            {SX,               "SX"},
                            {SY,               "SY"},
                            {GX,               "GX"},
                            {GY,               "GY"},
                            {COINTIT,          "COINTIT"},
                            {WEVEL,            "WEVEL"},
                            {SWEVEL,           "SWEVEL"},
                            {SUT,              "SUT"},
                            {GUT,              "GUT"},
                            {SSTAT,            "SSTAT"},
                            {GSTAT,            "GSTAT"},
                            {TSTAT,            "TSTAT"},
                            {LAGA,             "LAGA"},
                            {LAGB,             "LAGB"},
                            {DELRT,            "DELRT"},
                            {MUTS,             "MUTS"},
                            {MUTE,             "MUTE"},
                            {NS,               "NS"},
                            {DT,               "DT"},
                            {GAIN,             "GAIN"},
                            {IGC,              "IGC"},
                            {IGI,              "IGI"},
                            {CORR,             "CORR"},
                            {SFS,              "SFS"},
                            {SFE,              "SFE"},
                            {SLEN,             "SLEN"},
                            {STYP,             "STYP"},
                            {STAS,             "STAS"},
                            {STAE,             "STAE"},
                            {TATYP,            "TATYP"},
                            {AFILF,            "AFILF"},
                            {AFILS,            "AFILS"},
                            {NOFILF,           "NOFILF"},
                            {NOFILS,           "NOFILS"},
                            {LCF,              "LCF"},
                            {HCF,              "HCF"},
                            {LCS,              "LCS"},
                            {HCS,              "HCS"},
                            {YEAR,             "YEAR"},
                            {DAY,              "DAY"},
                            {HOUR,             "HOUR"},
                            {MINUTE,           "MINUTE"},
                            {SEC,              "SEC"},
                            {TIMBAS,           "TIMBAS"},
                            {TRWF,             "TRWF"},
                            {GRNORS,           "GRNORS"},
                            {GRNOFR,           "GRNOFR"},
                            {GRNLOF,           "GRNLOF"},
                            {GAPS,             "GAPS"},
                            {OTRAV,            "OTRAV"},

                            /* EXTENDED. */


                            {ENSX,             "ENSX"},
                            {ENSY,             "ENSY"},
                            {INLINE,           "INLINE"},
                            {CROSS,            "CROSS"},
                            {SHOOTPOINT,       "SHOOTPOINT"},
                            {SHOOTPOINT_SCALE, "SHOOTPOINT_SCALE"},
                            {TRACE_UNIT,       "TRACE_UNIT"},
                            {TRANSD_CONST,     "TRANSD_CONST"},
                            {TRANSD_UNIT,      "TRANSD_UNIT"},
                            {TRID,             "TRID"},
                            {SCALE_TIME,       "SCALE_TIME"},
                            {SRC_ORIENT,       "SRC_ORIENT"},
                            {SRC_DIRECTION,    "SRC_DIRECTION"},
                            {SRC_MEASUREMT,    "SRC_MEASUREMT"},
                            {SRC_UNIT,         "SRC_UNIT"},
                            {UNASSIGNED1,      "UNASSIGNED1"},
                            {DATA_BYTE_OFFSET, "DATA_BYTE_OFFSET"}
                    };
                    return ENUM_TO_STRING.find(this->mKey)->second;
                }

                /**
                 * @brief Convert trace header type to char.
                 * @return A char representation of the trace header.
                 */
                inline char ToChar() const { return this->mKey; }

                /**
                 * @brief Provides the hash for TraceHeaderKey object.
                 *
                 * @return A hash representation of this object.
                 */
                size_t hash() const {
                    return std::hash<Key>()(this->mKey);
                }

            private:
                /// @brief Key for trace header.
                /// @see {TraceHeaderKey::Key}
                Key mKey;
            };
        }//namespace dataunits
    } //namespace io
} //namespace bs

namespace std {
    /**
     * @brief The hash function used by the STL containers for TraceHeaderKey class.
     */
    template<>
    struct hash<bs::io::dataunits::TraceHeaderKey> {
        std::size_t operator()(const bs::io::dataunits::TraceHeaderKey &f) const {
            return f.hash();
        }
    };
} //namespace std

#endif //BS_IO_DATA_UNITS_DATA_TYPES_TRACE_HEADER_HPP
