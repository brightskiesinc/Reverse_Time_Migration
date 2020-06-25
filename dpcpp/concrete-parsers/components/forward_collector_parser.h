//
// Created by amr on 25/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_FORWARD_COLLECTOR_PARSER_H
#define ACOUSTIC2ND_RTM_FORWARD_COLLECTOR_PARSER_H

#include <concrete-components/acoustic_second_components.h>
#include <parsers/configuration_parser.h>

ForwardCollector *
parse_forward_collector_acoustic_iso_openmp_second(ConfigMap map,
                                                   std::string write_path);

#endif // ACOUSTIC2ND_RTM_FORWARD_COLLECTOR_PARSER_H
