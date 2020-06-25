//
// Created by amr on 25/01/2020.
//
#include "forward_collector_parser.h"

ForwardCollector *
parse_forward_collector_acoustic_iso_openmp_second(ConfigMap map,
                                                   string write_path) {
  ForwardCollector *forward_collector = nullptr;
  if (map.find("forward-collector") == map.end()) {
    cout << "No entry for forward-collector key : supported values [ two | "
            "three | two-compression | optimal-checkpointing ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["forward-collector"] == "two") {
    forward_collector = new TwoPropagation(false, write_path);
    cout << "Using two propagation mechanism..." << endl;
  } else if (map["forward-collector"] == "three") {
    forward_collector =
        new ReversePropagation(new SecondOrderComputationKernel());
    cout << "Using three propagation(Reverse Propagation) mechanism..." << endl;
    cout << "Notice : only works correctly for random/no boundary conditions"
         << endl;
  } else if (map["forward-collector"] == "two-compression") {
    float zfp_tolerance = 0.01f;
    int zfp_parallel = 1;
    bool zfp_is_relative = false;
    if (map.find("forward-collector.zfp-tolerance") != map.end()) {
      cout << "Parsing user defined zfp tolerance" << endl;
      zfp_tolerance = stof(map["forward-collector.zfp-tolerance"]);
    }
    if (map.find("forward-collector.zfp-parallel") != map.end()) {
      cout << "Parsing user defined zfp parallel value" << endl;
      int parallel = stoi(map["forward-collector.zfp-parallel"]);
      if (parallel != 0 && parallel != 1) {
        cout << "Invalid values for zfp-parallel property : Only 1 or 0 are "
                "supported..."
             << endl;
        cout << "Terminating..." << endl;
        exit(0);
      }
      zfp_parallel = parallel;
    }
    if (map.find("forward-collector.zfp-relative") != map.end()) {
      cout << "Parsing user defined zfp relative value" << endl;
      int relative = stoi(map["forward-collector.zfp-relative"]);
      if (relative != 0 && relative != 1) {
        cout << "Invalid values for zfp-relative property : Only 1 or 0 are "
                "supported..."
             << endl;
        cout << "Terminating..." << endl;
        exit(0);
      }
      zfp_is_relative = relative == 1;
    }
    forward_collector = new TwoPropagation(true, write_path, zfp_tolerance,
                                           zfp_parallel, zfp_is_relative);
    cout << "Using two propagation with compression mechanism..." << endl;
    cout << "\tZFP tolerance : " << zfp_tolerance << endl;
    cout << "\tZFP parallel use : " << zfp_parallel << endl;
    cout << "\tZFP use relative error : " << zfp_is_relative << endl;
  } else if (map["forward-collector"] == "optimal-checkpointing") {
    forward_collector =
        new ReverseInjectionPropagation(new SecondOrderComputationKernel());
    cout << "Using three propagation with boundary saving mechanism..." << endl;
  } else {
    cout << "Invalid value for forward-collector key : supported values [ two "
            "| three | two-compression | optimal-checkpointing ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return forward_collector;
}

ForwardCollector *
parse_forward_collector_acoustic_iso_openmp_first(ConfigMap map,
                                                  string write_path) {
  ForwardCollector *forward_collector = nullptr;
  if (map.find("forward-collector") == map.end()) {
    cout << "No entry for forward-collector key : supported values [ two | "
            "three | two-compression | optimal-checkpointing ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["forward-collector"] == "two") {
    forward_collector = new StaggeredTwoPropagation(false, write_path);
    cout << "Using two propagation mechanism..." << endl;
  } else if (map["forward-collector"] == "three") {
    forward_collector =
        new StaggeredReversePropagation(new StaggeredComputationKernel(false));
    cout << "Using three propagation(Reverse Propagation) mechanism..." << endl;
    cout << "Notice : only works correctly for random/no boundary conditions"
         << endl;
  } else if (map["forward-collector"] == "two-compression") {
    float zfp_tolerance = 0.01f;
    int zfp_parallel = 1;
    bool zfp_is_relative = false;
    if (map.find("forward-collector.zfp-tolerance") != map.end()) {
      cout << "Parsing user defined zfp tolerance" << endl;
      zfp_tolerance = stof(map["forward-collector.zfp-tolerance"]);
    }
    if (map.find("forward-collector.zfp-parallel") != map.end()) {
      cout << "Parsing user defined zfp parallel value" << endl;
      int parallel = stoi(map["forward-collector.zfp-parallel"]);
      if (parallel != 0 && parallel != 1) {
        cout << "Invalid values for zfp-parallel property : Only 1 or 0 are "
                "supported..."
             << endl;
        cout << "Terminating..." << endl;
        exit(0);
      }
      zfp_parallel = parallel;
    }
    if (map.find("forward-collector.zfp-relative") != map.end()) {
      cout << "Parsing user defined zfp relative value" << endl;
      int relative = stoi(map["forward-collector.zfp-relative"]);
      if (relative != 0 && relative != 1) {
        cout << "Invalid values for zfp-relative property : Only 1 or 0 are "
                "supported..."
             << endl;
        cout << "Terminating..." << endl;
        exit(0);
      }
      zfp_is_relative = relative == 1;
    }
    forward_collector = new StaggeredTwoPropagation(
        true, write_path, zfp_tolerance, zfp_parallel, zfp_is_relative);
    cout << "Using two propagation with compression mechanism..." << endl;
    cout << "\tZFP tolerance : " << zfp_tolerance << endl;
    cout << "\tZFP parallel use : " << zfp_parallel << endl;
    cout << "\tZFP use relative error : " << zfp_is_relative << endl;
  } else if (map["forward-collector"] == "optimal-checkpointing") {
    forward_collector = new StaggeredReverseInjectionPropagation(
        new StaggeredComputationKernel(false));
    cout << "Using three propagation with boundary saving mechanism..." << endl;
  } else {
    cout << "Invalid value for forward-collector key : supported values [ two "
            "| three | two-compression | optimal-checkpointing ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return forward_collector;
}