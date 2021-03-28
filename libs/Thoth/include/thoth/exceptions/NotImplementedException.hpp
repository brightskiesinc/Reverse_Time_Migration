//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_EXCEPTIONS_NOT_IMPLEMENTED_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_NOT_IMPLEMENTED_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct NotImplementedException : public std::exception {
            const char *what() const noexcept override {
                return "Not Implemented Exception: Function not yet implemented.";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_NOT_IMPLEMENTED_EXCEPTION_HPP
