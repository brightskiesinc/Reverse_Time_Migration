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

#ifndef BS_BASE_LOGGER_GENERAL_LOGGER_HPP
#define BS_BASE_LOGGER_GENERAL_LOGGER_HPP

#ifdef USING_SPDLOG

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/logger.h>

#endif

#include <bs/base/logger/interface/Logger.hpp>

namespace bs {
    namespace base {
        namespace logger {

            class GeneralLogger : public Logger {
            public:
                void SetPattern(Pattern aPattern) override {
#ifdef USING_SPDLOG
                    if (aPattern == Pattern::DATE_TIME_THREAD) {
                        this->mpLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%n][thread %t][%l] %v");
                    } else if (aPattern == Pattern::DATE_TIME) {
                        this->mpLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%n][%l] %v");
                    } else if (aPattern == Pattern::TIME) {
                        this->mpLogger->set_pattern("[%H:%M:%S.%e][%n] %v");
                    } else if (aPattern == Pattern::THREAD) {
                        this->mpLogger->set_pattern("[thread %t][%n] %v");
                    }
#endif
                }

            protected:
#ifdef USING_SPDLOG
                /// The logger pointer.
                std::shared_ptr<spdlog::logger> mpLogger;
#endif
            };

        } //namespace logger
    } //namespace base
} //namespace bs

#endif //BS_BASE_LOGGER_GENERAL_LOGGER_HPP
