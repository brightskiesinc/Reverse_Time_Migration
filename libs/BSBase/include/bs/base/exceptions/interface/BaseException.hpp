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

#ifndef BS_BASE_EXCEPTIONS_BASE_EXCEPTION_HPP
#define BS_BASE_EXCEPTIONS_BASE_EXCEPTION_HPP

#include <iomanip>
#include <sstream>
#include <cstring>
#include <exception>

#define BASE_EXCEPTION(MESSAGE) \
BaseException(MESSAGE, __FILE__, __LINE__, __FUNCTION__)

namespace bs {
    namespace base {
        namespace exceptions {

            class BaseException : public std::exception {
            public:
                BaseException(const char *aMessage,
                              const char *aFileName,
                              int aLineNumber,
                              const char *aFunctionName,
                              const char *aAdditionalInformation = nullptr) {
                    std::stringstream ss;

                    ss << aMessage << std::endl;

                    ss << std::left << std::setfill(' ') << std::setw(10) << "File" << ": ";
                    ss << std::left << std::setfill(' ') << std::setw(10) << aFileName << std::endl;

                    ss << std::left << std::setfill(' ') << std::setw(10) << "Line" << ": ";
                    ss << std::left << std::setfill(' ') << std::setw(10) << aLineNumber << std::endl;

                    ss << std::left << std::setfill(' ') << std::setw(10) << "Function" << ": ";
                    ss << std::left << std::setfill(' ') << std::setw(10) << aFunctionName << std::endl;

                    if (aAdditionalInformation != nullptr) {
                        ss << std::left << std::setfill(' ') << std::setw(10) << "Info" << ": ";
                        ss << std::left << std::setfill(' ') << std::setw(10) << aAdditionalInformation << std::endl;
                    }

                    this->mExceptionMessage = ss.str();
                }

                const char *
                GetExceptionMessage() const noexcept {
                    return this->mExceptionMessage.c_str();
                }

                const char *
                what() const noexcept override {
                    return this->GetExceptionMessage();
                }

            private:
                std::string mExceptionMessage;
            };

        } //namespace exceptions
    } //namespace base
} //namespace bs

#endif //BS_BASE_EXCEPTIONS_BASE_EXCEPTION_HPP
