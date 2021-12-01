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

#include <iostream>

#ifdef USING_SPDLOG

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/logger.h>

#else

#include <iomanip>

#endif

#include <bs/base/logger/concrete/ConsoleLogger.hpp>

using namespace std;
using namespace bs::base::logger;


ConsoleLogger::ConsoleLogger() = default;

ConsoleLogger::~ConsoleLogger() = default;

void
ConsoleLogger::Configure(std::string aLoggerName) {
#ifdef USING_SPDLOG
    std::shared_ptr<spdlog::sinks::stdout_sink_mt> mSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    this->mpLogger = std::make_shared<spdlog::logger>(aLoggerName, mSink);
#endif
}

WriterMode
ConsoleLogger::GetType() {
    return WriterMode::CONSOLE_MODE;
}

void
ConsoleLogger::Operate(std::string aStatement, Operation aOperation) {
    switch (aOperation) {
        case Operation::INFO:
#ifdef USING_SPDLOG
            this->mpLogger->info(aStatement);
#else
            std::cout << left << setfill(' ') << setw(20) << "[info] " << aStatement << std::endl;
#endif
            break;
        case Operation::ERROR:
#ifdef USING_SPDLOG
            this->mpLogger->error(aStatement);
#else
            std::cout << left << setfill(' ') << setw(20) << "[Error] " << aStatement << std::endl;
#endif
            break;
        case Operation::CRITICAL:
#ifdef USING_SPDLOG
            this->mpLogger->critical(aStatement);
#else
            std::cout << left << setfill(' ') << setw(20) << "[Critical] " << aStatement << std::endl;
#endif
            break;
        case Operation::DEBUG:
#ifdef USING_SPDLOG
            this->mpLogger->debug(aStatement);
#else
            std::cout << left << setfill(' ') << setw(20) << "[Debug] " << aStatement << std::endl;
#endif
            break;
        default:
            cout << "Undefined feature";
            exit(0);
    }
}
