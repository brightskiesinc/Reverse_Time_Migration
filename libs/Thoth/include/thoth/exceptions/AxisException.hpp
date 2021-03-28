//
// Created by zeyad-osama on 25/01/2021.
//

#ifndef THOTH_EXCEPTIONS_AXIS_EXCEPTION_HPP
#define THOTH_EXCEPTIONS_AXIS_EXCEPTION_HPP

#include <exception>

namespace thoth {
    namespace exceptions {
        struct AxisException : public std::exception {
            const char *what() const noexcept override {
                return "Axis Exception: Axis is not in range of [0-2] i.e. x,y,z";
            }
        };

/**
 * @brief Checks if the provided axe is within the x,y,z range.
 * @param axis[in]
 * @return boolean[out]
 */
        bool inline is_out_of_range(uint axis) {
            return axis < 0 || axis > 2;
        }
    } //namespace exceptions
} //namespace thoth

#endif //THOTH_EXCEPTIONS_AXIS_EXCEPTION_HPP
