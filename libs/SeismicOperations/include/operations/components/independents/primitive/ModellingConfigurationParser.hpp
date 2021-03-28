//
// Created by amr-nasr on 12/11/2019.
//

#ifndef OPERATIONS_LIB_COMPONENTS_MODELLING_CONFIGURATION_PARSER_HPP
#define OPERATIONS_LIB_COMPONENTS_MODELLING_CONFIGURATION_PARSER_HPP

#include "operations/components/independents/interface/Component.hpp"

#include "operations/common/DataTypes.h"

namespace operations {
    namespace components {

        class ModellingConfigurationParser : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~ModellingConfigurationParser() {};

            /**
             * @brief Parses a file with the proper format as the modelling configuration.
             *
             * @param[in] file_path
             * The file to be parsed.
             *
             * @param[in] is_2D
             *
             * @return[out] ModellingConfiguration
             * The parsed modelling configuration.
             * ModellingConfiguration: Parameters needed for the modelling operation.
             */
            virtual ModellingConfiguration ParseConfiguration(
                    std::string file_path, bool is_2D) = 0;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_MODELLING_CONFIGURATION_PARSER_HPP
