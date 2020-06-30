//
// Created by amr on 12/12/2019.
//

#include <concrete-components/data_units/acoustic_openmp_computation_parameters.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <parameter_parser.h>

using namespace std;

void PrintParameters(AcousticOmpComputationParameters *parameters) {
  cout << endl;
  cout << "Used parameters : " << endl;
  cout << "\torder of stencil used : " << parameters->half_length * 2 << endl;
  cout << "\tboundary length used : " << parameters->boundary_length << endl;
  cout << "\tsource frequency : " << parameters->source_frequency << endl;
  cout << "\tdt relaxation coefficient : " << parameters->dt_relax << endl;
  cout << "\t# of threads : " << parameters->n_threads << endl;
  cout << "\tblock factor in x-direction : " << parameters->block_x << endl;
  cout << "\tblock factor in z-direction : " << parameters->block_z << endl;
  cout << "\tblock factor in y-direction : " << parameters->block_y << endl;
  if (parameters->use_window) {
      cout << "\tWindow mode : enabled" << endl;
      if (parameters->left_window == 0 && parameters->right_window == 0) {
          cout <<"\t\tNO WINDOW IN X-axis" << endl;
      } else {
          cout << "\t\tLeft window : " << parameters->left_window << endl;
          cout << "\t\tRight window : " << parameters->right_window << endl;
      }
      if (parameters->front_window == 0 && parameters->back_window == 0) {
          cout <<"\t\tNO WINDOW IN Y-axis" << endl;
      } else {
          cout << "\t\tFrontal window : " << parameters->front_window << endl;
          cout << "\t\tBackward window : " << parameters->back_window << endl;
      }
      if (parameters->depth_window != 0) {
          cout << "\t\tDepth window : " << parameters->depth_window << endl;
      } else {
          cout <<"\t\tNO WINDOW IN Z-axis" << endl;
      }
  } else {
      cout << "\tWindow mode : disabled (To enable set use-window=yes)..." << endl;
  }
  cout << endl;
}

ComputationParameters *ParseParameterFile(string &file_name) {
  ifstream param_file(file_name);
  if (!param_file) {
    cout << file_name << " is not found. Please provide a valid file." << endl;
    exit(0);
  }
  cout << "Parsing acoustic OpenMP computation properties..." << endl;
  string temp_line;
  int boundary_length = -1, block_x = -1, block_z = -1, block_y = -1,order = -1;
  int left_win = -1, right_win = -1, front_win = -1, back_win = -1, depth_win = -1, use_window = -1;
    int n_threads;
#pragma omp parallel
    {
        n_threads= omp_get_num_threads();
    }
    float dt_relax = -1, source_frequency = -1;
  HALF_LENGTH half_length = O_8;
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
        cout << "Invalid order value entered for stencil..." << endl;
        cout << "Supported values are : 2, 4, 8, 12, 16" << endl;
        break;
      }
    } else if (key == "boundary-length") {
      int value = stoi(value_s);
      if (value < 0) {
        cout << "Invalid value entered for boundary length : must be positive "
                "or zero..."
             << endl;
      } else {
        boundary_length = value;
      }
    } else if (key == "source-frequency") {
      float value = stof(value_s);
      if (value <= 0) {
        cout
            << "Invalid value entered for source frequency: must be positive..."
            << endl;
      } else {
        source_frequency = value;
      }
    } else if (key == "dt-relax") {
      float value = stof(value_s);
      if (value <= 0 || value > 1) {
        cout << "Invalid value entered for dt relaxation coefficient: must be "
                "larger than 0"
                " and less than or equal 1..."
             << value << endl;
      } else {
        dt_relax = value;
      }
    } else if (key == "block-x") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in x-direction : must "
                "be positive..."
             << endl;
      } else {
        block_x = value;
      }
    } else if (key == "block-z") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in z-direction : must "
                "be positive..."
             << endl;
      } else {
        block_z = value;
      }
    } else if (key == "block-y") {
      int value = stoi(value_s);
      if (value <= 0) {
        cout << "Invalid value entered for block factor in y-direction : must "
                "be positive..."
             << endl;
      } else {
        block_y = value;
      }
    } else if (key == "use-window") {
        if (value_s == "yes") {
            use_window = 1;
        } else {
            use_window = 0;
        }
    } else if (key == "left-window") {
        int value = stoi(value_s);
        if (value < 0) {
            cout << "Invalid value entered for left window in x-direction : must "
                    "be positive..."
                 << endl;
        } else {
            left_win = value;
        }
    } else if (key == "right-window") {
        int value = stoi(value_s);
        if (value < 0) {
            cout << "Invalid value entered for right window in x-direction : must "
                    "be positive..."
                 << endl;
        } else {
            right_win = value;
        }
    } else if (key == "depth-window") {
        int value = stoi(value_s);
        if (value < 0) {
            cout << "Invalid value entered for depth window in z-direction : must "
                    "be positive..."
                 << endl;
        } else {
            depth_win = value;
        }
    } else if (key == "front-window") {
        int value = stoi(value_s);
        if (value < 0) {
            cout << "Invalid value entered for front window in y-direction : must "
                    "be positive..."
                 << endl;
        } else {
            front_win = value;
        }
    } else if (key == "back-window") {
        int value = stoi(value_s);
        if (value < 0) {
            cout << "Invalid value entered for back window in y-direction : must "
                    "be positive..."
                 << endl;
        } else {
            back_win = value;
        }
    }
  }
  if (order == -1) {
    cout << "No valid value provided for key 'stencil-order'..." << endl;
    cout << "Using default stencil order of 8" << endl;
    half_length = O_8;
  }
  if (boundary_length == -1) {
    cout << "No valid value provided for key 'boundary-length'..." << endl;
    cout << "Using default boundary-length of 20" << endl;
    boundary_length = 20;
  }
  if (source_frequency == -1) {
    cout << "No valid value provided for key 'source-frequency'..." << endl;
    cout << "Using default source frequency of 20" << endl;
    source_frequency = 20;
  }
  if (dt_relax == -1) {
    cout << "No valid value provided for key 'dt-relax'..." << endl;
    cout << "Using default relaxation coefficient for dt calculation of 0.4"
         << endl;
    dt_relax = 0.4;
  }
  if (block_x == -1) {
    cout << "No valid value provided for key 'block-x'..." << endl;
    cout << "Using default blocking factor in x-direction of 560" << endl;
    block_x = 560;
  }
  if (block_z == -1) {
    cout << "No valid value provided for key 'block-z'..." << endl;
    cout << "Using default blocking factor in z-direction of 35" << endl;
    block_z = 35;
  }
  if (use_window == -1) {
    cout << "No valid value provided for key 'use-window'..." << endl;
    cout << "Disabling window by default.." << endl;
    use_window = 0;
  }
  if (use_window) {
      if (left_win == -1) {
          cout << "No valid value provided for key 'left-window'..." << endl;
          cout << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis" << endl;
          left_win = 0;
      }
      if (right_win == -1) {
          cout << "No valid value provided for key 'right-window'..." << endl;
          cout << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis" << endl;
          right_win = 0;
      }
      if (depth_win == -1) {
          cout << "No valid value provided for key 'depth-window'..." << endl;
          cout << "Using default window size of 0 - notice if window is 0, no windowing happens" << endl;
          depth_win = 0;
      }
      if (front_win == -1) {
          cout << "No valid value provided for key 'front-window'..." << endl;
          cout << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis" << endl;
          front_win = 0;
      }
      if (back_win == -1) {
          cout << "No valid value provided for key 'back-window'..." << endl;
          cout << "Using default window size of 0- notice if both window in an axis are 0, no windowing happens on that axis" << endl;
          back_win = 0;
      }
  }
  auto *parameters = new AcousticOmpComputationParameters(half_length);
  parameters->boundary_length = boundary_length;
  parameters->dt_relax = dt_relax;
  parameters->source_frequency = source_frequency;
  parameters->n_threads = n_threads;
  parameters->block_x = block_x;
  parameters->block_z = block_z;
  parameters->block_y = block_y;
  parameters->use_window = use_window == 1;
  parameters->left_window = left_win;
  parameters->right_window = right_win;
  parameters->depth_window = depth_win;
  parameters->front_window = front_win;
  parameters->back_window = back_win;
  PrintParameters(parameters);
  omp_set_num_threads(parameters->n_threads);
  return parameters;
}