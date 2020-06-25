//
// Created by amr on 24/01/2020.
//

#include "model_handler_parser.h"

ModelHandler *parse_model_handler_acoustic_iso_openmp_second(ConfigMap map) {
  ModelHandler *modelHandler = nullptr;
  if (map.find("model-handler") == map.end()) {
    cout << "No entry for model-handler key : supported values [ homogenous "
            "segy]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["model-handler"] == "homogenous") {
    modelHandler = new HomogenousModelHandler(false);
    cout << "Using Homogenous model handler..." << endl;
  } else if (map["model-handler"] == "segy") {
    modelHandler = new SeismicModelHandler(false);
    cout << "Using Segy model handler..." << endl;
  } else {
    cout << "Invalid value for model-handler key : supported values [ "
            "homogenous segy]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return modelHandler;
}

ModelHandler *parse_model_handler_acoustic_iso_openmp_first(ConfigMap map) {
  ModelHandler *modelHandler = nullptr;
  if (map.find("model-handler") == map.end()) {
    cout << "No entry for model-handler key : supported values [ homogenous "
            "segy]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  } else if (map["model-handler"] == "homogenous") {
    modelHandler = new HomogenousModelHandler(true);
    cout << "Using Homogenous model handler..." << endl;
  } else if (map["model-handler"] == "segy") {
    modelHandler = new SeismicModelHandler(true);
    cout << "Using Segy model handler..." << endl;
  } else {
    cout << "Invalid value for model-handler key : supported values [ "
            "homogenous segy]"
         << endl;
    cout << "Terminating..." << endl;
    exit(0);
  }
  return modelHandler;
}