/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_BASE_COMMON_ASSERTIONS_TPP
#define BS_BASE_COMMON_ASSERTIONS_TPP

#include <type_traits>

namespace bs {
    namespace base {
        namespace common {

#define ASSERT_IS_POD(ARG)      std::is_pod<ARG>::value                     /* Assert that argument refers to a POD. */
#define ASSERT_IS_STR(ARG)      std::is_base_of<std::string, ARG>::value    /* Assert that argument is a string . */

#define ASSERT_T_TEMPLATE(ARG) \
static_assert(ASSERT_IS_POD(ARG) || ASSERT_IS_STR(ARG), "T type is not compatible")

        } // namespace common
    } //namespace base
} //namespace bs

#endif //BS_BASE_COMMON_ASSERTIONS_TPP
