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

#ifndef BS_IO_UTILS_CONVERTORS_KEYS_CONVERTOR_HPP
#define BS_IO_UTILS_CONVERTORS_KEYS_CONVERTOR_HPP

#include <bs/io/data-units/data-types/TraceHeaderKey.hpp>

#include <string>
#include <unordered_map>

namespace bs {
    namespace io {
        namespace utils {
            namespace convertors {

                /**
                 * @brief Keys convertor that work as a convertor from string representation
                 * to HeaderKey object(s) and vice versa.
                 */
                class KeysConvertor {
                public:
                    /**
                     * @brief Converts string representation for trace header key to TraceHeaderKey object.
                     */
                    static dataunits::TraceHeaderKey
                    ToTraceHeaderKey(std::string &aKey);

                private:
                    /**
                     * @brief Private constructor for preventing objects creation.
                     */
                    KeysConvertor() = default;

                private:
                    static const std::unordered_map<std::string, dataunits::TraceHeaderKey::Key> mTraceKeyMap;
                };

            } //namespace convertors
        } //namespace utils
    } //namespace io
} //namespace bs

#endif //BS_IO_UTILS_CONVERTORS_KEYS_CONVERTOR_HPP
