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


#ifndef BS_BASE_LOGGER_LOGGER_HPP
#define BS_BASE_LOGGER_LOGGER_HPP

#include <string>

#ifdef USING_SPDLOG

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/logger.h>

#endif

namespace bs {
    namespace base {
        namespace logger {

            /**
             * @brief The different logger Modes.
             */
            enum WriterMode {
                FILE_MODE, CONSOLE_MODE, FILE_CONSOLE
            };

            /**
             * @brief The different logger format
             */
            enum Pattern {
                TIME,
                DATE_TIME,
                THREAD,
                DATE_TIME_THREAD
            };

            /**
             * @brief The different operations type
             */
            enum Operation {
                INFO,
                ERROR,
                CRITICAL,
                DEBUG
            };

            /**
             * @brief Callback interface to be implemented by loggers
             */
            class Logger {
            public:
                /**
                 * @brief Default constructor.
                 */
                Logger() = default;

                /**
                 * @brief Default destructor.
                 */
                virtual ~Logger() = default;

                /**
                 * @brief
                 * Configure the type of logger -console or file-.
                 *
                 * @param[in] aLoggerName
                 *
                 */
                virtual void Configure(std::string aLoggerName) = 0;

                /**
                 * @brief
                 * Select the format of the message
                 *
                 * @param[in] aPattern
                 * The pattern to be display
                 */
                virtual void SetPattern(Pattern aPattern) = 0;

                /**
                 * @brief
                 * Detect the type of current logger
                 *
                 * @return
                 * The WRITER_MODE
                 */
                virtual WriterMode GetType() = 0;

                /**
                 * Implement the needed operation for logging
                 * @param aStatement :the statement to be written
                 * @param aOperation : operation type
                 */
                virtual void Operate(std::string aStatement, Operation aOperation) = 0;
            };

        } //namespace logger
    } //namespace base
} //namespace bs

#endif //BS_BASE_LOGGER_LOGGER_HPP
