# Copyright (C) 2021 by Brightskies inc
#
# This file is part of BS Base Package.
#
# BS Base Package is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BS Base Package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


set(BS_BASE_SOURCES

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/mem_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/string_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_tracker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/mem_utils.cpp

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_allocator.cpp

        ${BS_BASE_SOURCES}
        PARENT_SCOPE
        )
