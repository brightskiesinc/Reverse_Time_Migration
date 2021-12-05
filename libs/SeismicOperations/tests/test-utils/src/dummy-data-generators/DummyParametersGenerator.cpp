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


#include <operations/test-utils/dummy-data-generators/DummyParametersGenerator.hpp>

using namespace operations::common;

ComputationParameters *
generate_computation_parameters_no_wind(APPROXIMATION aApproximation);

ComputationParameters *
generate_computation_parameters_inc_wind(APPROXIMATION aApproximation);

ComputationParameters *operations::testutils::generate_computation_parameters(
        OP_TU_WIND aWindow, APPROXIMATION aApproximation) {
    ComputationParameters *p = nullptr;
    if (aWindow == OP_TU_NO_WIND) {
        p = generate_computation_parameters_no_wind(aApproximation);
    } else if (aWindow == OP_TU_INC_WIND) {
        p = generate_computation_parameters_inc_wind(aApproximation);
    }
    return p;
}


ComputationParameters *
generate_computation_parameters_no_wind(APPROXIMATION aApproximation) {
    /*
     * Variables initialization for computation parameters.
     */

    HALF_LENGTH half_length = O_8;
    int boundary_length = 5;
    float dt_relax = 0.9;
    float source_frequency = 20;
    int left_win = 0;
    int right_win = 0;
    int depth_win = 0;
    int front_win = 0;
    int back_win = 0;
    EQUATION_ORDER equation_order = SECOND;
    PHYSICS physics = ACOUSTIC;
    APPROXIMATION approximation = aApproximation;

    int n_threads = 1;
    int block_x = 1;
    int block_z = 1;
    int block_y = 1;

    int use_window = 0;

    int grain_side_length = 10;

    auto computation_parameters = new ComputationParameters(half_length);

    computation_parameters->SetBoundaryLength(boundary_length);
    computation_parameters->SetRelaxedDT(dt_relax);
    computation_parameters->SetSourceFrequency(source_frequency);
    computation_parameters->SetIsUsingWindow(use_window == 1);
    computation_parameters->SetLeftWindow(left_win);
    computation_parameters->SetRightWindow(right_win);
    computation_parameters->SetDepthWindow(depth_win);
    computation_parameters->SetFrontWindow(front_win);
    computation_parameters->SetBackWindow(back_win);
    computation_parameters->SetEquationOrder(equation_order);
    computation_parameters->SetApproximation(approximation);
    computation_parameters->SetPhysics(physics);


    computation_parameters->SetThreadCount(n_threads);
    computation_parameters->SetBlockX(block_x);
    computation_parameters->SetBlockZ(block_z);
    computation_parameters->SetBlockY(block_y);

    return computation_parameters;
}

ComputationParameters *
generate_computation_parameters_inc_wind(APPROXIMATION aApproximation) {
    /*
     * Variables initialization for computation parameters.
     */

    HALF_LENGTH half_length = O_8;
    int boundary_length = 5;
    float dt_relax = 0.9;
    float source_frequency = 20;
    int left_win = 12;
    int right_win = 11;
    int depth_win = 23;
    int front_win = 0;
    int back_win = 0;
    EQUATION_ORDER equation_order = SECOND;
    PHYSICS physics = ACOUSTIC;
    APPROXIMATION approximation = aApproximation;

    int n_threads = 1;
    int block_x = 1;
    int block_z = 1;
    int block_y = 1;

    int grain_side_length = 10;

    int use_window = 1;

    auto computation_parameters = new ComputationParameters(half_length);

    computation_parameters->SetBoundaryLength(boundary_length);
    computation_parameters->SetRelaxedDT(dt_relax);
    computation_parameters->SetSourceFrequency(source_frequency);
    computation_parameters->SetIsUsingWindow(use_window == 1);
    computation_parameters->SetLeftWindow(left_win);
    computation_parameters->SetRightWindow(right_win);
    computation_parameters->SetDepthWindow(depth_win);
    computation_parameters->SetFrontWindow(front_win);
    computation_parameters->SetBackWindow(back_win);
    computation_parameters->SetEquationOrder(equation_order);
    computation_parameters->SetApproximation(approximation);
    computation_parameters->SetPhysics(physics);


    computation_parameters->SetThreadCount(n_threads);
    computation_parameters->SetBlockX(block_x);
    computation_parameters->SetBlockZ(block_z);
    computation_parameters->SetBlockY(block_y);

    return computation_parameters;
}

/// @todo { TO BE REMOVED
ComputationParameters *generate_average_case_parameters() {
    HALF_LENGTH half_length = O_8;
    int boundary_length = 5;
    float dt_relax = 0.9;
    float source_frequency = 20;
    int left_win = 12;
    int right_win = 11;
    int depth_win = 23;
    int front_win = 0;
    int back_win = 0;
    EQUATION_ORDER equation_order = SECOND;
    PHYSICS physics = ACOUSTIC;
    APPROXIMATION approximation = ISOTROPIC;

    int n_threads = 1;
    int block_x = 1;
    int block_z = 1;
    int block_y = 1;

    int use_window = 0;

    int grain_side_length = 10;


    auto computation_parameters = new ComputationParameters(half_length);

    computation_parameters->SetBoundaryLength(boundary_length);
    computation_parameters->SetRelaxedDT(dt_relax);
    computation_parameters->SetSourceFrequency(source_frequency);
    computation_parameters->SetIsUsingWindow(use_window == 1);
    computation_parameters->SetLeftWindow(left_win);
    computation_parameters->SetRightWindow(right_win);
    computation_parameters->SetDepthWindow(depth_win);
    computation_parameters->SetFrontWindow(front_win);
    computation_parameters->SetBackWindow(back_win);
    computation_parameters->SetEquationOrder(equation_order);
    computation_parameters->SetApproximation(approximation);
    computation_parameters->SetPhysics(physics);


    computation_parameters->SetThreadCount(n_threads);
    computation_parameters->SetBlockX(block_x);
    computation_parameters->SetBlockZ(block_z);
    computation_parameters->SetBlockY(block_y);

    return computation_parameters;
}
/// }
