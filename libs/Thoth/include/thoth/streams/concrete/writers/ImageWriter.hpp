//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_IMAGE_WRITER_HPP
#define THOTH_STREAMS_IMAGE_WRITER_HPP

#include <thoth/streams/primitive/Writer.hpp>

namespace thoth {
    namespace streams {
        /**
         * @brief
         */
        class ImageWriter : public Writer {
        public:
            /**
             * @brief Destructor
             */
            ~ImageWriter() override = default;
        };

    } //streams
} //thoth

#endif //THOTH_STREAMS_IMAGE_WRITER_HPP
