//
// Created by zeyad-osama on 10/03/2021.
//

#ifndef THOTH_EXCEPTIONS_UNSUPPORTED_FEATURE_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_UNSUPPORTED_FEATURE_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct UnsupportedFeatureException : public std::exception {
            const char *what() const noexcept override {
                return "Unsupported Feature Exception: Unsupported feature.";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_UNSUPPORTED_FEATURE_EXCEPTION_HPP

