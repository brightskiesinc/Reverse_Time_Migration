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

#include <iostream>

#include <bs/io/utils/displayers/Displayer.hpp>
#include <bs/io/lookups/tables/TextHeaderLookup.hpp>

using namespace bs::io::utils::displayers;


void
Displayer::PrintTextHeader(unsigned char *apTextHeader) {
    if (apTextHeader == nullptr) {
        std::cerr << "Error: Null pointer received. Nothing to be printed." << std::endl;
        return;
    }

    for (size_t i = 0; i < IO_SIZE_TEXT_HEADER; i++) {
        if ((i % 80) == 0)
            std::cout << std::endl;
        std::cout << apTextHeader[i];
    }
    std::cout << std::endl;
}
