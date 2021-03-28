//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_BINARY_READER_HPP
#define THOTH_STREAMS_BINARY_READER_HPP

#include <thoth/streams/primitive/Reader.hpp>

namespace thoth {
    namespace streams {
        /**
         * @brief
         */
        class BinaryReader : public Reader {
        public:

            /**
             * @brief Destructor
             */
            ~BinaryReader() override;
        };

    } //streams
} //thoth

#endif //THOTH_STREAMS_BINARY_READER_HPP
