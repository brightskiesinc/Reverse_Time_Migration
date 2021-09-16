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

#ifndef BS_BASE_EXCEPTIONS_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
#define BS_BASE_EXCEPTIONS_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP

#include <bs/base/exceptions/interface/BaseException.hpp>

#define INDEX_OUT_OF_BOUNDS_EXCEPTION() \
IndexOutOfBoundsException(__FILE__, __LINE__, __FUNCTION__)

#define INDEX_OUT_OF_BOUNDS_EXCEPTION_WITH_MESSAGE(MESSAGE) \
IndexOutOfBoundsException(__FILE__, __LINE__, __FUNCTION__, MESSAGE)

namespace bs {
    namespace base {
        namespace exceptions {

            class IndexOutOfBoundsException : public BaseException {
            public:
                IndexOutOfBoundsException(const char *aFileName,
                                          int aLineNumber,
                                          const char *aFunctionName,
                                          const char *aAdditionalInformation = nullptr)
                        : BaseException("Index Out Of Bounds Exception: Provided index is out of your memory access.",
                                        aFileName, aLineNumber, aFunctionName, aAdditionalInformation) {}
            };

        } //namespace exceptions
    } //namespace base
} //namespace bs

#endif //BS_BASE_EXCEPTIONS_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
