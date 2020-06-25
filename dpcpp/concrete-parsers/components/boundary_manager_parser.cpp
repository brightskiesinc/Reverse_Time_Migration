//
// Created by amr on 25/01/2020.
//

#include "boundary_manager_parser.h"

BoundaryManager *
parse_boundary_manager_acoustic_iso_openmp_second(ConfigMap map) {
  BoundaryManager *boundary_manager = nullptr;
  if (map.find("boundary-manager") == map.end()) {
    cout << "No entry for boundary-manager key : supported values [ none | "
            "random ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["boundary-manager"] == "none") {
    boundary_manager = new NoBoundaryManager(false);
    cout << "Not utilizing a boundary condition..." << std::endl;
  } else if (map["boundary-manager"] == "random") {
    boundary_manager = new RandomBoundaryManager(false);
    cout << "Using a random boundary condition..." << std::endl;
  } else {
    cout << "Invalid value for boundary-manager key : supported values [ none "
            "| random ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return boundary_manager;
}