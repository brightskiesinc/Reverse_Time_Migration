/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SEISMIC_TOOLBOX_PARSERS_ARGUMENTS_PARSER_H
#define SEISMIC_TOOLBOX_PARSERS_ARGUMENTS_PARSER_H

#include <operations/common/Singleton.tpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

namespace stbx {
    namespace parsers {

        /**
         * @brief ArgumentsParser class responsible for parsing argc and argv to the appropriate targets.
         */
        class ArgumentsParser {
        public:
            static void
            Parse(std::string &aParameterFile,
                  std::string &aConfigurationFile,
                  std::string &aCallbackFile,
                  std::string &aSystem,
                  std::string &aWritePath,
                  int argc, char **argv);

        private:
            /**
             * @brief Private constructor for preventing objects creation.
             */
            ArgumentsParser() = default;

            static void
            PrintHelp();
        };

    }//namespace parsers
}//namespace stbx

#endif //SEISMIC_TOOLBOX_PARSERS_ARGUMENTS_PARSER_H
