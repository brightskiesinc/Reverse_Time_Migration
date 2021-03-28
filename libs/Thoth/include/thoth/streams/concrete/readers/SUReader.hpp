//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_SU_READER_HPP
#define THOTH_STREAMS_SU_READER_HPP

#include <thoth/streams/primitive/Reader.hpp>
#include <thoth/helpers/stream_helpers.h>

namespace thoth {
    namespace streams {
        /**
         * @brief
         */
        class SUReader : public Reader {
        public:
            /**
             * @brief Destructor
             */
            ~SUReader() override;
        };

    } //streams
} //thoth

#endif //THOTH_STREAMS_SU_READER_HPP
