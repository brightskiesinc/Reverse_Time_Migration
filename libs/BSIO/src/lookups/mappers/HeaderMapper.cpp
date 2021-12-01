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

#include <bs/base/exceptions/Exceptions.hpp>

#include <bs/io/lookups/mappers/HeaderMapper.hpp>
#include <bs/io/utils/convertors/NumbersConvertor.hpp>

using namespace bs::io::lookups;
using namespace bs::io::utils::convertors;


void
HeaderMapper::MapHeaderToTrace(const char *apRawData, io::dataunits::Trace &aTrace,
                               std::unordered_map<dataunits::TraceHeaderKey::Key, std::pair<size_t, NATIVE_TYPE>>
                               &aLocationTable,
                               bool aSwap) {
    for (auto &it : aLocationTable) {
        dataunits::TraceHeaderKey::Key key = it.first;
        size_t offset = it.second.first;
        NATIVE_TYPE type = it.second.second;
        const char *cur_data = (apRawData + offset);
        switch (type) {
            case NATIVE_TYPE::CHAR:
            case NATIVE_TYPE::UNSIGNED_CHAR:
                aTrace.SetTraceHeaderKeyValue(key, *cur_data);
                break;
            case NATIVE_TYPE::SHORT:
            case NATIVE_TYPE::UNSIGNED_SHORT: {
                short s = *((short *) (cur_data));
                if (aSwap) {
                    s = NumbersConvertor::ToLittleEndian(s);
                }
                aTrace.SetTraceHeaderKeyValue(key, s);

            }
                break;
            case NATIVE_TYPE::INT:
            case NATIVE_TYPE::UNSIGNED_INT: {
                int i = *((int *) (cur_data));
                if (aSwap) {
                    i = NumbersConvertor::ToLittleEndian(i);
                }
                aTrace.SetTraceHeaderKeyValue(key, i);
            }
                break;
            case NATIVE_TYPE::LONG:
            case NATIVE_TYPE::UNSIGNED_LONG: {
                long l = *((long *) (cur_data));
                if (aSwap) {
//                    l = NumbersConvertor::ToLittleEndian(l);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                aTrace.SetTraceHeaderKeyValue(key, l);
            }
                break;
            case NATIVE_TYPE::FLOAT: {
                float f = *((float *) (cur_data));
                if (aSwap) {
//                    f = NumbersConvertor::ToLittleEndian(f);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                aTrace.SetTraceHeaderKeyValue(key, f);
            }
                break;
            case NATIVE_TYPE::DOUBLE: {
                double d = *((double *) (cur_data));
                if (aSwap) {
//                    d = NumbersConvertor::ToLittleEndian(d);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                aTrace.SetTraceHeaderKeyValue(key, d);
            }
                break;
        }
    }
}

void
HeaderMapper::MapTraceToHeader(char *apRawData, io::dataunits::Trace &aTrace,
                               std::unordered_map<dataunits::TraceHeaderKey::Key, std::pair<size_t, NATIVE_TYPE>> &aLocationTable,
                               bool aSwap) {
    for (auto &it : aLocationTable) {
        dataunits::TraceHeaderKey::Key key = it.first;
        size_t offset = it.second.first;
        NATIVE_TYPE type = it.second.second;
        char *cur_data = (apRawData + offset);
        switch (type) {
            case NATIVE_TYPE::CHAR:
            case NATIVE_TYPE::UNSIGNED_CHAR: {
                char c = 0;
                if (aTrace.HasTraceHeader(key)) {
                    c = aTrace.GetTraceHeaderKeyValue<char>(key);
                }
                *cur_data = c;
            }
                break;
            case NATIVE_TYPE::SHORT:
            case NATIVE_TYPE::UNSIGNED_SHORT: {
                short s = 0;
                if (aTrace.HasTraceHeader(key)) {
                    s = aTrace.GetTraceHeaderKeyValue<short>(key);
                }
                if (aSwap) {
                    s = NumbersConvertor::ToLittleEndian(s);
                }
                *((short *) (cur_data)) = s;
            }
                break;
            case NATIVE_TYPE::INT:
            case NATIVE_TYPE::UNSIGNED_INT: {
                int i = 0;
                if (aTrace.HasTraceHeader(key)) {
                    i = aTrace.GetTraceHeaderKeyValue<int>(key);
                }
                if (aSwap) {
                    i = NumbersConvertor::ToLittleEndian(i);
                }
                *((int *) (cur_data)) = i;
            }
                break;
            case NATIVE_TYPE::LONG:
            case NATIVE_TYPE::UNSIGNED_LONG: {
                int l = 0;
                if (aTrace.HasTraceHeader(key)) {
                    l = aTrace.GetTraceHeaderKeyValue<long>(key);
                }
                if (aSwap) {
//                    l = NumbersConvertor::ToLittleEndian(l);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                *((long *) (cur_data)) = l;
            }
                break;
            case NATIVE_TYPE::FLOAT: {
                float f = 0;
                if (aTrace.HasTraceHeader(key)) {
                    f = aTrace.GetTraceHeaderKeyValue<float>(key);
                }
                if (aSwap) {
//                    f = NumbersConvertor::ToLittleEndian(f);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                *((float *) (cur_data)) = f;
            }
                break;
            case NATIVE_TYPE::DOUBLE: {
                double d = 0;
                if (aTrace.HasTraceHeader(key)) {
                    d = aTrace.GetTraceHeaderKeyValue<double>(key);
                }
                if (aSwap) {
//                    d = NumbersConvertor::ToLittleEndian(d);
                    throw bs::base::exceptions::UNSUPPORTED_FEATURE_EXCEPTION();
                }
                *((double *) (cur_data)) = d;
            }
                break;
        }
    }
}

size_t
bs::io::lookups::native_type_to_size(NATIVE_TYPE native_type) {
    switch (native_type) {
        case NATIVE_TYPE::CHAR:
            return sizeof(char);
        case NATIVE_TYPE::UNSIGNED_CHAR:
            return sizeof(unsigned char);
        case NATIVE_TYPE::SHORT:
            return sizeof(short);
        case NATIVE_TYPE::UNSIGNED_SHORT:
            return sizeof(unsigned short);
        case NATIVE_TYPE::LONG:
            return sizeof(long);
        case NATIVE_TYPE::UNSIGNED_LONG:
            return sizeof(unsigned long);
        case NATIVE_TYPE::INT:
            return sizeof(int);
        case NATIVE_TYPE::UNSIGNED_INT:
            return sizeof(unsigned int);
        case NATIVE_TYPE::FLOAT:
            return sizeof(float);
        case NATIVE_TYPE::DOUBLE:
            return sizeof(double);
        default:
            return 0;
    }
}
