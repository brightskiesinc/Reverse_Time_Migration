# Copyright (C) 2021 by Brightskies inc
#
# This file is part of BS I/O.
#
# BS I/O is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BS I/O is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.


# Executable
add_executable(Example_SegyToImage ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(Example_SegyToImage BS-IO)
