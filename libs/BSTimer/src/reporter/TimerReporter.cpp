/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Timer.
 *
 * BS Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Timer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ostream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>

#include <bs/base/common/ExitCodes.hpp>

#include <bs/timer/reporter/TimerReporter.hpp>
#include <bs/timer/common/Definitions.hpp>

#define BS_TIMER_DU_MEGA    (1024 * 1024)               /* Mega definition. */
#define BS_TIMER_DU_GIGA    (1024 * 1024 * 1024)        /* Giga definition. */

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::dataunits;
using namespace bs::timer::reporter;


TimerReporter::TimerReporter() {
    for (const auto &channel : TimerManager::GetInstance()->GetMap()) {
        this->mDataMap[channel.first] = channel.second->GetChannelStats();
    }
}

map<string, double>
TimerReporter::GetMap(const std::string &aChannelName) {
    return this->mDataMap[aChannelName].GetMap();
}

void
TimerReporter::Resolve() {
    for (auto &channel: this->mDataMap) {
        channel.second.Resolve();
    }
}

std::string
TimerReporter::GenerateStream(std::ostream &aOutputStream, const std::string &aChannelName) {
    string val;
    auto precision = TimerManager::GetInstance()->GetPrecision();
    string unit = TimerReporter::PrecisionToUnit(precision);

    if (aChannelName == " ") {
        string report;
        for (const auto &channel: this->mDataMap) {
            report += GenerateStream(aOutputStream, channel.first);
        }
        val = report;
        aOutputStream << val;
    } else {
        this->mDataMap[aChannelName].Resolve();
        std::stringstream os;
        auto stats = this->mDataMap[aChannelName].GetMap();
        os.precision(5);

        os << std::endl;

        os << left << setfill(' ') << setw(20) << "Function Name" << ": "
           << left << setfill(' ') << setw(20) << aChannelName << std::endl;

        os << left << setfill(' ') << setw(20) << "Number of Calls" << ": "
           << left << setfill(' ') << setw(20)
           << std::fixed << this->mDataMap[aChannelName].GetNumberOfCalls() << std::endl;

        os << left << setfill(' ') << setw(20) << "Maximum Runtime" << ": "
           << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_MAX_RUNTIME] / precision
           << left << unit << "\n";

        os << left << setfill(' ') << setw(20) << "Minimum Runtime" << ": "
           << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_MIN_RUNTIME] / precision << unit << std::endl;

        os << left << setfill(' ') << setw(20) << "Average Runtime" << ": "
           << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_AVERAGE_RUNTIME] / precision << unit << std::endl;

        os << left << setfill(' ') << setw(20) << "Total Runtime" << ": "
           << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_TOTAL] / precision << unit << std::endl;

        if (this->mDataMap[aChannelName].GetDataSize() > 0) {
            os << left << setfill(' ') << setw(20) << "Size of Data" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << (double) this->mDataMap[aChannelName].GetDataSize() / BS_TIMER_DU_MEGA;

            os << " Mpts" << std::endl
               << left << setfill(' ') << setw(20) << "Maximum Bandwidth" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_BANDWIDTH] / BS_TIMER_DU_GIGA;

            os << " GBytes/s" << std::endl
               << left << setfill(' ') << setw(20) << "Minimum Bandwidth" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_BANDWIDTH] / BS_TIMER_DU_GIGA;

            os << " GBytes/s" << std::endl
               << left << setfill(' ') << setw(20) << "Average Bandwidth" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_BANDWIDTH] / BS_TIMER_DU_GIGA
               << " GBytes/s" << std::endl;
        }
        if (this->mDataMap[aChannelName].GetGridSize() > 0) {
            os << left << setfill(' ') << setw(20) << "Grid Size" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << (double) this->mDataMap[aChannelName].GetGridSize() / BS_TIMER_DU_MEGA
               << " Mpts" << std::endl;

            os << left << setfill(' ') << setw(20) << "Maximum Throughput" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_THROUGHPUT] / BS_TIMER_DU_MEGA
               << " Mpts/s" << std::endl;

            os << left << setfill(' ') << setw(20) << "Minimum Throughput" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_THROUGHPUT] / BS_TIMER_DU_MEGA
               << " Mpts/s" << std::endl;

            os << left << setfill(' ') << setw(20) << "Average Throughput" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_THROUGHPUT] / BS_TIMER_DU_MEGA
               << " Mpts/s" << std::endl;

            os << left << setfill(' ') << setw(20) << "Maximum GFLops" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_GFLOPS]
               << " GFLOPS" << std::endl;

            os << left << setfill(' ') << setw(20) << "Minimum GFlops" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_GFLOPS]
               << " GFLOPS" << std::endl;

            os << left << setfill(' ') << setw(20) << "Average GFlops" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_GFLOPS]
               << " GFLOPS" << std::endl;

            os << left << setfill(' ') << setw(20) << "Number of Operations" << ": "
               << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_OPERATIONS] << std::endl;
        }
        val = os.str();
    }
    return val;
}

int
TimerReporter::FlushReport(const std::string &aFilePath) {
    TimerReporter::HandleFilePath(aFilePath);
    std::ofstream os = std::ofstream(aFilePath, std::ios::out | std::ios::binary);
    if (!os) {
        std::cerr << "Error: Cannot open file!" << std::endl;
        return BS_BASE_RC_FAILURE;
    }
    auto channels_count = TimerManager::GetInstance()->GetMap().size();
    os.write(reinterpret_cast<char *>(&channels_count),
             sizeof(int)); /* Number of kernels reported. (4 bytes)*/
    for (auto channel : this->mDataMap) {
        /* Resolve channel first to be able to fetch data correctly. */
        channel.second.Resolve();

        auto stats = channel.second.GetMap();
        auto size = channel.first.size();
        os.write(reinterpret_cast<char *>(&size), sizeof(unsigned)); /* Size of kernel name. (4 bytes) */
        os.write(channel.first.c_str(), size * sizeof(char)); /* Kernel name (variable)*/
        auto runtimes = channel.second.GetRuntimes();
        size = runtimes.size();
        os.write(reinterpret_cast<char *>(&size),
                 sizeof(unsigned)); /* Number of calls for this kernel/ size of arrays(4 byte)*/
        os.write(reinterpret_cast<char *>(&runtimes[0]),
                 size * sizeof(double)); /* Array of runtimes (variable)*/
        os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_MAX_RUNTIME]),
                 sizeof(double));
        os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_MIN_RUNTIME]),
                 sizeof(double));
        os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_AVERAGE_RUNTIME]),
                 sizeof(double));
        unsigned char flag;
        if (channel.second.GetDataSize() > 0) {
            flag = 1;
            os.write(reinterpret_cast<char *>(&flag),
                     sizeof(unsigned char));
            auto bandwidths = channel.second.GetBandwidths();
            os.write(reinterpret_cast<char *>(&bandwidths[0]),
                     size * sizeof(double)); /*Array of bandwidths (variable) */
            os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_MAX_BANDWIDTH]),
                     sizeof(double));
            os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_MIN_BANDWIDTH]),
                     sizeof(double));
            os.write(reinterpret_cast<char *>(&stats[BS_TIMER_K_AVERAGE_BANDWIDTH]),
                     sizeof(double));
        } else {
            flag = 0;
            os.write(reinterpret_cast<char *>(&flag),
                     sizeof(unsigned char));

        }
    }
    os.close();
    return BS_BASE_RC_SUCCESS;
}

int
TimerReporter::HandleFilePath(const std::string &aFilePath) {
    auto directory = aFilePath;
    auto pos = directory.rfind('/');
    if (pos != string::npos) {
        directory.erase(directory.begin() + pos, directory.end());
    }
    return mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

map<std::string, ChannelStats>
TimerReporter::GetStats() {
    return this->mDataMap;
}

std::string
TimerReporter::PrecisionToUnit(double aPrecision) {
    string unit;
    if (aPrecision == BS_TIMER_TU_MILLI) {
        unit = BS_TIMER_TU_STR_MILLI;
    } else if (aPrecision == BS_TIMER_TU_MICRO) {
        unit = BS_TIMER_TU_STR_MICRO;
    } else if (aPrecision == BS_TIMER_TU_NANO) {
        unit = BS_TIMER_TU_STR_NANO;
    } else {
        unit = BS_TIMER_TU_STR_SEC;
    }
    return unit;
}
