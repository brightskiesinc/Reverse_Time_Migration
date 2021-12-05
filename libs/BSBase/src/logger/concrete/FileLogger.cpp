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
#include <fstream>

#ifdef USING_SPDLOG

#include <spdlog/sinks/file_sinks.h>
#include <spdlog/logger.h>

#else

#include <fstream>
#include <sys/stat.h>

#endif

#include <bs/base/logger/concrete/FileLogger.hpp>


using namespace bs::base::logger;


FileLogger::FileLogger(const std::string &aFilePath) {
    FileLogger::HandleFilePath(aFilePath);
    this->mFilePath = aFilePath;
    mFileStream.open(mFilePath);
}

FileLogger::~FileLogger() = default;

void
FileLogger::Configure(std::string aLoggerName) {
#ifdef USING_SPDLOG
    std::shared_ptr<spdlog::sinks::simple_file_sink_mt> mSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>(
            mFilePath);
    mpLogger = std::make_shared<spdlog::logger>(aLoggerName, mSink);
#endif
}


WriterMode
FileLogger::GetType() {
    return WriterMode::FILE_MODE;
}


int
FileLogger::HandleFilePath(const std::string &aFilePath) {
    auto directory = aFilePath;
    auto pos = directory.rfind('/');
    if (pos != std::string::npos) {
        directory.erase(directory.begin() + pos, directory.end());
    }
    return mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void
FileLogger::Operate(std::string aStatement, Operation aOperation) {
    this->mStatement = aStatement;
    switch (aOperation) {
        case Operation::INFO:
#ifdef USING_SPDLOG
            mpLogger->info(mStatement);
#else
            mFileStream.open(mFilePath, std::ios::app);
            mFileStream << "[Info] " << mStatement << std::endl;
            mFileStream.close();
#endif
            break;
        case Operation::ERROR:
#ifdef USING_SPDLOG
            mpLogger->error(mStatement);
#else
            mFileStream.open(mFilePath, std::ios::app);
            mFileStream << "[Error] " << mStatement << std::endl;
            mFileStream.close();
#endif
            break;
        case Operation::CRITICAL:
#ifdef USING_SPDLOG
            mpLogger->critical(mStatement);
#else
            mFileStream.open(mFilePath, std::ios::app);
            mFileStream << "[Critical] " << mStatement << std::endl;
            mFileStream.close();
#endif
        case Operation::DEBUG:
#ifdef USING_SPDLOG
            mpLogger->debug(mStatement);
#else
            mFileStream.open(mFilePath, std::ios::app);
            mFileStream << "[Debug] " << mStatement << std::endl;
            mFileStream.close();
#endif
            break;
        default:
            std::cout << "Undefined feature";
            exit(0);
    }
}
