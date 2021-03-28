//
// Created by amr-nasr on 25/01/2020.
//

#ifndef SEISMIC_TOOLBOX_UTILS_CMD_CMD_PARSER_UTILS_H
#define SEISMIC_TOOLBOX_UTILS_CMD_CMD_PARSER_UTILS_H

#include <string>

namespace stbx {
    namespace utils {

        void parse_args(std::string &parameter_file,
                        std::string &configuration_file,
                        std::string &callback_file,
                        std::string &pipeline,
                        std::string &write_path,
                        int argc, char **argv);

    }//namespace utils
}//namespace stbx

#endif //SEISMIC_TOOLBOX_UTILS_CMD_CMD_PARSER_UTILS_H