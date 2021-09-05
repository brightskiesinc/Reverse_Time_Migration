/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_BASE_EXCEPTIONS_NULL_POINTER_EXCEPTION_HPP
#define BS_BASE_EXCEPTIONS_NULL_POINTER_EXCEPTION_HPP

#include <bs/base/exceptions/interface/BaseException.hpp>

#define NULL_POINTER_EXCEPTION() \
NullPointerException(__FILE__, __LINE__, __FUNCTION__)

#define NULL_POINTER_EXCEPTION_WITH_MESSAGE(MESSAGE) \
NullPointerException(__FILE__, __LINE__, __FUNCTION__, MESSAGE)

namespace bs {
    namespace base {
        namespace exceptions {

            class NullPointerException : public BaseException {
            public:
                NullPointerException(const char *aFileName,
                                     int aLineNumber,
                                     const char *aFunctionName,
                                     const char *aAdditionalInformation = nullptr)
                        : BaseException("Null Pointer Exception: Pointer returns null.",
                                        aFileName, aLineNumber, aFunctionName, aAdditionalInformation) {}
            };

            /**
             * @brief Checks if the pointer is returned null
             * @return boolean[out]
             */
            bool inline is_null_ptr(void *pointer) {
                return pointer == nullptr;
            }
        } //namespace exceptions
    } //namespace base
} //namespace bs

#endif //BS_BASE_EXCEPTIONS_NULL_POINTER_EXCEPTION_HPP
