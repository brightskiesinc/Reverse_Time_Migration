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

#include <stbx/generators/primitive/ComputationParametersGetter.hpp>

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>

#include <operations/common/ComputationParameters.hpp>
#include <operations/common/DataTypes.h>

#include <prerequisites/libraries/nlohmann/json.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <omp.h>

#include <iostream>

using namespace stbx::generators;
using namespace operations::common;
using namespace bs::base::logger;
using json = nlohmann::json;


void print_parameters(ComputationParameters *parameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Used parameters : " << '\n';
    Logger->Info() << "\torder of stencil used : " << parameters->GetHalfLength() * 2 << '\n';
    Logger->Info() << "\tboundary length used : " << parameters->GetBoundaryLength() << '\n';
    Logger->Info() << "\tsource frequency : " << parameters->GetSourceFrequency() << '\n';
    Logger->Info() << "\tdt relaxation coefficient : " << parameters->GetRelaxedDT() << '\n';
    Logger->Info() << "\t# of threads : " << parameters->GetThreadCount() << '\n';
    Logger->Info() << "\tblock factor in x-direction : " << parameters->GetBlockX() << '\n';
    Logger->Info() << "\tblock factor in z-direction : " << parameters->GetBlockZ() << '\n';
    Logger->Info() << "\tblock factor in y-direction : " << parameters->GetBlockY() << '\n';
    if (parameters->IsUsingWindow()) {
        Logger->Info() << "\tWindow mode : enabled" << '\n';
        if (parameters->GetLeftWindow() == 0 && parameters->GetRightWindow() == 0) {
            Logger->Info() << "\t\tNO WINDOW IN X-axis" << '\n';
        } else {
            Logger->Info() << "\t\tLeft window : " << parameters->GetLeftWindow() << '\n';
            Logger->Info() << "\t\tRight window : " << parameters->GetRightWindow() << '\n';
        }
        if (parameters->GetFrontWindow() == 0 && parameters->GetBackWindow() == 0) {
            Logger->Info() << "\t\tNO WINDOW IN Y-axis" << '\n';
        } else {
            Logger->Info() << "\t\tFrontal window : " << parameters->GetFrontWindow() << '\n';
            Logger->Info() << "\t\tBackward window : " << parameters->GetBackWindow() << '\n';
        }
        if (parameters->GetDepthWindow() != 0) {
            Logger->Info() << "\t\tDepth window : " << parameters->GetDepthWindow() << '\n';
        } else {
            Logger->Info() << "\t\tNO WINDOW IN Z-axis" << '\n';
        }
    } else {
        Logger->Info() << "\tWindow mode : disabled (To enable set use-window=yes)..." << '\n';
    }
}

operations::common::ComputationParameters *
generate_parameters(json &map) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Parsing OpenMP computation properties..." << '\n';
    json computation_parameters_map = map["computation-parameters"];

    int boundary_length = -1, block_x = -1, block_z = -1, block_y = -1, order = -1;
    int left_win = -1, right_win = -1, front_win = -1, back_win = -1, depth_win = -1, use_window = -1;
    int n_threads;
    float dt_relax = -1, source_frequency = -1;
    HALF_LENGTH half_length = O_8;

#pragma omp parallel
    {
        n_threads = omp_get_num_threads();
    }

    auto computation_parameters_getter = new ComputationParametersGetter(computation_parameters_map);
    StencilOrder so = computation_parameters_getter->GetStencilOrder();
    order = so.order;
    half_length = so.half_length;

    boundary_length = computation_parameters_getter->GetBoundaryLength();
    source_frequency = computation_parameters_getter->GetSourceFrequency();
    dt_relax = computation_parameters_getter->GetDTRelaxed();
    block_x = computation_parameters_getter->GetBlock("x");
    block_y = computation_parameters_getter->GetBlock("y");
    block_z = computation_parameters_getter->GetBlock("z");

    Window w = computation_parameters_getter->GetWindow();
    left_win = w.left_win;
    right_win = w.right_win;
    front_win = w.front_win;
    back_win = w.back_win;
    depth_win = w.depth_win;
    use_window = w.use_window;

    if (order == -1) {
        Logger->Error() << "No valid value provided for key 'stencil-order'..." << '\n';
        Logger->Info() << "Using default stencil order of 8" << '\n';
        half_length = O_8;
    }
    if (boundary_length == -1) {
        Logger->Error() << "No valid value provided for key 'boundary-length'..." << '\n';
        Logger->Info() << "Using default boundary-length of 20" << '\n';
        boundary_length = 20;
    }
    if (source_frequency == -1) {
        Logger->Error() << "No valid value provided for key 'source-frequency'..." << '\n';
        Logger->Info() << "Using default source frequency of 20" << '\n';
        source_frequency = 20;
    }
    if (dt_relax == -1) {
        Logger->Error() << "No valid value provided for key 'dt-relax'..." << '\n';
        Logger->Info() << "Using default relaxation coefficient for dt calculation of 0.4" << '\n';
        dt_relax = 0.4;
    }
    if (block_x == -1) {
        Logger->Error() << "No valid value provided for key 'block-x'..." << '\n';
        Logger->Info() << "Using default blocking factor in x-direction of 560" << '\n';
        block_x = 560;
    }
    if (block_z == -1) {
        Logger->Error() << "No valid value provided for key 'block-z'..." << '\n';
        Logger->Info() << "Using default blocking factor in z-direction of 35" << '\n';
        block_z = 35;
    }
    if (use_window == -1) {
        Logger->Error() << "No valid value provided for key 'use-window'..." << '\n';
        Logger->Info() << "Disabling window by default.." << '\n';
        use_window = 0;
    }
    if (use_window) {
        if (left_win == -1) {
            Logger->Error() << "No valid value provided for key 'left-window'..." << '\n';
            Logger->Info()
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << '\n';
            left_win = 0;
        }
        if (right_win == -1) {
            Logger->Error() << "No valid value provided for key 'right-window'..." << '\n';
            Logger->Info()
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << '\n';
            right_win = 0;
        }
        if (depth_win == -1) {
            Logger->Error() << "No valid value provided for key 'depth-window'..." << '\n';
            Logger->Info() << "Using default window size of 0 - notice if window is 0, no windowing happens" << '\n';
            depth_win = 0;
        }
        if (front_win == -1) {
            Logger->Error() << "No valid value provided for key 'front-window'..." << '\n';
            Logger->Info()
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << '\n';
            front_win = 0;
        }
        if (back_win == -1) {
            Logger->Error() << "No valid value provided for key 'back-window'..." << '\n';
            Logger->Info()
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << '\n';
            back_win = 0;
        }
    }
    auto *parameters = new ComputationParameters(half_length);
    auto *configurationsGenerator = new ConfigurationsGenerator(map);

    /// General
    parameters->SetBoundaryLength(boundary_length);
    parameters->SetRelaxedDT(dt_relax);
    parameters->SetSourceFrequency(source_frequency);
    parameters->SetIsUsingWindow(use_window == 1);
    parameters->SetLeftWindow(left_win);
    parameters->SetRightWindow(right_win);
    parameters->SetDepthWindow(depth_win);
    parameters->SetFrontWindow(front_win);
    parameters->SetBackWindow(back_win);
    parameters->SetEquationOrder(configurationsGenerator->GetEquationOrder());
    parameters->SetApproximation(configurationsGenerator->GetApproximation());
    parameters->SetPhysics(configurationsGenerator->GetPhysics());

    /// OMP
    parameters->SetThreadCount(n_threads);
    parameters->SetBlockX(block_x);
    parameters->SetBlockZ(block_z);
    parameters->SetBlockY(block_y);

    print_parameters(parameters);
    return parameters;
}
