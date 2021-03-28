//
// Created by zeyad-osama on 08/03/2021.
//

#ifndef THOTH_UTILS_CHECKERS_CHECKER_HPP
#define THOTH_UTILS_CHECKERS_CHECKER_HPP

namespace thoth {
    namespace utils {
        namespace checkers {

            class Checker {
            public:
                /**
                 * @brief Return 0 for big endian, 1 for little endian.
                 */
                static bool
                IsLittleEndianMachine();

            private:
                Checker() = default;

            };
        } //namespace checkers
    } //namespace utils
} //namespace thoth

#endif //THOTH_UTILS_CHECKERS_CHECKER_HPP
