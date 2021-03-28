//
// Created by amr-nasr on 12/12/2019.
//

#include <stbx/generators/primitive/ComputationParametersGetter.hpp>

#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>
#include <operations/backend/OneAPIBackend.hpp>

#include <operations/common/ComputationParameters.hpp>
#include <operations/common/DataTypes.h>

#include <libraries/nlohmann/json.hpp>

#include <iostream>

#include <CL/sycl.hpp>

using namespace std;
using namespace operations::common;
using namespace operations::backend;
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
    auto device = q->get_device();
    int temp_block_x = parameters->GetBlockX();
    if (temp_block_x % 16 != 0 && temp_block_x != 1) {
        temp_block_x = (temp_block_x + (16 - (temp_block_x % 16)));
    }
    int temp_block_z = parameters->GetBlockZ();
    auto maxBlockSize =
            device.get_info<cl::sycl::info::device::max_work_group_size>();
    if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        // Reject if STATIC block is bigger than max block size.
        if (temp_block_x * temp_block_z > maxBlockSize) {
            std::cout << "Warning : Invalid block size." << std::endl;
            std::cout << "Max workgroup size : " << maxBlockSize << std::endl;
            std::cout << "Used workgroup size : block-x(" << temp_block_x << ") * block-z("
                      << temp_block_z << ") = " << temp_block_x * temp_block_z
                      << std::endl;
            std::cout << "Notice : if block-x entered by user is different than the one "
                         "entered,"
                      << std::endl
                      << " this is because if block-x is not equal 1 and is not divisible "
                         "by 16. It is increased to be divisible by 16"
                      << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        // Reject if STATIC block is bigger than max block size.
        if (temp_block_x * temp_block_z > maxBlockSize) {
            std::cout << "Warning : Invalid block size." << std::endl;
            std::cout << "Max workgroup size : " << maxBlockSize << std::endl;
            std::cout << "Used workgroup size : block-x(" << temp_block_x << ") * block-z("
                      << temp_block_z << ") = " << temp_block_x * temp_block_z
                      << std::endl;
            std::cout << "Notice : if block-x entered by user is different than the one "
                         "entered,"
                      << std::endl
                      << " this is because if block-x is not equal 1 and is not divisible "
                         "by 16. It is increased to be divisible by 16"
                      << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (temp_block_z < parameters->GetHalfLength()) {
            std::cout << "Warning : Small block-z for the order selected" << std::endl;
            std::cout
                    << "For the selected order : a block-z of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << std::endl;
            std::cout << "Block in z = " << temp_block_z << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            std::cout << "Warning : Small block-x for the order selected" << std::endl;
            std::cout
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << std::endl;
            std::cout << "Block in x = " << temp_block_x << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        // Reject if block-x is bigger than max block size.
        if (temp_block_x > maxBlockSize) {
            std::cout << "Warning : Invalid block size." << std::endl;
            std::cout << "Max workgroup size : " << maxBlockSize << std::endl;
            std::cout << "Used workgroup size : block-x = " << temp_block_x << std::endl;
            std::cout << "Notice : if block-x entered by user is different than the one "
                         "entered,"
                      << std::endl
                      << " this is because if block-x is not equal 1 and is not divisible "
                         "by 16. It is increased to be divisible by 16"
                      << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            std::cout << "Warning : Small block-x for the order selected" << std::endl;
            std::cout
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << std::endl;
            std::cout << "Block in x = " << temp_block_x << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        // Reject if block-x is bigger than max block size.
        if (temp_block_x > maxBlockSize) {
            std::cout << "Warning : Invalid block size." << std::endl;
            std::cout << "Max workgroup size : " << maxBlockSize << std::endl;
            std::cout << "Used workgroup size : block-x = " << temp_block_x << std::endl;
            std::cout << "Notice : if block-x entered by user is different than the one "
                         "entered,"
                      << std::endl
                      << " this is because if block-x is not equal 1 and is not divisible "
                         "by 16. It is increased to be divisible by 16"
                      << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (temp_block_x < parameters->GetHalfLength()) {
            std::cout << "Warning : Small block-x for the order selected" << std::endl;
            std::cout
                    << "For the selected order : a block-x of at least the half length = "
                    << parameters->GetHalfLength() << " must be selected" << std::endl;
            std::cout << "Block in x = " << temp_block_x << std::endl;
            std::cout << "Terminating..." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

/*
 * Host-Code
 * Utility function to print Device info
 */
void PrintTargetInfo(cl::sycl::queue *q) {
    auto device = q->get_device();
    auto maxBlockSize =
            device.get_info<cl::sycl::info::device::max_work_group_size>();
    auto maxEUCount =
            device.get_info<cl::sycl::info::device::max_compute_units>();

    std::cout << " Running on " << device.get_info<cl::sycl::info::device::name>()
              << std::endl;
    std::cout << " The Device Max Work Group Size is : " << maxBlockSize
              << std::endl;
    std::cout << " The Device Max EUCount is : " << maxEUCount << std::endl;
}

void print_parameters(ComputationParameters *parameters) {
    std::cout << endl;
    std::cout << "Used parameters : " << endl;
    std::cout << "\torder of stencil used : " << parameters->GetHalfLength() * 2 << endl;
    std::cout << "\tboundary length used : " << parameters->GetBoundaryLength() << endl;
    std::cout << "\tsource frequency : " << parameters->GetSourceFrequency() << endl;
    std::cout << "\tdt relaxation coefficient : " << parameters->GetRelaxedDT() << endl;
    std::cout << "\tblock factor in x-direction : " << parameters->GetBlockX() << endl;
    std::cout << "\tblock factor in z-direction : " << parameters->GetBlockZ() << endl;
    std::cout << "\tblock factor in y-direction : " << parameters->GetBlockY() << endl;
    if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::CPU) {
        std::cout << "\tUsing CPU Device" << std::endl;
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SHARED) {
        std::cout << "\tUsing GPU Device - STATIC Memory Algorithm" << std::endl;
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU_SEMI_SHARED) {
        std::cout << "\tUsing GPU Device - Sliding in Z - STATIC Memory X Algorithm"
                  << std::endl;
    } else if (OneAPIBackend::GetInstance()->GetAlgorithm() == SYCL_ALGORITHM::GPU) {
        std::cout << "\tUsing GPU Device - Slice z + STATIC x Hybrid" << std::endl;
    }
    if (parameters->IsUsingWindow()) {
        std::cout << "\tWindow mode : enabled" << endl;
        if (parameters->GetLeftWindow() == 0 && parameters->GetRightWindow() == 0) {
            std::cout << "\t\tNO WINDOW IN X-axis" << endl;
        } else {
            std::cout << "\t\tLeft window : " << parameters->GetLeftWindow() << endl;
            std::cout << "\t\tRight window : " << parameters->GetRightWindow() << endl;
        }
        if (parameters->GetFrontWindow() == 0 && parameters->GetBackWindow() == 0) {
            std::cout << "\t\tNO WINDOW IN Y-axis" << endl;
        } else {
            std::cout << "\t\tFrontal window : " << parameters->GetFrontWindow() << endl;
            std::cout << "\t\tBackward window : " << parameters->GetBackWindow() << endl;
        }
        if (parameters->GetDepthWindow() != 0) {
            std::cout << "\t\tDepth window : " << parameters->GetDepthWindow() << endl;
        } else {
            std::cout << "\t\tNO WINDOW IN Z-axis" << endl;
        }
    } else {
        std::cout << "\tWindow mode : disabled (To enable set use-window=yes)..." << endl;
    }
    std::cout << endl;
}

struct Algorithm {
    int device_selected = DEF_VAL;
    SYCL_ALGORITHM selected_device = SYCL_ALGORITHM::CPU;
};

Algorithm ParseAlgorithm(json &map) {
    string value = map["algorithm"].get<std::string>();
    Algorithm alg;

    if (value == "cpu") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::CPU;
    } else if (value == "gpu-static") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU_SHARED;
    } else if (value == "gpu-semi-static") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU_SEMI_SHARED;
    } else if (value == "gpu") {
        alg.device_selected = 1;
        alg.selected_device = SYCL_ALGORITHM::GPU;
    } else {
        std::cout << "Invalid value entered for algorithm : must be <cpu> , <gpu> , "
                     "<gpu-static> or <gpu-semi-static>"
                  << std::endl;
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
    std::cout << "Parsing DPC++ computation properties..." << std::endl;
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
        std::cout << "No valid value provided for key 'source-frequency'..."
                  << std::endl;
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
    if (block_y == -1) {
        std::cout << "No valid value provided for key 'block-y'..." << std::endl;
        std::cout << "Using default blocking factor in y-direction of 5" << std::endl;
        block_y = 5;
    }
    if (device_selected == -1) {
        std::cout << "No valid value provided for key 'Device'..." << std::endl;
        std::cout << "Using default Device : CPU" << std::endl;
        selected_device = SYCL_ALGORITHM::CPU;
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

    /// OneAPI
    parameters->SetBlockX(block_x);
    parameters->SetBlockZ(block_z);
    parameters->SetBlockY(block_y);

    auto asyncHandler = [&](cl::sycl::exception_list eL) {
        for (auto &e : eL) {
            try {
                std::rethrow_exception(e);
            } catch (cl::sycl::exception &e) {
                std::cout << e.what() << std::endl;
                std::cout << "fail" << std::endl;
                // std::terminate() will exit the process, return non-zero, and output a
                // message to the user about the exception
                std::terminate();
            }
        }
    };
    if (device_name != 1) {
        if (selected_device == SYCL_ALGORITHM::CPU) {
            std::cout << "Using default CPU selector" << std::endl;
            sycl::cpu_selector cpu_sel;
            OneAPIBackend::GetInstance()->SetDeviceQueue(
                    new sycl::queue(cpu_sel, asyncHandler));
        } else {
            std::cout << "Using default GPU selector" << std::endl;
            sycl::gpu_selector gpu_sel;
            OneAPIBackend::GetInstance()->SetDeviceQueue(
                    new sycl::queue(gpu_sel, asyncHandler));
        }
    } else {
        std::cout << "Trying to select the Device that is closest to the given pattern '" << device_pattern << "'"
                  << std::endl;
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
