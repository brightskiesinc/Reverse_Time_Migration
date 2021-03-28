//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_EXCEPTIONS_ILLOGICAL_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_ILLOGICAL_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct IllogicalException : public std::exception {
            const char *what() const noexcept override {
                return "Illogical Exception: Illogical value provided";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_ILLOGICAL_EXCEPTION_HPP
