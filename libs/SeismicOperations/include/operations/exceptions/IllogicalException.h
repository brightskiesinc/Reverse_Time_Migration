//
// Created by zeyad-osama on 24/09/2020.
//

#ifndef OPERATIONS_LIB_EXCEPTIONS_ILLOGICAL_EXCEPTION_H
#define OPERATIONS_LIB_EXCEPTIONS_ILLOGICAL_EXCEPTION_H

#include <exception>

namespace operations {
    namespace exceptions {
        struct IllogicalException : public std::exception {
            const char *what() const noexcept override {
                return "Illogical Exception: Illogical value provided";
            }
        };
    } //namespace exceptions
} //namespace operations

#endif //OPERATIONS_LIB_EXCEPTIONS_ILLOGICAL_EXCEPTION_H
