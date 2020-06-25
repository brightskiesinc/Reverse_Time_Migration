//
// Created by amr on 25/01/2020.
//
#include "forward_collector_parser.h"

ForwardCollector *
parse_forward_collector_acoustic_iso_openmp_second(ConfigMap map,
                                                   string write_path) {
  ForwardCollector *forward_collector = nullptr;
  if (map.find("forward-collector") == map.end()) {
    cout << "No entry for forward-collector key : supported values [ three ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["forward-collector"] == "three") {
    forward_collector =
        new ReversePropagation(new SecondOrderComputationKernel());
    cout << "Using three propagation(Reverse Propagation) mechanism..."
         << std::endl;
    cout << "Notice : only works correctly for random/no boundary conditions"
         << std::endl;
  } else {
    cout << "Invalid value for forward-collector key : supported values [ "
            "three ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return forward_collector;
}
