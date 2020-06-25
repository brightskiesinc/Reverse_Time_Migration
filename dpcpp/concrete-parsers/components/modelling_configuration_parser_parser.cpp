//
// Created by amr on 25/01/2020.
//

#include "modelling_configuration_parser_parser.h"

ModellingConfigurationParser *
parse_modelling_configuration_parser_acoustic_iso_openmp_second(ConfigMap map) {
  ModellingConfigurationParser *modelling_configuration_parser = nullptr;
  if (map.find("modelling-configuration-parser") == map.end()) {
    cout << "No entry for modelling-configuration-parser key : supported "
            "values [ text ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  } else if (map["modelling-configuration-parser"] == "text") {
    modelling_configuration_parser = new TextModellingConfigurationParser();
    cout << "Using text modelling configuration parser..." << std::endl;
  } else {
    cout << "Invalid value for modelling-configuration-parser key : supported "
            "values [ text ]"
         << std::endl;
    cout << "Terminating..." << std::endl;
    exit(0);
  }
  return modelling_configuration_parser;
}
