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


#ifndef BS_IO_BS_IO_HPP
#define BS_IO_BS_IO_HPP

/* DATA UNITS. */
#include <bs/io/data-units/concrete/Trace.hpp>
#include <bs/io/data-units/concrete/Gather.hpp>

/* READERS/WRITERS. */
#include <bs/io/streams/primitive/Reader.hpp>
#include <bs/io/streams/primitive/Writer.hpp>
#include <bs/io/streams/concrete/readers/SeismicReader.hpp>
#include <bs/io/streams/concrete/writers/SeismicWriter.hpp>
#include <bs/io/streams/concrete/readers/SegyReader.hpp>
#include <bs/io/streams/concrete/writers/SegyWriter.hpp>

/* CONFIGURATIONS. */
#include <bs/io/configurations/MapKeys.h>

#endif //BS_IO_BS_IO_HPP
