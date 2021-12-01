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

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <operations/test-utils/dummy-data-generators/DummyTraceGenerator.hpp>

#include <operations/utils/io/write_utils.h>


using namespace std;

namespace operations {
    namespace testutils {

        float *generate_dummy_trace(const std::string &aFileName,
                                    dataunits::GridBox *apGridBox,
                                    int trace_stride_x,
                                    int trace_stride_y) {
            // One trace every n points in x
            uint nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize() / trace_stride_x;
            uint nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
            uint ny = 1;
            uint nt = nz;

            float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
            float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension() * trace_stride_x;
            float dy = 1.0f;

            float dt = 1.0f;

            if (apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize() > 1) {
                // One trace every n points in y
                ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize() / trace_stride_y;
                dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension() * trace_stride_y;
            }

            auto data = new float[nx * nz * ny];
            for (int i = 0; i < nx * nz * ny; i++) {
                data[i] = (float) rand() * 100 / RAND_MAX;
            }
            nlohmann::json configuration_map;
            bs::base::configurations::JSONConfigurationMap io_map(configuration_map);
            bs::io::streams::SeismicWriter writer(bs::io::streams::SeismicWriter::ToWriterType("segy"),
                                                  &io_map);
            writer.AcquireConfiguration();
            std::string path = aFileName;
            writer.Initialize(path);
            auto gathers = operations::utils::io::TransformToGather(data, 1, 1, nz, dx, dy, dt,
                                                                    0, 0, 0, 0,
                                                                    nx * ny, 1e3, 1e6);
            writer.Write(gathers);
            writer.Finalize();
            for (auto g : gathers) {
                delete g;
            }
            return data;
        }

    } //namespace testutils
} //namespace operations
