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

set(BS_IO_SOURCES

        # READERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SegyReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SUReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/JsonReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/readers/SeismicReader.cpp

        # WRITERS
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/BinaryWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/CSVWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/ImageWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SegyWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SUWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/concrete/writers/SeismicWriter.cpp

        # HELPERS
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/InStreamHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers/OutStreamHelper.cpp

        ${BS_IO_SOURCES}
        PARENT_SCOPE
        )