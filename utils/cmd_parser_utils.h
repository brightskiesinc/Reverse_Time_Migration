//
// Created by amr on 25/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_CMD_PARSER_UTILS_H
#define ACOUSTIC2ND_RTM_CMD_PARSER_UTILS_H

#include <string>

void parse_args_engine(std::string &parameter_file,
                       std::string &configuration_file,
                       std::string &callback_file, std::string &write_path,
                       int argc, char *argv[], const char *message);
#endif