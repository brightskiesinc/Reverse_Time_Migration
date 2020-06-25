//
// Created by amr on 25/01/2020.
//

#include "trace_writer_parser.h"

TraceWriter *parse_trace_writer_acoustic_iso_openmp_second(ConfigMap map) {
  TraceWriter *trace_writer = nullptr;
  if (map.find("trace-writer") == map.end()) {
    cout << "No entry for trace-writer key : supported values [ binary ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["trace-writer"] == "binary") {
    trace_writer = new BinaryTraceWriter();
    cout << "Using binary trace writer..." << std::endl;
  } else {
    cout << "Invalid value for trace-writer key : supported values [ binary ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return trace_writer;
}
