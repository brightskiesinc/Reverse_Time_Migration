//
// Created by amr on 25/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_CALLBACK_PARSER_H
#define ACOUSTIC2ND_RTM_CALLBACK_PARSER_H

#include <skeleton/helpers/callbacks/callback_collection.h>

#include "configuration_parser.h"

CallbackCollection *parse_callbacks(std::string filename,
                                    std::string write_path);

#endif // ACOUSTIC2ND_RTM_CALLBACK_PARSER_H
