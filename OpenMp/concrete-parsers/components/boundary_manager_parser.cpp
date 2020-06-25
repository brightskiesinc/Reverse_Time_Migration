//
// Created by amr on 25/01/2020.
//

#include "boundary_manager_parser.h"

BoundaryManager *
parse_boundary_manager_acoustic_iso_openmp_second(ConfigMap map) {
  BoundaryManager *boundary_manager = nullptr;
  if (map.find("boundary-manager") == map.end()) {
    cout << "No entry for boundary-manager key : supported values [ none | "
            "random | sponge | cpml ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["boundary-manager"] == "none") {
    boundary_manager = new NoBoundaryManager(false);
    cout << "Not utilizing a boundary condition..." << endl;
  } else if (map["boundary-manager"] == "random") {
    boundary_manager = new RandomBoundaryManager(false);
    cout << "Using a random boundary condition..." << endl;
  } else if (map["boundary-manager"] == "sponge") {
    bool use_top_layer = true;
    if (map.find("boundary-manager.use-top-layer") != map.end()) {
      cout << "Parsing top layer setting" << endl;
      if (map["boundary-manager.use-top-layer"] == "yes") {
        cout << "Using top boundary layer for forward modelling. To disable it "
                "set <boundary-manager.use-top-layer=no>"
             << endl;
      } else {
        use_top_layer = false;
        cout << "Not using top boundary layer for forward modelling. To enable "
                "it set <boundary-manager.use-top-layer=yes>"
             << endl;
      }
    } else {
      cout << "Using top boundary layer for forward modelling. To disable it "
              "set <boundary-manager.use-top-layer=no>"
           << endl;
    }
    boundary_manager = new SpongeBoundaryManager(use_top_layer, false);
    cout << "Using a sponge boundary condition..." << endl;
  } else if (map["boundary-manager"] == "cpml") {
    float reflect_coeff = 0.1;
    float shift_ratio = 0.1;
    float relax_cp = 1;
    bool use_top_layer = true;
    if (map.find("boundary-manager.use-top-layer") != map.end()) {
      cout << "Parsing top layer setting" << endl;
      if (map["boundary-manager.use-top-layer"] == "yes") {
        cout << "Using top boundary layer for forward modelling" << endl;
      } else {
        use_top_layer = false;
        cout << "Not using top boundary layer for forward modelling. To enable "
                "it set <boundary-manager.use-top-layer=yes>"
             << endl;
      }
    } else {
      cout << "Using top boundary layer for forward modelling" << endl;
    }
    if (map.find("boundary-manager.reflect-coeff") != map.end()) {
      cout << "Parsing user defined reflect coefficient" << endl;
      reflect_coeff = stof(map["boundary-manager.reflect-coeff"]);
    }
    if (map.find("boundary-manager.shift-ratio") != map.end()) {
      cout << "Parsing user defined shift ratio" << endl;
      shift_ratio = stof(map["boundary-manager.shift-ratio"]);
    }
    if (map.find("boundary-manager.relax-cp") != map.end()) {
      cout << "Parsing user defined relax coefficient" << endl;
      relax_cp = stof(map["boundary-manager.relax-cp"]);
    }
    boundary_manager = new CPMLBoundaryManager(use_top_layer, reflect_coeff,
                                               shift_ratio, relax_cp);
    cout << "Using CPML as a boundary condition..." << endl;
    cout << "\tCPML reflect coeff is " << reflect_coeff << endl;
    cout << "\tCPML shift ratio is " << shift_ratio << endl;
    cout << "\tCPML relax coefficient is " << relax_cp << endl;
  } else {
    cout << "Invalid value for boundary-manager key : supported values [ none "
            "| random | sponge | cpml ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return boundary_manager;
}

BoundaryManager *
parse_boundary_manager_acoustic_iso_openmp_first(ConfigMap map) {
  BoundaryManager *boundary_manager = nullptr;
  if (map.find("boundary-manager") == map.end()) {
    cout << "No entry for boundary-manager key : supported values [ none | "
            "random | sponge | cpml ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["boundary-manager"] == "none") {
    boundary_manager = new NoBoundaryManager(true);
    cout << "Not utilizing a boundary condition..." << endl;
  } else if (map["boundary-manager"] == "random") {
    boundary_manager = new RandomBoundaryManager(true);
    cout << "Using a random boundary condition..." << endl;
  } else if (map["boundary-manager"] == "sponge") {
    bool use_top_layer = true;
    if (map.find("boundary-manager.use-top-layer") != map.end()) {
      cout << "Parsing top layer setting" << endl;
      if (map["boundary-manager.use-top-layer"] == "yes") {
        cout << "Using top boundary layer for forward modelling. To disable it "
                "set <boundary-manager.use-top-layer=no>"
             << endl;
      } else {
        use_top_layer = false;
        cout << "Not using top boundary layer for forward modelling. To enable "
                "it set <boundary-manager.use-top-layer=yes>"
             << endl;
      }
    } else {
      cout << "Using top boundary layer for forward modelling. To disable it "
              "set <boundary-manager.use-top-layer=no>"
           << endl;
    }
    boundary_manager = new SpongeBoundaryManager(use_top_layer, true);
    cout << "Using a sponge boundary condition..." << endl;
  } else if (map["boundary-manager"] == "cpml") {
    float reflect_coeff = 0.1;
    float shift_ratio = 0.1;
    float relax_cp = 1;
    bool use_top_layer = true;
    if (map.find("boundary-manager.use-top-layer") != map.end()) {
      cout << "Parsing top layer setting" << endl;
      if (map["boundary-manager.use-top-layer"] == "yes") {
        cout << "Using top boundary layer for forward modelling. To disable it "
                "set <boundary-manager.use-top-layer=no>"
             << endl;
      } else {
        use_top_layer = false;
        cout << "Not using top boundary layer for forward modelling. To enable "
                "it set <boundary-manager.use-top-layer=yes>"
             << endl;
      }
    } else {
      cout << "Using top boundary layer for forward modelling. To disable it "
              "set <boundary-manager.use-top-layer=no>"
           << endl;
    }
    if (map.find("boundary-manager.reflect-coeff") != map.end()) {
      cout << "Parsing user defined reflect coefficient" << endl;
      reflect_coeff = stof(map["boundary-manager.reflect-coeff"]);
    }
    if (map.find("boundary-manager.shift-ratio") != map.end()) {
      cout << "Parsing user defined shift ratio" << endl;
      shift_ratio = stof(map["boundary-manager.shift-ratio"]);
    }
    if (map.find("boundary-manager.relax-cp") != map.end()) {
      cout << "Parsing user defined relax coefficient" << endl;
      relax_cp = stof(map["boundary-manager.relax-cp"]);
    }
    boundary_manager = new StaggeredCPMLBoundaryManager(
        use_top_layer, reflect_coeff, shift_ratio, relax_cp);
    cout << "Using CPML as a boundary condition..." << endl;
    cout << "\tCPML reflect coeff is " << reflect_coeff << endl;
    cout << "\tCPML shift ratio is " << shift_ratio << endl;
    cout << "\tCPML relax coefficient is " << relax_cp << endl;
  } else {
    cout << "Invalid value for boundary-manager key : supported values [ none "
            "| random | sponge | cpml ]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return boundary_manager;
}