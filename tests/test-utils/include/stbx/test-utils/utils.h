//
// Created by zeyad-osama on 24/01/2021.
//

#ifndef SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H
#define SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H

#include <string>

namespace stbx {
    namespace testutils {
        template<typename Base, typename T>
        inline bool instanceof(const T *) {
            return std::is_base_of<Base, T>::value;
        }
    } //namespace testutils
} //namespace stbx

#endif //SEISMIC_TOOLBOX_TESTS_TEST_UTILS_UTILS_H
