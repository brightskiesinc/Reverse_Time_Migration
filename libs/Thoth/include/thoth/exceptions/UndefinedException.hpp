//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_EXCEPTIONS_NOT_UNDEFINED_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_NOT_UNDEFINED_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct UndefinedException : public std::exception {
            const char *what() const noexcept override {
                return "Undefined Exception: Undefined feature.";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_NOT_UNDEFINED_EXCEPTION_HPP
