//
// Created by zeyad-osama on 09/03/2021.
//

#ifndef THOTH_HELPERS_DISPLAYERS_H
#define THOTH_HELPERS_DISPLAYERS_H

namespace thoth {
    namespace helpers {
        namespace displayers {

            /**
             * @brief Prints the text header extracted from the given SEG-Y file in the
             * SEG-Y community format.
             *
             * @param apTextHeader
             */
            void
            print_text_header(unsigned char *apTextHeader);

        } //namespace displayers
    } //namespace helpers
} //namespace thoth

#endif //THOTH_HELPERS_DISPLAYERS_H
