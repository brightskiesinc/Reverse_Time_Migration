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

#include <bs/timer/reporter/TimerReporter.hpp>

#include <bs/timer/common/Definitions.hpp>

#include <bs/base/common/ExitCodes.hpp>

#include <ostream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>

#define mega  1024 * 1024
#define giga  1024 * 1024 * 1024

using namespace std;
using namespace bs::timer;
using namespace bs::timer::configurations;
using namespace bs::timer::dataunits;


reporter::TimerReporter::TimerReporter() {
    for (const auto &channel : TimerManager::GetInstance()->GetMap()) {
        this->mDataMap[channel.first] = channel.second->GetChannelStats();
    }
}

map<string, double>
reporter::TimerReporter::GetMap(const std::string &aChannelName) {
    return this->mDataMap[aChannelName].GetMap();
}

void
reporter::TimerReporter::Resolve() {
    for (auto &channel: this->mDataMap) {
        channel.second.Resolve();
    }
}

std::string
reporter::TimerReporter::GenerateStream(std::ostream &aOutputStream, const std::string &aChannelName) {
    string val;
    auto precision = TimerManager::GetInstance()->GetPrecision();
    string unit;
    if (precision == BS_TIMER_TU_MILLI) {
        unit = BS_TIMER_TU_STR_MILLI;
    } else if (precision == BS_TIMER_TU_MICRO) {
        unit = BS_TIMER_TU_STR_MICRO;
    } else if (precision == BS_TIMER_TU_NANO) {
        unit = BS_TIMER_TU_STR_NANO;
    } else {
        unit = BS_TIMER_TU_STR_SEC;
    }

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
        os << left << setfill(' ') << setw(20) << "Function name" << ": ";
        os << left << setfill(' ') << setw(20) << aChannelName << "\n";
        os << left << setfill(' ') << setw(20) << "Number of Calls" << ": ";
        os << left << setfill(' ') << setw(20)
           << std::fixed << this->mDataMap[aChannelName].GetNumberOfCalls() << "\n";
        os << left << setfill(' ') << setw(20) << "Maximum Runtime" << ": ";
        os << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_MAX_RUNTIME] / precision;
        os << left << unit << "\n";
        os << left << setfill(' ') << setw(20) << "Minimum Runtime" << ": ";
        os << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_MIN_RUNTIME] / precision << unit << "\n";
        os << left << setfill(' ') << setw(20) << "Average Runtime" << ": ";
        os << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_AVERAGE_RUNTIME] / precision << unit << "\n";
        os << left << setfill(' ') << setw(20) << "Total Runtime" << ": ";
        os << left << setfill(' ') << setw(12)
           << std::scientific << stats[BS_TIMER_K_TOTAL] / precision << unit << "\n";

        if (this->mDataMap[aChannelName].GetDataSize() > 0) {
            os << left << setfill(' ') << setw(20) << "Size of Data" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << (double) this->mDataMap[aChannelName].GetDataSize() / mega;
            os << " Mpts"
               << "\n";
            os << left << setfill(' ') << setw(20) << "Maximum Bandwidth" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_BANDWIDTH] / giga;
            os << " GBytes/s" << "\n";
            os << left << setfill(' ') << setw(20) << "Minimum Bandwidth" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_BANDWIDTH] / giga;
            os << " Gbytes/s" << "\n";
            os << left << setfill(' ') << setw(20) << "Average Bandwidth" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_BANDWIDTH] / giga;
            os << " GBytes/s" << "\n";
        }
        if (this->mDataMap[aChannelName].GetGridSize() > 0) {
            os << left << setfill(' ') << setw(20) << "Grid Size" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << (double) this->mDataMap[aChannelName].GetGridSize() / mega;
            os << " Mpts" << "\n";
            os << left << setfill(' ') << setw(20) << "Maximum Throughput" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_THROUGHPUT] / mega;
            os << " Mpts/s" << "\n";
            os << left << setfill(' ') << setw(20) << "Minimum Throughput" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_THROUGHPUT] / mega;
            os << " Mpts/s" << "\n";
            os << left << setfill(' ') << setw(20) << "Average Throughput" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_THROUGHPUT] / mega;
            os << " Mpts/s" << "\n";
            os << left << setfill(' ') << setw(20) << "Maximum GFLops" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MAX_GFLOPS] / giga;
            os << " GFLOPS" << "\n";
            os << left << setfill(' ') << setw(20) << "Minimum GFlops" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_MIN_GFLOPS] / giga;
            os << " GFLOPS" << "\n";
            os << left << setfill(' ') << setw(20) << "Average GFlops" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_AVERAGE_GFLOPS] / giga;
            os << " GFLOPS" << "\n";
            os << left << setfill(' ') << setw(20) << "Number of Operations" << ": ";
            os << left << setfill(' ') << setw(11)
               << std::scientific << stats[BS_TIMER_K_OPERATIONS] << "\n";
        }
        val = os.str();
    }
    return val;
}

int
reporter::TimerReporter::FlushReport(const std::string &aFilePath) {
    TimerReporter::HandleFilePath(aFilePath);
    std::ofstream os = std::ofstream(aFilePath, std::ios::out | std::ios::binary);
    if (!os) {
        std::cerr << "Error: Cannot open file!" << std::endl;
        return BS_BASE_RC_FAILURE;
    }
    auto channels_count = TimerManager::GetInstance()->GetMap().size();
    os.write(reinterpret_cast<char *>(&channels_count),
             sizeof(int)); /* Number of kernels reported. (4 bytes)*/
    unsigned int size;
    vector<double> runtimes;
    vector<double> bandwidths;
    for (auto channel : this->mDataMap) {
        auto stats = channel.second.GetMap();
        size = channel.first.size();
        os.write(reinterpret_cast<char *>(&size), sizeof(unsigned)); /* Size of kernel name. (4 bytes) */
        os.write(channel.first.c_str(), size * sizeof(char)); /* Kernel name (variable)*/
        runtimes = channel.second.GetRuntimes();
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
            bandwidths = channel.second.GetBandwidths();
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
reporter::TimerReporter::HandleFilePath(const std::string &aFilePath) {
    auto directory = aFilePath;
    auto pos = directory.rfind('/');
    if (pos != string::npos) {
        directory.erase(directory.begin() + pos, directory.end());
    }
    return mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

map<std::string, ChannelStats>
reporter::TimerReporter::GetStats() {
    return this->mDataMap;
}
