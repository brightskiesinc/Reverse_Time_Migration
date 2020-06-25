//
// Created by amr on 25/01/2020.
//

#include "trace_manager_parser.h"

TraceManager *parse_trace_manager_acoustic_iso_openmp_second(ConfigMap map) {
  TraceManager *traceManager = nullptr;
  if (map.find("trace-manager") == map.end()) {
    cout << "No entry for trace-manager key : supported values [ binary segy]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["trace-manager"] == "binary") {
    traceManager = new BinaryTraceManager();
    cout << "Using binary trace manager..." << std::endl;
  } else if (map["trace-manager"] == "segy") {
    traceManager = new SeismicTraceManager();
    cout << "Using segy trace manager..." << std::endl;
  } else {
    cout << "Invalid value for trace-manager key : supported values [ binary "
            "segy ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return traceManager;
}
