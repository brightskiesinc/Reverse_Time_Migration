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
#include <operations/backend/OneAPIBackend.hpp>

#include <operations/common/ComputationParameters.hpp>
#include <operations/common/DataTypes.h>

#include <prerequisites/libraries/nlohmann/json.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <iostream>

#include <CL/sycl.hpp>

using namespace std;
using namespace operations::common;
using namespace operations::backend;
using namespace bs::base::logger;
using namespace stbx::generators;
using json = nlohmann::json;

// This is the class provided to SYCL runtime by the application to decide
// on which Device to run, or whether to run at all.
// When selecting a Device, SYCL runtime first takes (1) a selector provided by
// the program or a default one and (2) the set of all available devices. Then
// it passes each Device to the '()' operator of the selector. Device, for
// which '()' returned the highest number, is selected. If a negative number
// was returned for all devices, then the selection process will cause an
// exception.
class MyDeviceSelector : public cl::sycl::device_selector {
public:
    MyDeviceSelector(const std::string &p) : pattern(p) {
        // std::cout << "Looking for \"" << p << "\" devices" << std::endl;
    }

    // This is the function which gives a "rating" to devices.
    virtual int operator()(const cl::sycl::device &device) const override {
        // The template parameter to Device.get_info can be a variety of properties
        // defined by the SYCL spec's cl::sycl::info:: enum. Properties may have
        // different types. Here we query name which is a string.
        const std::string name = device.get_info<cl::sycl::info::device::name>();
        // std::cout << "Trying Device: " << name << "..." << std::endl;
        // std::cout << "  Vendor: " <<
        // Device.get_info<cl::sycl::info::device::vendor>() << std::endl;

        // Device with pattern in the name is prioritized:
        return (name.find(pattern) != std::string::npos) ? 100 : 1;
    }

private:
    std::string pattern;
};

/*
 * Host-Code
 * Utility function to check blocking factor.
 */
void CheckBlockingFactors(cl::sycl::queue *q,
                          ComputationParameters *parameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto device = q->get_device();
    int temp_block_x = parameters->GetBlockX();
    if (temp_block_x % 16 != 0 && temp_block_x != 1) {
        temp_block_x = (temp_block_x + (16 - (temp_block_x % 16)));
    }
    int temp_block_z = parameters->GetBlockZ();
    auto maxBlockSize =
            device.get_info<cl::sycl::info::device::max_work_group_size>();
    if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        // No need to check since the blocks don't control the group launching.
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        // Reject if STATIC block is bigger than max block size.
        if (temp_block_x * temp_block_z > maxBlockSize) {
            Logger->Info() << "Warning : Invalid block size." << '\n';
            Logger->Info() << "Max workgroup size : " << maxBlockSize << '\n';
            Logger->Info() << "Used workgroup size : block-x(" << temp_block_x << ") * block-z("
                           << temp_block_z << ") = " << temp_block_x * temp_block_z << '\n';
            Logger->Info() << "Notice : if block-x entered by user is different than the one "
                              "entered,"

                           << " this is because if block-x is not equal 1 and is not divisible "
                              "by 16. It is increased to be divisible by 16"
                           << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
        if (temp_block_z < parameters->GetHalfLength()) {
            Logger->Info() << "Warning : Small block-z for the order selected" << '\n';
            Logger->Info()
                    << "For the selected order : a block-z of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << '\n';
            Logger->Info() << "Block in z = " << temp_block_z << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            Logger->Info() << "Warning : Small block-x for the order selected" << '\n';
            Logger->Info()
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << '\n';
            Logger->Info() << "Block in x = " << temp_block_x << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        // Reject if block-x is bigger than max block size.
        if (temp_block_x > maxBlockSize) {
            Logger->Info() << "Warning : Invalid block size." << '\n';
            Logger->Info() << "Max workgroup size : " << maxBlockSize << '\n';
            Logger->Info() << "Used workgroup size : block-x = " << temp_block_x << '\n';
            Logger->Info() << "Notice : if block-x entered by user is different than the one "
                              "entered,"

                           << " this is because if block-x is not equal 1 and is not divisible "
                              "by 16. It is increased to be divisible by 16"
                           << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            Logger->Info() << "Warning : Small block-x for the order selected" << '\n';
            Logger->Info()
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << '\n';
            Logger->Info() << "Block in x = " << temp_block_x << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        // Reject if block-x is bigger than max block size.
        if (temp_block_x > maxBlockSize) {
            Logger->Info() << "Warning : Invalid block size." << '\n';
            Logger->Info() << "Max workgroup size : " << maxBlockSize << '\n';
            Logger->Info() << "Used workgroup size : block-x = " << temp_block_x << '\n';
            Logger->Info() << "Notice : if block-x entered by user is different than the one "
                              "entered,"

                           << " this is because if block-x is not equal 1 and is not divisible "
                              "by 16. It is increased to be divisible by 16"
                           << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            Logger->Info() << "Warning : Small block-x for the order selected" << '\n';
            Logger->Info()
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << '\n';
            Logger->Info() << "Block in x = " << temp_block_x << '\n';
            Logger->Info() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
    }
}

/*
 * Host-Code
 * Utility function to print Device info
 */
void PrintTargetInfo(cl::sycl::queue *q) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto device = q->get_device();
    auto maxBlockSize =
            device.get_info<cl::sycl::info::device::max_work_group_size>();
    auto maxEUCount =
            device.get_info<cl::sycl::info::device::max_compute_units>();

    Logger->Info() << " Running on " << device.get_info<cl::sycl::info::device::name>() << '\n';
    Logger->Info() << " The Device Max Work Group Size is : " << maxBlockSize << '\n';
    Logger->Info() << " The Device Max EUCount is : " << maxEUCount << '\n';
}

void print_parameters(ComputationParameters *parameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Used parameters : " << '\n';
    Logger->Info() << "\torder of stencil used : " << parameters->GetHalfLength() * 2 << '\n';
    Logger->Info() << "\tboundary length used : " << parameters->GetBoundaryLength() << '\n';
    Logger->Info() << "\tsource frequency : " << parameters->GetSourceFrequency() << '\n';
    Logger->Info() << "\tdt relaxation coefficient : " << parameters->GetRelaxedDT() << '\n';
    Logger->Info() << "\tblock factor in x-direction : " << parameters->GetBlockX() << '\n';
    Logger->Info() << "\tblock factor in z-direction : " << parameters->GetBlockZ() << '\n';
    Logger->Info() << "\tblock factor in y-direction : " << parameters->GetBlockY() << '\n';
    if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        Logger->Info() << "\tUsing CPU Algorithm" << '\n';
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        Logger->Info() << "\tUsing GPU Algorithm - Shared Memory Algorithm" << '\n';
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        Logger->Info() << "\tUsing GPU Algorithm - Sliding in Z - Shared Memory X Algorithm" << '\n';
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        Logger->Info() << "\tUsing GPU Algorithm - Slice z + Shared x Hybrid" << '\n';
    }
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

struct Algorithm {
    int device_selected = DEF_VAL;
    SYCL_ALGORITHM selected_device = SYCL_ALGORITHM::CPU;
};

Algorithm ParseAlgorithm(json &map) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    string value = map["algorithm"].get<std::string>();
    Algorithm alg;

    if (value == "cpu") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::CPU;
    } else if (value == "gpu-shared") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU_SHARED;
    } else if (value == "gpu-semi-shared") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU_SEMI_SHARED;
    } else if (value == "gpu") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU;
    } else {
        Logger->Error() << "Invalid value entered for algorithm : must be <cpu> , <gpu> , "
                           "<gpu-shared> or <gpu-semi-shared>" << '\n';
    }
    return alg;
}

struct Device {
    int device_name = DEF_VAL;
    string device_pattern;
};

Device ParseDevice(json &map) {
    string value = map["device"].get<std::string>();
    Device d;
    if (value != "none") {
        d.device_name = 1;
        d.device_pattern = value;
    }
    return d;
}

/*!
 * File format should be follow the following example :
 *
 * stencil-order=8
 * boundary-length=20
 * source-frequency=200
 * dt-relax=0.4
 * thread-number=4
 * block-x=500
 * block-z=44
 * block-y=5
 * Device=cpu
 */
ComputationParameters *generate_parameters(json &map) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Parsing DPC++ computation properties..." << '\n';
    json computation_parameters_map = map["computation-parameters"];
    int boundary_length = -1, block_x = -1, block_z = -1, block_y = -1,
            order = -1;
    float dt_relax = -1, source_frequency = -1;
    uint cor_block = -1;
    HALF_LENGTH half_length = O_8;
    string device_pattern;
    int device_selected = -1;
    int left_win = -1, right_win = -1, front_win = -1, back_win = -1, depth_win = -1, use_window = -1, device_name = -1;
    SYCL_ALGORITHM selected_device = SYCL_ALGORITHM::CPU;

    auto *computationParametersGetter = new stbx::generators::ComputationParametersGetter(computation_parameters_map);
    StencilOrder so = computationParametersGetter->GetStencilOrder();
    order = so.order;
    half_length = so.half_length;

    boundary_length = computationParametersGetter->GetBoundaryLength();
    source_frequency = computationParametersGetter->GetSourceFrequency();
    dt_relax = computationParametersGetter->GetDTRelaxed();
    block_x = computationParametersGetter->GetBlock("x");
    block_y = computationParametersGetter->GetBlock("y");
    block_z = computationParametersGetter->GetBlock("z");

    Window w = computationParametersGetter->GetWindow();
    left_win = w.left_win;
    right_win = w.right_win;
    front_win = w.front_win;
    back_win = w.back_win;
    depth_win = w.depth_win;
    use_window = w.use_window;

    Algorithm alg = ParseAlgorithm(computation_parameters_map);
    device_selected = alg.device_selected;
    selected_device = alg.selected_device;

    Device d = ParseDevice(computation_parameters_map);
    device_name = d.device_name;
    device_pattern = d.device_pattern;

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
    if (block_y == -1) {
        Logger->Error() << "No valid value provided for key 'block-y'..." << '\n';
        Logger->Info() << "Using default blocking factor in y-direction of 5" << '\n';
        block_y = 5;
    }
    if (device_selected == -1) {
        Logger->Error() << "No valid value provided for key 'Device'..." << '\n';
        Logger->Info() << "Using default Device : CPU" << '\n';
        selected_device = SYCL_ALGORITHM::CPU;
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

    /// OneAPI
    parameters->SetBlockX(block_x);
    parameters->SetBlockZ(block_z);
    parameters->SetBlockY(block_y);

    auto asyncHandler = [&](cl::sycl::exception_list eL) {
        for (auto &e : eL) {
            try {
                std::rethrow_exception(e);
            } catch (cl::sycl::exception &e) {
                Logger->Error() << e.what() << '\n';
                Logger->Error() << "fail" << '\n';
                // std::terminate() will exit the process, return non-zero, and output a
                // message to the user about the exception
                std::terminate();
            }
        }
    };
    if (device_name != 1) {
        if (selected_device == SYCL_ALGORITHM::CPU) {
            Logger->Info() << "Using default CPU selector" << '\n';
            sycl::cpu_selector cpu_sel;
            OneAPIBackend::GetInstance()->SetDeviceQueue(
                    new sycl::queue(cpu_sel, asyncHandler));
        } else {
            Logger->Info() << "Using default GPU selector" << '\n';
            sycl::gpu_selector gpu_sel;
            OneAPIBackend::GetInstance()->SetDeviceQueue(
                    new sycl::queue(gpu_sel, asyncHandler));
        }
    } else {
        Logger->Info() << "Trying to select the Device that is closest to the given pattern '" << device_pattern << "'"
                       << '\n';
        MyDeviceSelector dev_sel(device_pattern);
        OneAPIBackend::GetInstance()->SetDeviceQueue(
                new sycl::queue(dev_sel, asyncHandler));
    }
    OneAPIBackend::GetInstance()->SetAlgorithm(selected_device);
    print_parameters(parameters);
    PrintTargetInfo(OneAPIBackend::GetInstance()->GetDeviceQueue());
    CheckBlockingFactors(OneAPIBackend::GetInstance()->GetDeviceQueue(), parameters);
    return parameters;
}
