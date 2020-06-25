//
// Created by amr on 25/01/2020.
//

#include "correlation_kernel_parser.h"

CorrelationKernel *
parse_correlation_kernel_acoustic_iso_openmp_second(ConfigMap map) {
  CorrelationKernel *correlation_kernel = nullptr;
  if (map.find("correlation-kernel") == map.end()) {
    cout << "No entry for correlation-kernel key : supported values [ "
            "cross-correlation ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["correlation-kernel"] == "cross-correlation") {
    correlation_kernel = new CrossCorrelationKernel();
    cout << "Using Cross-Correlation as correlation kernel..." << std::endl;
  } else {
    cout << "Invalid value for correlation-kernel key : supported values [ "
            "cross-correlation ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return correlation_kernel;
}