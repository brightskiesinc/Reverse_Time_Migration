//
// Created by marwan-elsafty on 08/02/2021.
//

#ifndef STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR
#define STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR

#include <operations/helpers/callbacks/primitive/CallbackCollection.hpp>
#include <operations/common/DataTypes.h>
#include <operations/components/Components.hpp>

#include <libraries/nlohmann/json.hpp>

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
            GenerateWriters();

            void
            GetImageCallback();

            void
            GetCsvCallback();

            void
            GetNormCallback();

            void
            GetSegyCallback();

            void
            GetSuCallback();

            void
            GetBinaryCallback();

        private:
            std::string mWritePath;
            nlohmann::json mMap;
            operations::helpers::callbacks::CallbackCollection *mpCollection;
        };
    }// namespace generators
}//namesapce stbx

#endif //STBX_GENERATORS_CALLBACKS_CALLBACKS_GENERATOR

