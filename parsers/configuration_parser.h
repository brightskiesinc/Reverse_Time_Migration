//
// Created by amr on 22/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_CONFIGURATION_PARSER_H
#define ACOUSTIC2ND_RTM_CONFIGURATION_PARSER_H

#include <skeleton/base/engine_configuration.h>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::unordered_map<std::string, std::string> ConfigMap;

enum PHYSICS { ACOUSTIC, ELASTIC };
enum APPROXIMATION { ISOTROPIC };
enum EQUATION_ORDER { SECOND, FIRST };
enum GRID_SAMPLING { UNIFORM, VARIABLE };

PHYSICS parse_physics(ConfigMap map);
APPROXIMATION parse_approximation(ConfigMap map);
EQUATION_ORDER parse_equation_order(ConfigMap map);
GRID_SAMPLING parse_grid_sampling(ConfigMap map);

std::vector<std::string> parse_model_files(ConfigMap map);
void parse_trace_files(ConfigMap map, EngineConfiguration *configuration);
std::string parse_trace_file(ConfigMap map);
std::string parse_modelling_configuration_file(ConfigMap map);

ConfigMap build_map(std::string filename);

std::vector<std::string> read_lines(std::string filename);

EngineConfiguration *parse_rtm_configuration(std::string filename,
                                             std::string write_path);

ModellingEngineConfiguration *
parse_modelling_configuration(std::string filename, std::string write_path);

#endif // ACOUSTIC2ND_RTM_CONFIGURATION_PARSER_H
