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

#ifndef BS_BASE_LOGGER_LOGGER_SYSTEM_HPP
#define BS_BASE_LOGGER_LOGGER_SYSTEM_HPP

#include <bs/base/logger/concrete/LoggerCollection.hpp>
#include <bs/base/logger/concrete/LoggerChannel.hpp>
#include <bs/base/logger/interface/Logger.hpp>
#include <bs/base/common/Singleton.tpp>

namespace bs {
    namespace base {
        namespace logger {

            class LoggerSystem : public common::Singleton<LoggerSystem> {
            public:

                friend class Singleton<LoggerSystem>;

                /**
                 * @brief constructor.
                 */
                LoggerSystem();


                /**
                 * @brief Default destructor.
                 */
                ~LoggerSystem() = default;


                /**
                 * Wrapper for the RegisterLogger function of the Logger collection
                 * @param aLogger :Logger to be registered
                 */
                void RegisterLogger(Logger *aLogger);

                /**
                  * @brief Wrapper for the ConfigureLoggers function of the Logger collection
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
                 * Choose the Info logger channel.
                 * @return
                 */
                LoggerChannel &Info();


                /**
                 * Choose the Error logger channel.
                 * @return
                 */
                LoggerChannel &Error();


                /**
                 * Choose the Critical logger channel.
                 * @return
                 */
                LoggerChannel &Critical();

                /**
                 * Choose the Debug logger channel.
                 * @return
                 */
                LoggerChannel &Debug();

            private:
                ///Instance of LoggerCollection Interface
                LoggerCollection mpCollection;
                ///Vector of pointers to the different logger channels
                std::vector<LoggerChannel *> mpLoggerChannels;

            };

        } //namespace logger
    } //namespace base
} //namespace bs

#endif //BS_BASE_LOGGER_LOGGER_SYSTEM_HPP
