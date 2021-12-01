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

#ifndef STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR
#define STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR

#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>
#include <operations/common/DataTypes.h>
#include <operations/components/Components.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <vector>
#include <string>

namespace stbx {
    namespace generators {

        class CallbacksGenerator {
        public:
            CallbacksGenerator(const std::string &aWritePath, nlohmann::json &aMap);

            ~CallbacksGenerator();

            operations::helpers::callbacks::CallbackCollection *
            GenerateCallbacks();

        private:
            struct WritersBooleans {
                bool WriteParams = false,
                        WriteForward = false,
                        WriteBackward = false,
                        WriteReverse = false,
                        WriteMigration = false,
                        WriteReExtendedParams = false,
                        WriteSingleShotCorrelation = false,
                        WriteEachStackedShot = false,
                        WriteTracesRaw = false,
                        WriteTracesPreprocessed = false;
                std::vector<std::string> VecParams;
                std::vector<std::string> VecReExtendedParams;
            };

        private:
            CallbacksGenerator::WritersBooleans
            GenerateWritersConfiguration();

            void
            GetNormCallback();

            void
            GetWriterCallback();

        private:
            std::string mWritePath;
            nlohmann::json mMap;
            operations::helpers::callbacks::CallbackCollection *mpCollection;
        };
    }// namespace generators
}//namesapce stbx

#endif //STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR

