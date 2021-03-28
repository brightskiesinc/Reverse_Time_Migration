//
// Created by zeyad-osama on 29/11/2020.
//

#ifndef THOTH_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP
#define THOTH_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP

#include <thoth/configurations/interface/ConfigurationMap.hpp>

#include <libraries/nlohmann/json.hpp>

namespace thoth {
    namespace configuration {
        /**
         * @brief
         * A JSON implementation of the configurations map interface.
         *
         * @tparam
         * The interface type used by the optimizer.
         */
        class JSONConfigurationMap : public ConfigurationMap {
        public:
            /**
             * @brief Copy constructor from a nlohmann::json to be able to
             * handle getting a JSON directly.
             *
             * @param[in] aJson
             * The json given as task information.
             */
            explicit JSONConfigurationMap(nlohmann::json aJson);

            /**
             * @brief Default destructor.
             */
            ~JSONConfigurationMap() override = default;

            /**
             * @brief Getter for the value of the property.
             * Same getter for different types of values.
             * The type is deduced from the aDefaultValue parameter,
             * If the key combination doesn't exist or has a null value,
             * the given default value is returned.
             *
             * @param[in] aSectionKey
             * The section key.
             *
             * @param[in] aPropertyKey
             * The property key.
             *
             * @param[in] aDefaultValue
             * The default value used if key doesn't not exist or is a null object.
             *
             * @return
             * value stored under this key, or the default value.
             */

            float GetValue(const std::string &aSectionKey,
                           const std::string &aPropertyKey,
                           float aDefaultValue) override;

            uint GetValue(const std::string &aSectionKey,
                          const std::string &aPropertyKey,
                          int aDefaultValue) override;

            double GetValue(const std::string &aSectionKey,
                            const std::string &aPropertyKey,
                            double aDefaultValue) override;

            std::string GetValue(const std::string &aSectionKey,
                                 const std::string &aPropertyKey,
                                 std::string aDefaultValue) override;

            bool GetValue(const std::string &aSectionKey,
                          const std::string &aPropertyKey,
                          bool aDefaultValue) override;


            void WriteValue(const std::string &aSectionKey,
                            const std::string &aPropertyKey,
                            std::string aValue) override;


            /**
             * @brief Check if a section entry exists in the object.
             *
             * @param[in] aSectionKey
             * The section key to check.
             *
             * @return True if the section exists.
             */
            bool Contains(const std::string &aSectionKey) override;

            /**
             * @brief Check if a property entry exists in a specific section.
             *
             * @param[in] aSectionKey
             * The section key to check.
             *
             * @param[in] aPropertyKey
             * The property key to check
             *
             * @return True if the property in that section exists.
             */
            bool Contains(const std::string &aSectionKey,
                          const std::string &aPropertyKey) override;


            int Size();

            std::string GetValue(const std::string &aPropertyKey,
                                 const std::string &aDefaultValue);


        private:
            /// The json object used internally.
            nlohmann::json mJson;

        };
    } //namespace configurations
} //namespace operations

#endif //THOTH_CONFIGURATIONS_JSON_CONFIGURATION_MAP_HPP
