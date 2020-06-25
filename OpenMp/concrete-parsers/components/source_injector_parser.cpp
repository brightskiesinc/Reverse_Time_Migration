//
// Created by amr on 24/01/2020.
//

#include "source_injector_parser.h"

SourceInjector *
parse_source_injector_acoustic_iso_openmp_second(ConfigMap map) {
  SourceInjector *sourceInjector = nullptr;
  if (map.find("source-injector") == map.end()) {
    cout << "No entry for source-injector key : supported values [ ricker ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["source-injector"] == "ricker") {
    sourceInjector = new RickerSourceInjector();
    cout << "Using Ricker source injection..." << endl;
  } else {
    cout
        << "Invalid value for source-injector key : supported values [ ricker ]"
        << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return sourceInjector;
}

SourceInjector *parse_source_injector_acoustic_iso_openmp_first(ConfigMap map) {
  SourceInjector *sourceInjector = nullptr;
  if (map.find("source-injector") == map.end()) {
    cout << "No entry for source-injector key : supported values [ ricker ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["source-injector"] == "ricker") {
    sourceInjector = new RickerSourceInjector();
    cout << "Using Ricker source injection..." << endl;
  } else {
    cout
        << "Invalid value for source-injector key : supported values [ ricker ]"
        << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return sourceInjector;
}