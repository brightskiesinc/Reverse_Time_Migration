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

#ifndef BS_BASE_LOGGER_FILE_LOGGER_HPP
#define BS_BASE_LOGGER_FILE_LOGGER_HPP

#include <fstream>

#include <bs/base/logger/interface/Logger.hpp>
#include <bs/base/logger/concrete/GeneralLogger.hpp>

namespace bs {
    namespace base {
        namespace logger {

            class FileLogger : public GeneralLogger {
            public:
                explicit FileLogger(const std::string &aFilePath);

                ~FileLogger() override;

                void
                Configure(std::string aLoggerName) override;

                WriterMode
                GetType() override;

                void
                Operate(std::string aStatement, Operation aOperation) override;

            private:
                static int
                HandleFilePath(const std::string &aFilePath);

            private:
                /// The filepath
                std::string mFilePath;
                /// The statement
                std::string mStatement;
                /// The file stream
                std::ofstream mFileStream;
            };

        } //namespace logger
    } //namespace base
} //namespace bs

#endif //BS_BASE_LOGGER_FILE_LOGGER_HPP
