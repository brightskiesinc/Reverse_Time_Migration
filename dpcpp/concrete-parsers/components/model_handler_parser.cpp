//
// Created by amr on 24/01/2020.
//

#include "model_handler_parser.h"

ModelHandler *parse_model_handler_acoustic_iso_openmp_second(ConfigMap map) {
  ModelHandler *modelHandler = nullptr;
  if (map.find("model-handler") == map.end()) {
    cout << "No entry for model-handler key : supported values [ homogenous "
            "segy]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["model-handler"] == "homogenous") {
    modelHandler = new HomogenousModelHandler();
    cout << "Using Homogenous model handler..." << std::endl;
  } else if (map["model-handler"] == "segy") {
    modelHandler = new SeismicModelHandler(false);
    cout << "Using Segy model handler..." << std::endl;
  } else {
    cout << "Invalid value for model-handler key : supported values [ "
            "homogenous segy]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return modelHandler;
}
