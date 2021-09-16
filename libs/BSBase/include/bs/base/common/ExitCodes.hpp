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

#ifndef BS_BASE_COMMON_EXIT_CODES_HPP
#define BS_BASE_COMMON_EXIT_CODES_HPP

namespace bs {
    namespace base {
        namespace common {

#define BS_BASE_RC_FAILURE        0      /* Failure exit code. */
#define BS_BASE_RC_SUCCESS        1      /* Success exit code. */
#define BS_BASE_RC_ABORTED        2      /* Abortion exit code. */

        } // namespace common
    } //namespace base
} //namespace bs

#endif //BS_BASE_COMMON_EXIT_CODES_HPP
