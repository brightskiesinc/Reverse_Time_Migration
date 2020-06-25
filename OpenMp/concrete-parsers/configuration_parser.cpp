//
// Created by amr on 22/01/2020.
//

#include "configuration_parser.h"

#include "components/boundary_manager_parser.h"
#include "components/correlation_kernel_parser.h"
#include "components/forward_collector_parser.h"
#include "components/model_handler_parser.h"
#include "components/modelling_configuration_parser_parser.h"
#include "components/source_injector_parser.h"
#include "components/trace_manager_parser.h"
#include "components/trace_writer_parser.h"

#include <concrete-components/acoustic_second_components.h>

#include <fstream>
#include <iostream>

using namespace std;

vector<string> read_lines(string filename) {
  vector<string> lines;
  ifstream line_file(filename);
  if (!line_file.is_open()) {
    cout << "Couldn't open file : '" << filename << "' ! Terminating program"
         << endl;
    exit(0);
  }
  string temp_line;
  while (line_file >> temp_line) {
    lines.push_back(temp_line);
  }
  return lines;
}

PHYSICS parse_physics(ConfigMap map) {
  PHYSICS physics = ACOUSTIC;
  if (map.find("physics") == map.end()) {
    cout << "No entry for physics key : supported values [ acoustic ]" << endl;
    cout << "Using acoustic for physics" << endl;
  } else if (map["physics"] == "acoustic") {
    physics = ACOUSTIC;
    cout << "Using acoustic for physics" << endl;
  } else {
    cout << "Invalid value for physics key : supported values [ acoustic ]"
         << endl;
    cout << "Using acoustic for physics" << endl;
  }
  return physics;
}

APPROXIMATION parse_approximation(ConfigMap map) {
  APPROXIMATION approximation = ISOTROPIC;
  if (map.find("approximation") == map.end()) {
    cout << "No entry for approximation key : supported values [ isotropic ]"
         << endl;
    cout << "Using isotropic as approximation" << endl;
  } else if (map["approximation"] == "isotropic") {
    approximation = ISOTROPIC;
    cout << "Using isotropic as approximation" << endl;
  } else {
    cout << "Invalid value for approximation key : supported values [ "
            "isotropic ]"
         << endl;
    cout << "Using isotropic as approximation" << endl;
  }
  return approximation;
}

EQUATION_ORDER parse_equation_order(ConfigMap map) {
  EQUATION_ORDER order = SECOND;
  if (map.find("equation-order") == map.end()) {
    cout << "No entry for equation-order key : supported values [ second | "
            "first ]"
         << endl;
    cout << "Using second order wave equation" << endl;
  } else if (map["equation-order"] == "second") {
    order = SECOND;
    cout << "Using second order wave equation" << endl;
  } else if (map["equation-order"] == "first") {
    order = FIRST;
    cout << "Using first order wave equation" << endl;
  } else {
    cout << "Invalid value for equation-order key : supported values [ second "
            "| first ]"
         << endl;
    cout << "Using second order wave equation" << endl;
  }
  return order;
}

GRID_SAMPLING parse_grid_sampling(ConfigMap map) {
  GRID_SAMPLING sampling = UNIFORM;
  if (map.find("grid-sampling") == map.end()) {
    cout << "No entry for grid-sampling key : supported values [ uniform ]"
         << endl;
    cout << "Using uniform grid sampling" << endl;
  } else if (map["grid-sampling"] == "uniform") {
    sampling = UNIFORM;
    cout << "Using uniform grid sampling" << endl;
  } else {
    cout << "Invalid value for grid-sampling key : supported values [ uniform ]"
         << endl;
    cout << "Using uniform grid sampling" << endl;
  }
  return sampling;
}

vector<string> parse_model_files(ConfigMap map) {
  vector<string> model_files;
  if (map.find("models-list") == map.end()) {
    cout << "No entry for models-list key : a value providing the filename"
            " to the file that contains the directories of model files(each in "
            "a line) must be provided"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else {
    model_files = read_lines(map["models-list"]);
    cout << "The following model files were detected : " << endl;
    for (int i = 0; i < model_files.size(); i++) {
      cout << "\t" << (i + 1) << ". " << model_files[i] << endl;
    }
  }
  return model_files;
}

string parse_trace_file(ConfigMap map) {
  string filename;
  if (map.find("trace-file") == map.end()) {
    cout << "No entry for trace-file key : a value providing the filename"
            " to the file that the recorded traces will be written into must "
            "be provided"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else {
    filename = map["trace-file"];
    cout << "Trace output file : '" << filename << "'" << endl;
  }
  return filename;
}

string parse_modelling_configuration_file(ConfigMap map) {
  string filename;
  if (map.find("modelling-configuration-file") == map.end()) {
    cout << "No entry for modelling-configuration-file : a value providing the "
            "filename"
            " to read the runtime configuration of the modelling process eg: "
            "source point, receiver distribution"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else {
    filename = map["modelling-configuration-file"];
    cout << "Modelling runtime configuration file : '" << filename << "'"
         << endl;
  }
  return filename;
}

void parse_trace_files(ConfigMap map, EngineConfiguration *configuration) {
  if (map.find("traces-list") == map.end()) {
    cout << "No entry for traces-list key : a value providing the filename"
            " to the file that contains the directories of the traces "
            "files(each in a line) must be provided.\n"
         << "The file should have the start shot id in the first line, and the "
            "ending shot id(exclusive) in the second line."
         << "\nThe following lines should be the directories of the traces "
            "file(each in a line"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else {
    vector<string> traces_files = read_lines(map["traces-list"]);
    cout << "Parsing minimum shot id..." << endl;
    if (traces_files[0] == "none") {
        configuration->sort_min = 0;
    } else {
        try {
            configuration->sort_min = stoi(traces_files[0]);
        }
        catch(std::invalid_argument& e){
            configuration->sort_min = 0;
            cout << "Couldn't parse the minimum shot ID to process... Setting to 0" << std::endl;
        }
    }
    traces_files.erase(traces_files.begin());
    cout << "Parsing maximum shot id..." << endl;
    if (traces_files[0] == "none") {
        configuration->sort_max = UINT_MAX;
    } else {
        try {
            configuration->sort_max = stoi(traces_files[0]);
        }
        catch(std::invalid_argument& e){
            configuration->sort_max = UINT_MAX;
            cout << "Couldn't parse the maximum shot ID to process... Setting to " << UINT_MAX << std::endl;
        }
    }
    configuration->sort_key = "CSR";
    traces_files.erase(traces_files.begin());
    cout << "The following trace files were detected : " << endl;
    for (int i = 0; i < traces_files.size(); i++) {
      cout << "\t" << (i + 1) << ". " << traces_files[i] << endl;
    }
    cout << "Minimum allowable ID : " << configuration->sort_min << endl;
    cout << "Maximum allowable ID : " << configuration->sort_max << endl;
    configuration->trace_files = traces_files;
  }
}

ConfigMap build_map(string filename) {
  ifstream conf_file(filename);
  ConfigMap map;
  string temp_line;
  while (conf_file >> temp_line) {
    if (temp_line[0] == '#') {
      continue;
    }
    string key = temp_line.substr(0, temp_line.find("="));
    string value = temp_line.substr(temp_line.find("=") + 1);
    map[key] = value;
  }
  return map;
}

EngineConfiguration *parse_rtm_configuration(std::string filename,
                                             std::string write_path) {
  ConfigMap conf = build_map(filename);
  PHYSICS physics = parse_physics(conf);
  APPROXIMATION approximation = parse_approximation(conf);
  EQUATION_ORDER order = parse_equation_order(conf);
  GRID_SAMPLING sampling = parse_grid_sampling(conf);
  EngineConfiguration *configuration = new EngineConfiguration();
  cout << "Reading model files..." << endl;
  configuration->model_files = parse_model_files(conf);
  cout << "Reading trace files..." << endl;
  parse_trace_files(conf, configuration);
  if (physics == ACOUSTIC && approximation == ISOTROPIC && order == SECOND &&
      sampling == UNIFORM) {
    configuration->model_handler =
        parse_model_handler_acoustic_iso_openmp_second(conf);
    configuration->source_injector =
        parse_source_injector_acoustic_iso_openmp_second(conf);
    configuration->computation_kernel = new SecondOrderComputationKernel();
    cout << "Using Second Order Computation Kernel..." << endl;
    configuration->boundary_manager =
        parse_boundary_manager_acoustic_iso_openmp_second(conf);
    configuration->forward_collector =
        parse_forward_collector_acoustic_iso_openmp_second(conf, write_path);
    configuration->correlation_kernel =
        parse_correlation_kernel_acoustic_iso_openmp_second(conf);
    configuration->trace_manager =
        parse_trace_manager_acoustic_iso_openmp_second(conf);
  } else if (physics == ACOUSTIC && approximation == ISOTROPIC &&
             order == FIRST && sampling == UNIFORM) {
    configuration->model_handler =
        parse_model_handler_acoustic_iso_openmp_first(conf);
    configuration->source_injector =
        parse_source_injector_acoustic_iso_openmp_first(conf);
    configuration->computation_kernel = new StaggeredComputationKernel(true);
    cout << "Using Staggered Computation Kernel..." << endl;
    configuration->boundary_manager =
        parse_boundary_manager_acoustic_iso_openmp_first(conf);
    configuration->forward_collector =
        parse_forward_collector_acoustic_iso_openmp_first(conf, write_path);
    configuration->correlation_kernel =
        parse_correlation_kernel_acoustic_iso_openmp_first(conf);
    configuration->trace_manager =
        parse_trace_manager_acoustic_iso_openmp_first(conf);
  } else {
    cout << "Unsupported settings" << endl;
    exit(0);
  }
  return configuration;
}

ModellingEngineConfiguration *
parse_modelling_configuration(std::string filename, std::string write_path) {
  ConfigMap conf = build_map(filename);
  ModellingEngineConfiguration *configuration =
      new ModellingEngineConfiguration();
  PHYSICS physics = parse_physics(conf);
  APPROXIMATION approximation = parse_approximation(conf);
  EQUATION_ORDER order = parse_equation_order(conf);
  GRID_SAMPLING sampling = parse_grid_sampling(conf);
  configuration->model_files = parse_model_files(conf);
  configuration->modelling_configuration_file =
      parse_modelling_configuration_file(conf);
  configuration->trace_file = parse_trace_file(conf);
  if (physics == ACOUSTIC && approximation == ISOTROPIC && order == SECOND &&
      sampling == UNIFORM) {
    configuration->model_handler =
        parse_model_handler_acoustic_iso_openmp_second(conf);
    configuration->source_injector =
        parse_source_injector_acoustic_iso_openmp_second(conf);
    configuration->computation_kernel = new SecondOrderComputationKernel();
    cout << "Using Second Order Computation Kernel..." << endl;
    configuration->boundary_manager =
        parse_boundary_manager_acoustic_iso_openmp_second(conf);
    configuration->modelling_configuration_parser =
        parse_modelling_configuration_parser_acoustic_iso_openmp_second(conf);
    configuration->trace_writer =
        parse_trace_writer_acoustic_iso_openmp_second(conf);
  } else if (physics == ACOUSTIC && approximation == ISOTROPIC &&
             order == FIRST && sampling == UNIFORM) {
    configuration->model_handler =
        parse_model_handler_acoustic_iso_openmp_first(conf);
    configuration->source_injector =
        parse_source_injector_acoustic_iso_openmp_first(conf);
    configuration->computation_kernel = new StaggeredComputationKernel(true);
    cout << "Using Staggered Computation Kernel..." << endl;
    configuration->boundary_manager =
        parse_boundary_manager_acoustic_iso_openmp_first(conf);
    configuration->modelling_configuration_parser =
        parse_modelling_configuration_parser_acoustic_iso_openmp_first(conf);
    configuration->trace_writer =
        parse_trace_writer_acoustic_iso_openmp_first(conf);
  } else {
    cout << "Unsupported settings" << endl;
    exit(0);
  }
  return configuration;
}