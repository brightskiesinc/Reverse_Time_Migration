//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_EXCEPTIONS_NOT_FOUND_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_NOT_FOUND_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct NotFoundException : public std::exception {
            const char *what() const noexcept override {
                return "Not Found Exception: Key provided is not found";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_NOT_FOUND_EXCEPTION_HPP
