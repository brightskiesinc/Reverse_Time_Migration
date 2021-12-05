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

#include <operations/components/independents/concrete/forward-collectors/file-handler/file_handler.h>

void operations::components::helpers::bin_file_save(
        const char *file_name, const float *data, const size_t &size) {
    std::ofstream stream(file_name, std::ios::out | std::ios::binary);
    if (!stream.is_open()) {
        exit(EXIT_FAILURE);
    }
    stream.write(reinterpret_cast<const char *>(data), size * sizeof(float));
    stream.close();
}

void operations::components::helpers::bin_file_load(
        const char *file_name, float *data, const size_t &size) {
    std::ifstream stream(file_name, std::ios::binary | std::ios::in);
    if (!stream.is_open()) {
        exit(EXIT_FAILURE);
    }
    stream.read(reinterpret_cast<char *>(data), std::streamsize(size * sizeof(float)));
    stream.close();
}