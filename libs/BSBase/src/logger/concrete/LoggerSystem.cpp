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

#include <bs/base/logger/concrete/LoggerSystem.hpp>

using namespace bs::base::logger;


LoggerSystem::LoggerSystem() {
    this->mpLoggerChannels.push_back(new LoggerChannel(&this->mpCollection, INFO));
    this->mpLoggerChannels.push_back(new LoggerChannel(&this->mpCollection, ERROR));
    this->mpLoggerChannels.push_back(new LoggerChannel(&this->mpCollection, CRITICAL));
    this->mpLoggerChannels.push_back(new LoggerChannel(&this->mpCollection, DEBUG));
}

void
LoggerSystem::RegisterLogger(Logger *aLogger) {
    this->mpCollection.RegisterLogger(aLogger);
}

void
LoggerSystem::ConfigureLoggers(const std::string &aLoggerName, WriterMode aMode, Pattern aPattern) {
    this->mpCollection.ConfigureLoggers(aLoggerName, aMode, aPattern);
}

LoggerChannel &
LoggerSystem::Info() {
    return *this->mpLoggerChannels[0];
}

LoggerChannel &
LoggerSystem::Error() {
    return *this->mpLoggerChannels[1];
}

LoggerChannel &
LoggerSystem::Critical() {
    return *this->mpLoggerChannels[2];
}

LoggerChannel &
LoggerSystem::Debug() {
    return *this->mpLoggerChannels[3];
}
