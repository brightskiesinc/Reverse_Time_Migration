//
// Created by amr on 25/01/2020.
//

#ifndef ACOUSTIC2ND_RTM_BOUNDARY_MANAGER_PARSER_H
#define ACOUSTIC2ND_RTM_BOUNDARY_MANAGER_PARSER_H

#include <concrete-components/acoustic_second_components.h>
#include <parsers/configuration_parser.h>

BoundaryManager *
parse_boundary_manager_acoustic_iso_openmp_second(ConfigMap map);

#endif // ACOUSTIC2ND_RTM_BOUNDARY_MANAGER_PARSER_H
