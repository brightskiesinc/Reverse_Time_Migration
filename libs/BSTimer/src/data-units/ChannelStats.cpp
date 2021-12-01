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

#include <iostream>

#include <bs/timer/data-units/ChannelStats.hpp>

#define BS_TIMER_DU_GIGA    (1024 * 1024 * 1024)        /* Giga definition. */

using namespace std;
using namespace bs::timer::dataunits;


ChannelStats::ChannelStats() :
        mGridSize(-1),
        mDataSize(-1),
        mFLOPS(-1),
        mNumberOfCalls(0) {}

ChannelStats::~ChannelStats() {
    this->mSnapshots.clear();
}

void
ChannelStats::AddRuntime(double aRuntime) {
    this->mRuntimes.push_back(aRuntime);
    if (this->mDataSize > 0) {
        this->mBandwidths.push_back(this->mDataSize / aRuntime);
    }
    this->mNumberOfCalls++;
}

void
ChannelStats::AddSnapshot(core::snapshots::Snapshot *apSnapshot) {
    this->mSnapshots.push_back(apSnapshot);
}

void
ChannelStats::Resolve() {
    if (!mResolved) {
        mResolved = true;
        for (auto it : this->mSnapshots) {
            this->AddRuntime(it->Resolve());
        }
    }
}

void
ChannelStats::SetGridSize(int aGridSize) {
    this->mGridSize = aGridSize;
}

void
ChannelStats::SetDataSize(int aDataSize) {
    this->mDataSize = aDataSize;
}

void
ChannelStats::SetFLOPS(int aFLOPS) {
    this->mFLOPS = aFLOPS;
}

map<string, double>
ChannelStats::GetMap() {
    mStatisticsMap[BS_TIMER_K_MIN_RUNTIME] = this->GetMinRuntime();
    mStatisticsMap[BS_TIMER_K_MAX_RUNTIME] = this->GetMaxRuntime();
    mStatisticsMap[BS_TIMER_K_AVERAGE_RUNTIME] = this->GetAverageRuntime();
    mStatisticsMap[BS_TIMER_K_VARIANCE] = this->GetVariance();
    mStatisticsMap[BS_TIMER_K_DEVIATION] = this->GetDeviation();
    mStatisticsMap[BS_TIMER_K_TOTAL] = this->GetTotal();
    if (this->mDataSize > 0) {
        mStatisticsMap[BS_TIMER_K_MAX_BANDWIDTH] = this->GetMaxBandwidth();
        mStatisticsMap[BS_TIMER_K_MIN_BANDWIDTH] = this->GetMinBandwidth();
        mStatisticsMap[BS_TIMER_K_AVERAGE_BANDWIDTH] = this->GetAverageBandwidth();
    }
    if (this->mGridSize > 0) {
        mStatisticsMap[BS_TIMER_K_MIN_THROUGHPUT] = this->GetMinThroughput();
        mStatisticsMap[BS_TIMER_K_MAX_THROUGHPUT] = this->GetMaxThroughput();
        mStatisticsMap[BS_TIMER_K_AVERAGE_THROUGHPUT] = this->GetAverageThroughput();
        mStatisticsMap[BS_TIMER_K_MIN_GFLOPS] = this->GetMinGFLOPS();
        mStatisticsMap[BS_TIMER_K_MAX_GFLOPS] = this->GetMaxGFLOPS();
        mStatisticsMap[BS_TIMER_K_AVERAGE_GFLOPS] = this->GetAverageGFLOPS();
        mStatisticsMap[BS_TIMER_K_OPERATIONS] = this->GetNumberOfOperations();
    }
    return this->mStatisticsMap;
}

double
ChannelStats::GetTotal() {
    return utils::stats::StatisticsHelper::GetTotal(this->mRuntimes);
}

double
ChannelStats::GetMaxRuntime() {
    return utils::stats::StatisticsHelper::GetMax(this->mRuntimes);
}

double
ChannelStats::GetMinRuntime() {
    return utils::stats::StatisticsHelper::GetMin(this->mRuntimes);
}

double
ChannelStats::GetAverageRuntime() {
    return utils::stats::StatisticsHelper::GetAverage(this->mRuntimes);
}

double
ChannelStats::GetVariance() {
    return utils::stats::StatisticsHelper::GetVariance(this->mRuntimes);
}

double
ChannelStats::GetDeviation() {
    return utils::stats::StatisticsHelper::GetDeviation(this->mRuntimes);
}

vector<double>
ChannelStats::GetRuntimes() {
    return this->mRuntimes;
}

unsigned int
ChannelStats::GetNumberOfCalls() const {
    return this->mNumberOfCalls;
}

int
ChannelStats::GetGridSize() const {
    return this->mGridSize;
}

int
ChannelStats::GetDataSize() const {
    return this->mDataSize;
}

double
ChannelStats::GetMaxBandwidth() {
    return this->mDataSize / this->mStatisticsMap[BS_TIMER_K_MIN_RUNTIME];
}

double
ChannelStats::GetMinBandwidth() {
    return this->mDataSize / this->mStatisticsMap[BS_TIMER_K_MAX_RUNTIME];
}

double
ChannelStats::GetAverageBandwidth() {
    return this->mDataSize / this->mStatisticsMap[BS_TIMER_K_AVERAGE_RUNTIME];
}

double
ChannelStats::GetMaxThroughput() {
    return this->mGridSize / this->mStatisticsMap[BS_TIMER_K_MIN_RUNTIME];
}

double
ChannelStats::GetMinThroughput() {
    return this->mGridSize / this->mStatisticsMap[BS_TIMER_K_MAX_RUNTIME];
}

double
ChannelStats::GetAverageThroughput() {
    return this->mGridSize / this->mStatisticsMap[BS_TIMER_K_AVERAGE_RUNTIME];
}

int
ChannelStats::GetNumberOfOperations() const {
    return this->mGridSize * this->mFLOPS;
}

double
ChannelStats::GetMinGFLOPS() {
    return (this->mGridSize * this->mFLOPS) / mStatisticsMap[BS_TIMER_K_MAX_RUNTIME] / BS_TIMER_DU_GIGA;
}

double
ChannelStats::GetMaxGFLOPS() {
    return (this->mGridSize * this->mFLOPS) / mStatisticsMap[BS_TIMER_K_MIN_RUNTIME] / BS_TIMER_DU_GIGA;
}

double
ChannelStats::GetAverageGFLOPS() {
    return (this->mGridSize * this->mFLOPS) / mStatisticsMap[BS_TIMER_K_AVERAGE_RUNTIME] / BS_TIMER_DU_GIGA;
}

vector<double>
ChannelStats::GetBandwidths() {
    return this->mBandwidths;
}
