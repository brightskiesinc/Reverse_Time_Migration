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

#include <bs/base/logger/concrete/LoggerCollection.hpp>

#include <iostream>

using namespace bs::base::logger;


LoggerCollection::LoggerCollection() = default;

LoggerCollection::~LoggerCollection() {
    for (auto l : this->mpLoggersVector) { delete l; }
    this->mpLoggersVector.clear();
}

void
LoggerCollection::RegisterLogger(Logger *aLogger) {
    this->mpLoggersVector.push_back(aLogger);
}

void
LoggerCollection::ConfigureLoggers(const std::string &aLoggerName, WriterMode aMode, Pattern aPattern) {
    this->mMode = aMode;

    for (auto l : this->mpLoggersVector) {
        if (l->GetType() == this->mMode || this->mMode == WriterMode::FILE_CONSOLE) {
            l->Configure(aLoggerName);
            l->SetPattern(aPattern);
        }
    }
}

void
LoggerCollection::Operate(const std::string &aStatement, Operation aOperation) {
    for (auto l : this->mpLoggersVector) {
        if (l->GetType() == this->mMode || this->mMode == WriterMode::FILE_CONSOLE) {
            l->Operate(aStatement, aOperation);

        }
    }
}