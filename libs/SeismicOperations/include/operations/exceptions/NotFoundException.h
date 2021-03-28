//
// Created by zeyad-osama on 24/09/2020.
//

#ifndef OPERATIONS_LIB_EXCEPTIONS_NOT_FOUND_EXCEPTION_H
#define OPERATIONS_LIB_EXCEPTIONS_NOT_FOUND_EXCEPTION_H

#include <exception>

namespace operations {
    namespace exceptions {
        struct NotFoundException : public std::exception {
            const char *what() const noexcept override {
                return "Not Found Exception: Key provided is not found";
            }
        };
    } //namespace exceptions
} //namespace operations

#endif //OPERATIONS_LIB_EXCEPTIONS_NOT_FOUND_EXCEPTION_H
