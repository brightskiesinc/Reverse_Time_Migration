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


#include <operations/test-utils/dummy-data-generators/DummyModelGenerator.hpp>

#include <operations/utils/io/write_utils.h>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

using namespace std;

namespace operations {
    namespace testutils {

        float *generate_dummy_model(const std::string &aFileName) {
            int nx = 5;
            int ny = 5;
            int nz = 5;
            int nt = 1;
            float dx = 1.0f;
            float dy = 1.0f;
            float dz = 10.0f;
            float dt = 1.0;
            bool is_traces = false;
            string file_name = OPERATIONS_TEST_DATA_PATH "/" + aFileName + ".segy";
            int size = nx * ny * nz;

            auto data = new float[size];
            for (int i = 0; i < size; i++) {
                data[i] = 1;
            }
            nlohmann::json configuration_map;
            bs::base::configurations::JSONConfigurationMap io_map(configuration_map);
            bs::io::streams::SeismicWriter writer(bs::io::streams::SeismicWriter::ToWriterType("segy"),
                                                  &io_map);
            writer.AcquireConfiguration();
            writer.Initialize(file_name);
            auto gathers = operations::utils::io::TransformToGather(data, nx, ny, nz, dx, dy, dz,
                                                                    0, 0, 0, 0,
                                                                    1, 1e3, 1e3);
            writer.Write(gathers);
            writer.Finalize();
            for (auto g : gathers) {
                delete g;
            }
            return data;
        }
    } //namespace testutils
} //namespace operations
