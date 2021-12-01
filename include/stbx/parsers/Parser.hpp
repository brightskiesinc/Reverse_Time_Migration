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

#ifndef SEISMIC_TOOLBOX_PARSERS_PARSER_HPP
#define SEISMIC_TOOLBOX_PARSERS_PARSER_HPP

#include <operations/common/Singleton.tpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

namespace stbx {
    namespace parsers {

        /**
         * @brief Parser class responsible for registering .json files
         * to be parsed or a whole folder to parse all .json files inside.
         * <br>
         * Variant parsing functions are implemented to provide extraction of
         * some components from parsed map and return specific object instance.
         */
        class Parser : public operations::common::Singleton<Parser> {
        public:
            friend class Singleton<Parser>;

        public:
            /**
             * @brief Builds cne big map containing all registered files
             * key values pairs.
             * @return Map : json -> json::Value
             */
            nlohmann::json
            BuildMap();

            /**
             * @brief Registers a given .json files to be parsed.
             * @param folder : string       File path to be parsed
             */
            void
            RegisterFile(const std::string &file);

            const std::vector<std::string> &
            GetFiles() const;

            /**
             * @brief Registers all .json files found inside a folder.
             * @param folder : string       Folder path to be parsed
             */
            void
            RegisterFolder(const std::string &folder);

            /**
             * @brief Map getter.
             * @return Map : json -> json::Value
             */
            nlohmann::json
            GetMap();

        private:
            /// Map that holds configurations key value pairs
            nlohmann::json mMap;
            /// Registered files to be parsed.
            std::vector<std::string> mFiles;
        };

    }//namespace parsers
}//namespace stbx

#endif //SEISMIC_TOOLBOX_PARSERS_PARSER_HPP
