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

#ifndef SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H
#define SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H

#include <string>

namespace stbx {
    namespace testutils {
        template<typename Base, typename T>
        inline bool instanceof(const T *) {
            return std::is_base_of<Base, T>::value;
        }
    } //namespace testutils
} //namespace stbx

#endif //SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H
