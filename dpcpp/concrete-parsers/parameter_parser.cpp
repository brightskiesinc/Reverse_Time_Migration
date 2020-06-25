//
// Created by amr on 12/12/2019.
//

#include "parameter_parser.h"
#include <concrete-components/data_units/acoustic_dpc_computation_parameters.h>
#include <fstream>
#include <iostream>

using namespace std;

sycl::queue *AcousticDpcComputationParameters::device_queue = nullptr;

/*
 * Host-Code
 * Utility function to check blocking factor.
 */
void CheckBlockingFactors(cl::sycl::queue *q,
                          AcousticDpcComputationParameters *parameters) {
  auto device = q->get_device();
  int temp_block_x = parameters->block_x;
  int temp_cor_block = parameters->cor_block;
  if (temp_block_x % 16 != 0 && temp_block_x != 1) {
    temp_block_x = (temp_block_x + (16 - (temp_block_x % 16)));
  }
  if (temp_cor_block % 16 != 0 && temp_cor_block != 1) {
    temp_cor_block = (temp_cor_block + (16 - (temp_cor_block % 16)));
  }
  int temp_block_z = parameters->block_z;
  auto maxBlockSize =
      device.get_info<cl::sycl::info::device::max_work_group_size>();
  if (parameters->device == CPU) {
    // All cases accepted-not using local groups.
  } else if (parameters->device == GPU_SHARED) {
    // Reject if shared block is bigger than max block size.
    if (temp_block_x * temp_block_z > maxBlockSize) {
      cout << "Warning : Invalid block size." << std::endl;
      cout << "Max workgroup size : " << maxBlockSize << std::endl;
      cout << "Used workgroup size : block-x(" << temp_block_x << ") * block-z("
           << temp_block_z << ") = " << temp_block_x * temp_block_z
           << std::endl;
      cout << "Notice : if block-x entered by user is different than the one "
              "entered,"
           << std::endl
           << " this is because if block-x is not equal 1 and is not divisible "
              "by 16. It is increased to be divisible by 16"
           << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
    if (temp_block_z < parameters->half_length) {
      cout << "Warning : Small block-z for the order selected" << std::endl;
      cout
          << "For the selected order : a block-z of at least the half length = "
          << parameters->half_length << " must be selected" << std::endl;
      cout << "Block in z = " << temp_block_z << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
    if (temp_block_x < parameters->half_length) {
      cout << "Warning : Small block-x for the order selected" << std::endl;
      cout
          << "For the selected order : a block-x of at least the half length = "
          << parameters->half_length << " must be selected" << std::endl;
      cout << "Block in x = " << temp_block_x << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
  } else if (parameters->device == GPU_SEMI_SHARED) {
    // Reject if block-x is bigger than max block size.
    if (temp_block_x > maxBlockSize) {
      cout << "Warning : Invalid block size." << std::endl;
      cout << "Max workgroup size : " << maxBlockSize << std::endl;
      cout << "Used workgroup size : block-x = " << temp_block_x << std::endl;
      cout << "Notice : if block-x entered by user is different than the one "
              "entered,"
           << std::endl
           << " this is because if block-x is not equal 1 and is not divisible "
              "by 16. It is increased to be divisible by 16"
           << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
    if (temp_block_x < parameters->half_length) {
      cout << "Warning : Small block-x for the order selected" << std::endl;
      cout
          << "For the selected order : a block-x of at least the half length = "
          << parameters->half_length << " must be selected" << std::endl;
      cout << "Block in x = " << temp_block_x << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
  } else if (parameters->device == GPU) {
    // Reject if block-x is bigger than max block size.
    if (temp_block_x > maxBlockSize) {
      cout << "Warning : Invalid block size." << std::endl;
      cout << "Max workgroup size : " << maxBlockSize << std::endl;
      cout << "Used workgroup size : block-x = " << temp_block_x << std::endl;
      cout << "Notice : if block-x entered by user is different than the one "
              "entered,"
           << std::endl
           << " this is because if block-x is not equal 1 and is not divisible "
              "by 16. It is increased to be divisible by 16"
           << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
    if (temp_block_x < parameters->half_length) {
      cout << "Warning : Small block-x for the order selected" << std::endl;
      cout
          << "For the selected order : a block-x of at least the half length = "
          << parameters->half_length << " must be selected" << std::endl;
      cout << "Block in x = " << temp_block_x << std::endl;
      cout << "Terminating..." << std::endl;
      exit(0);
    }
  }
  if (temp_cor_block > maxBlockSize) {
    cout << "Warning : Invalid block size." << std::endl;
    cout << "Max workgroup size : " << maxBlockSize << std::endl;
    cout << "Used workgroup size : cor-block = " << temp_cor_block << std::endl;
    cout << "Notice : if cor-block entered by user is different than the one "
            "entered,"
         << std::endl
         << " this is because if cor-block is not equal 1 and is not divisible "
            "by 16. It is increased to be divisible by 16"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
}
/*
 * Host-Code
 * Utility function to print device info
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

void PrintParameters(AcousticDpcComputationParameters *parameters) {
  cout << std::endl;
  cout << "Used parameters : " << std::endl;
  cout << "\torder of stencil used : " << parameters->half_length * 2
       << std::endl;
  cout << "\tboundary length used : " << parameters->boundary_length
       << std::endl;
  cout << "\tsource frequency : " << parameters->source_frequency << std::endl;
  cout << "\tdt relaxation coefficient : " << parameters->dt_relax << std::endl;
  cout << "\tblock factor in x-direction : " << parameters->block_x
       << std::endl;
  cout << "\tblock factor in z-direction : " << parameters->block_z
       << std::endl;
  cout << "\tblock factor in y-direction : " << parameters->block_y
       << std::endl;
  cout << "\tcorrelation block factor : " << parameters->cor_block << std::endl;
  if (parameters->device == CPU) {
    cout << "\tUsing CPU device" << std::endl;
  } else if (parameters->device == GPU_SHARED) {
    cout << "\tUsing GPU device - Shared Memory Algorithm" << std::endl;
  } else if (parameters->device == GPU_SEMI_SHARED) {
    cout << "\tUsing GPU device - Sliding in Z - Shared Memory X Algorithm"
         << std::endl;
  } else if (parameters->device == GPU) {
    cout << "\tUsing GPU device - Slice z + Shared x Hybrid" << std::endl;
  }
  cout << std::endl;
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
 * device=cpu
 */
ComputationParameters *ParseParameterFile(string &file_name) {
  ifstream param_file(file_name);
  if (!param_file) {
    cout << file_name << " is not found. Please provide a valid file."
         << std::endl;
    exit(0);
  }
  cout << "Parsing acoustic DPC++ computation properties..." << std::endl;
  string temp_line;
  int boundary_length = -1, block_x = -1, block_z = -1, block_y = -1,
  order = -1;
  float dt_relax = -1, source_frequency = -1;
  uint cor_block = -1;
  HALF_LENGTH half_length = O_8;
  int device_selected = -1;
  SYCL_DEVICE selected_device = CPU;
  while (param_file >> temp_line) {
    string key = temp_line.substr(0, temp_line.find('='));
    string value_s = temp_line.substr(temp_line.find('=') + 1);
    if (key == "stencil-order") {
      int value = stoi(value_s);
      switch (value) {
      case 2:
        order = 2;
        half_length = O_2;
        break;
      case 4:
        order = 4;
        half_length = O_4;
        break;
      case 8:
        order = 8;
        half_length = O_8;
        break;
      case 12:
        order = 12;
        half_length = O_12;
        break;
      case 16:
        order = 16;
        half_length = O_16;
        break;
      default:
        cout << "Invalid order value entered for stencil..." << std::endl;
        cout << "Supported values are : 2, 4, 8, 12, 16" << std::endl;
        break;
      }
    } else if (key == "boundary-length") {
      int value = stoi(value_s);
      if (value < 0) {
        cout << "Invalid value entered for boundary length : must be positive "
                "or zero..."
             << std::endl;
      } else {
        boundary_length = value;
      }
    } else if (key == "source-frequency") {
      float value = stof(value_s);
      if (value <= 0) {
        cout
            << "Invalid value entered for source frequency: must be positive..."
            << std::endl;
      } else {
        source_frequency = value;
      }
    } else if (key == "dt-relax") {
      float value = stof(value_s);
      if (value <= 0 || value > 1) {
        cout << "Invalid value entered for dt relaxation coefficient: must be "
                "larger than 0"
                " and less than or equal 1..."
             << value << std::endl;
      } else {
        dt_relax = value;
      }
    } else if (key == "block-x") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in x-direction : must "
                "be positive..."
             << std::endl;
      } else {
        block_x = value;
      }
    } else if (key == "block-z") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in z-direction : must "
                "be positive..."
             << std::endl;
      } else {
        block_z = value;
      }
    } else if (key == "block-y") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in y-direction : must "
                "be positive..."
             << std::endl;
      } else {
        block_y = value;
      }
    } else if (key == "cor-block") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for correlation block factor : must be "
                "positive..."
             << std::endl;
      } else {
        cor_block = value;
      }
    } else if (key == "device") {
      if (value_s == "cpu") {
        device_selected = 1;
        selected_device = CPU;
      } else if (value_s == "gpu-shared") {
        device_selected = 1;
        selected_device = GPU_SHARED;
      } else if (value_s == "gpu-semi-shared") {
        device_selected = 1;
        selected_device = GPU_SEMI_SHARED;
      } else if (value_s == "gpu") {
        device_selected = 1;
        selected_device = GPU;
      } else {
        cout << "Invalid value entered for device : must be <cpu> , <gpu> , "
                "<gpu-shared> or <gpu-semi-shared>"
             << std::endl;
      }
    }
  }
  if (order == -1) {
    cout << "No valid value provided for key 'stencil-order'..." << std::endl;
    cout << "Using default stencil order of 8" << std::endl;
    half_length = O_8;
  }
  if (boundary_length == -1) {
    cout << "No valid value provided for key 'boundary-length'..." << std::endl;
    cout << "Using default boundary-length of 20" << std::endl;
    boundary_length = 20;
  }
  if (source_frequency == -1) {
    cout << "No valid value provided for key 'source-frequency'..."
         << std::endl;
    cout << "Using default source frequency of 20" << std::endl;
    source_frequency = 20;
  }
  if (dt_relax == -1) {
    cout << "No valid value provided for key 'dt-relax'..." << std::endl;
    cout << "Using default relaxation coefficient for dt calculation of 0.4"
         << std::endl;
    dt_relax = 0.4;
  }
  if (block_x == -1) {
    cout << "No valid value provided for key 'block-x'..." << std::endl;
    cout << "Using default blocking factor in x-direction of 560" << std::endl;
    block_x = 560;
  }
  if (block_z == -1) {
    cout << "No valid value provided for key 'block-z'..." << std::endl;
    cout << "Using default blocking factor in z-direction of 35" << std::endl;
    block_z = 35;
  }
  if (block_y == -1) {
    cout << "No valid value provided for key 'block-y'..." << std::endl;
    cout << "Using default blocking factor in y-direction of 5" << std::endl;
    block_y = 5;
  }
  if (cor_block == -1) {
    cout << "No valid value provided for key 'cor-block'..." << std::endl;
    cout << "Using default correlation blocking factor of 256" << std::endl;
    cor_block = 256;
  }
  if (device_selected == -1) {
    cout << "No valid value provided for key 'device'..." << std::endl;
    cout << "Using default device : CPU" << std::endl;
    selected_device = CPU;
  }
  auto *parameters = new AcousticDpcComputationParameters(half_length);
  parameters->boundary_length = boundary_length;
  parameters->dt_relax = dt_relax;
  parameters->source_frequency = source_frequency;
  parameters->block_x = block_x;
  parameters->block_z = block_z;
  parameters->block_y = block_y;
  parameters->cor_block = cor_block;
  parameters->device = selected_device;
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
  if (selected_device == CPU) {
    sycl::cpu_selector cpu_sel;
    sycl::device cpu_device(cpu_sel);
    AcousticDpcComputationParameters::device_queue =
        new sycl::queue(cpu_device, asyncHandler);
  } else {
    sycl::gpu_selector gpu_sel;
    AcousticDpcComputationParameters::device_queue =
        new sycl::queue(gpu_sel, asyncHandler);
  }
  PrintParameters(parameters);
  PrintTargetInfo(AcousticDpcComputationParameters::device_queue);
  CheckBlockingFactors(AcousticDpcComputationParameters::device_queue,
                       parameters);
  return parameters;
}