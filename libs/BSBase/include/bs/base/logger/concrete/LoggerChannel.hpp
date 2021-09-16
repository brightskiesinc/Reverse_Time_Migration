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

#ifndef BS_BASE_LOGGER_LOGGER_CHANNEL_H
#define BS_BASE_LOGGER_LOGGER_CHANNEL_H

#include <bs/base/logger/interface/Logger.hpp>
#include <bs/base/logger/concrete/LoggerCollection.hpp>

#include <sstream>

namespace bs {
    namespace base {
        namespace logger {

            class LoggerChannel {
            public:
                /**
                 * Constructor
                 * @param mCollection : pointer to the logger collection
                 * @param mOperation : Operation type
                 */
                LoggerChannel(LoggerCollection *aCollection, Operation aOperation);

                /**
                 * @brief Default destructor.
                 */
                ~LoggerChannel();

                /**
                 * @brief overload << operator to accept any number and type of elements
                 *
                 * @param[in] aValue
                 * The template variable to hold the data
                 *
                 * @return
                 * Reference to the LoggerChannel class
                 */
                template<typename T>
                LoggerChannel &operator<<(const T &aValue) {
                    mStringStream << aValue;
                    std::string message = mStringStream.str();
                    const char *cstr = message.c_str();
                    if (cstr[message.length() - 1] == '\n') {
                        message.erase(message.size() - 1);
                        this->mpCollection->Operate(message, this->mOperationType);
                        mStringStream.str("");
                    }
                    return *this;
                }

            private:
                /// The string stream that holds the input data
                std::stringstream mStringStream;
                ///The Operation type
                Operation mOperationType;
                ///Pointer to the LoggerCollection Interface
                LoggerCollection *mpCollection;
            };

        } //namespace logger
    } //namespace helpers
} //namespace operations

#endif //BS_BASE_LOGGER_LOGGER_CHANNEL_H