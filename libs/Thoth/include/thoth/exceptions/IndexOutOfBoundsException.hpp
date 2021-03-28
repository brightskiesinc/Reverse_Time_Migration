//
// Created by zeyad-osama on 11/03/2021.
//

#ifndef THOTH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
#define THOTH_INDEXOUTOFBOUNDSEXCEPTION_HPP

#endif //THOTH_INDEXOUTOFBOUNDSEXCEPTION_HPP

//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
#define THOTH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct IndexOutOfBoundsException : public std::exception {
            const char *what() const noexcept override {
                return "Index Out Of Bounds Exception: Provided index is out of your memory access.";
            }
        };
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
