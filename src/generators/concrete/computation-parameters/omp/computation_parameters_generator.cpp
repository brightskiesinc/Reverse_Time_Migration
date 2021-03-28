//
// Created by zeyad-osama on 20/07/2020.
//

#include <stbx/generators/primitive/ComputationParametersGetter.hpp>

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>

#include <operations/common/ComputationParameters.hpp>
#include <operations/common/DataTypes.h>

#include <libraries/nlohmann/json.hpp>
#include <omp.h>

#include <iostream>

using namespace operations::common;
using namespace stbx::generators;
using json = nlohmann::json;

void print_parameters(ComputationParameters *parameters) {
    std::cout << std::endl;
    std::cout << "Used parameters : " << std::endl;
    std::cout << "\torder of stencil used : " << parameters->GetHalfLength() * 2 << std::endl;
    std::cout << "\tboundary length used : " << parameters->GetBoundaryLength() << std::endl;
    std::cout << "\tsource frequency : " << parameters->GetSourceFrequency() << std::endl;
    std::cout << "\tdt relaxation coefficient : " << parameters->GetRelaxedDT() << std::endl;
    std::cout << "\t# of threads : " << parameters->GetThreadCount() << std::endl;
    std::cout << "\tblock factor in x-direction : " << parameters->GetBlockX() << std::endl;
    std::cout << "\tblock factor in z-direction : " << parameters->GetBlockZ() << std::endl;
    std::cout << "\tblock factor in y-direction : " << parameters->GetBlockY() << std::endl;
    if (parameters->IsUsingWindow()) {
        std::cout << "\tWindow mode : enabled" << std::endl;
        if (parameters->GetLeftWindow() == 0 && parameters->GetRightWindow() == 0) {
            std::cout << "\t\tNO WINDOW IN X-axis" << std::endl;
        } else {
            std::cout << "\t\tLeft window : " << parameters->GetLeftWindow() << std::endl;
            std::cout << "\t\tRight window : " << parameters->GetRightWindow() << std::endl;
        }
        if (parameters->GetFrontWindow() == 0 && parameters->GetBackWindow() == 0) {
            std::cout << "\t\tNO WINDOW IN Y-axis" << std::endl;
        } else {
            std::cout << "\t\tFrontal window : " << parameters->GetFrontWindow() << std::endl;
            std::cout << "\t\tBackward window : " << parameters->GetBackWindow() << std::endl;
        }
        if (parameters->GetDepthWindow() != 0) {
            std::cout << "\t\tDepth window : " << parameters->GetDepthWindow() << std::endl;
        } else {
            std::cout << "\t\tNO WINDOW IN Z-axis" << std::endl;
        }
    } else {
        std::cout << "\tWindow mode : disabled (To enable set use-window=yes)..." << std::endl;
    }
    std::cout << std::endl;
}

operations::common::ComputationParameters *generate_parameters(json &map) {
    std::cout << "Parsing OpenMP computation properties..." << std::endl;
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
        std::cout << "No valid value provided for key 'stencil-order'..." << std::endl;
        std::cout << "Using default stencil order of 8" << std::endl;
        half_length = O_8;
    }
    if (boundary_length == -1) {
        std::cout << "No valid value provided for key 'boundary-length'..." << std::endl;
        std::cout << "Using default boundary-length of 20" << std::endl;
        boundary_length = 20;
    }
    if (source_frequency == -1) {
        std::cout << "No valid value provided for key 'source-frequency'..." << std::endl;
        std::cout << "Using default source frequency of 20" << std::endl;
        source_frequency = 20;
    }
    if (dt_relax == -1) {
        std::cout << "No valid value provided for key 'dt-relax'..." << std::endl;
        std::cout << "Using default relaxation coefficient for dt calculation of 0.4"
                  << std::endl;
        dt_relax = 0.4;
    }
    if (block_x == -1) {
        std::cout << "No valid value provided for key 'block-x'..." << std::endl;
        std::cout << "Using default blocking factor in x-direction of 560" << std::endl;
        block_x = 560;
    }
    if (block_z == -1) {
        std::cout << "No valid value provided for key 'block-z'..." << std::endl;
        std::cout << "Using default blocking factor in z-direction of 35" << std::endl;
        block_z = 35;
    }
    if (use_window == -1) {
        std::cout << "No valid value provided for key 'use-window'..." << std::endl;
        std::cout << "Disabling window by default.." << std::endl;
        use_window = 0;
    }
    if (use_window) {
        if (left_win == -1) {
            std::cout << "No valid value provided for key 'left-window'..." << std::endl;
            std::cout
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << std::endl;
            left_win = 0;
        }
        if (right_win == -1) {
            std::cout << "No valid value provided for key 'right-window'..." << std::endl;
            std::cout
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << std::endl;
            right_win = 0;
        }
        if (depth_win == -1) {
            std::cout << "No valid value provided for key 'depth-window'..." << std::endl;
            std::cout << "Using default window size of 0 - notice if window is 0, no windowing happens" << std::endl;
            depth_win = 0;
        }
        if (front_win == -1) {
            std::cout << "No valid value provided for key 'front-window'..." << std::endl;
            std::cout
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << std::endl;
            front_win = 0;
        }
        if (back_win == -1) {
            std::cout << "No valid value provided for key 'back-window'..." << std::endl;
            std::cout
                    << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis"
                    << std::endl;
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
