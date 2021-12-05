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

#ifndef BS_BASE_LOGGER_LOGGER_COLLECTION_HPP
#define BS_BASE_LOGGER_LOGGER_COLLECTION_HPP

#include <vector>
#include <sstream>

#include <bs/base/logger/interface/Logger.hpp>

namespace bs {
    namespace base {
        namespace logger {

            /**
             * @brief The different logger Modes.
             */
            class LoggerCollection {
            public:
                /**
                 * @brief Default constructor.
                 */
                LoggerCollection();

                /**
                 * @brief Default destructor.
                 */
                ~LoggerCollection();

                /**
                  * @brief Adding the different loggers to the vector
                  *
                  * @param[in] aLogger
                  * The vector of loggers
                  */
                void RegisterLogger(Logger *aLogger);

                /**
                  * @brief Handling the logger requirements
                  *
                  * @param[in] aLoggerName
                  * The string of loggerName
                  *
                  * @param[in] aMode
                  * The WriterMode to select from file, console or both
                  *
                  * @param[in] aPattern
                  * The patterns to select which format will print
                  */
                void ConfigureLoggers(const std::string &aLoggerName,
                                      WriterMode aMode = FILE_CONSOLE,
                                      Pattern aPattern = DATE_TIME);

                /**
                 * Implement the needed operation for logging
                 * @param aStatement :the statement to be written
                 * @param aOperation : operation type
                 */
                void Operate(const std::string &aStatement, Operation aOperation);

            private:
                /// Vector of pointers to the available loggers
                std::vector<Logger *> mpLoggersVector;
                /// The logger mode
                WriterMode mMode;
            };

        } //namespace logger
    } //namespace base
} //namespace bs

#endif //BS_BASE_LOGGER_LOGGER_COLLECTION_HPP
