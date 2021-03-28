//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_STREAM_HPP
#define THOTH_STREAMS_STREAM_HPP

#include <thoth/configurations/interface/Configurable.hpp>

#include <string>

namespace thoth {
    namespace streams {
        /**
         * @brief
         */
        class Stream : public thoth::configuration::Configurable {
        public:
            /**
             * @brief Destructor
             */
            virtual ~Stream() = default;

            /**
             * @brief Returns the file format extension of the current stream.
             */
            virtual std::string GetExtension() = 0;

        protected:
            /// Configurations Map
            thoth::configuration::ConfigurationMap *mpConfigurationMap;
        };

    } //streams
} //thoth

#endif //THOTH_STREAMS_STREAM_HPP
