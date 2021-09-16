/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <operations/components/independents/concrete/trace-managers/SeismicTraceManager.hpp>

#include <operations/configurations/MapKeys.h>
#include <operations/utils/interpolation/Interpolator.hpp>
#include <operations/utils/io/read_utils.h>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/memory/MemoryManager.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <iostream>
#include <utility>
#include <cmath>

using namespace std;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::utils::interpolation;
using namespace bs::base::logger;
using namespace bs::base::memory;
using namespace bs::timer;
using namespace bs::io::streams;
using namespace bs::io::dataunits;

SeismicTraceManager::SeismicTraceManager(
        bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mInterpolation = NONE;
    this->mpTracesHolder = new TracesHolder();
    this->mShotStride = 1;
}

SeismicTraceManager::~SeismicTraceManager() {
    if (this->mpTracesHolder->Traces != nullptr) {
        delete this->mpTracesHolder->Traces;
        mem_free(this->mpTracesHolder->PositionsX);
        mem_free(this->mpTracesHolder->PositionsY);
    }
    delete this->mpSeismicReader;
    delete this->mpTracesHolder;
}

void SeismicTraceManager::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_INTERPOLATION)) {
        string interpolation = OP_K_NONE;
        interpolation = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_INTERPOLATION,
                                                           interpolation);
        if (interpolation == OP_K_NONE) {
            this->mInterpolation = NONE;
        } else if (interpolation == OP_K_SPLINE) {
            this->mInterpolation = SPLINE;
        }
    } else {
        Logger->Error() << "Invalid value for trace-manager->interpolation key : "
                           "supported values [ none | spline ]" << '\n';
        Logger->Info() << "Using default trace-manager->interpolation value: none..." << '\n';
    }

    this->mShotStride = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_SHOT_STRIDE, this->mShotStride);
    Logger->Info() << "Using Shot Stride = " << this->mShotStride << " for trace-manager" << '\n';
    // Initialize reader.
    bool header_only = false;
    header_only = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_HEADER_ONLY,
                                                     header_only);
    std::string reader_type = "segy";
    reader_type = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_TYPE,
                                                     reader_type);
    try {
        SeismicReader::ToReaderType(reader_type);
    } catch (exception &e) {
        Logger->Error() << "Invalid trace manager type provided : " << e.what() << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    Logger->Info() << "Trace manager will use " << reader_type << " format." << '\n';
    nlohmann::json configuration_map =
            nlohmann::json::parse(this->mpConfigurationMap->ToString());
    bs::base::configurations::JSONConfigurationMap io_conf_map(configuration_map);
    this->mpSeismicReader = new SeismicReader(
            SeismicReader::ToReaderType(reader_type), &io_conf_map);
    this->mpSeismicReader->AcquireConfiguration();
    this->mpSeismicReader->SetHeaderOnlyMode(header_only);
}

void SeismicTraceManager::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicTraceManager::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicTraceManager::ReadShot(vector<string> file_names,
                                   uint shot_number,
                                   string sort_key) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    if (this->mpTracesHolder->Traces != nullptr) {
        delete this->mpTracesHolder->Traces;
        mem_free(this->mpTracesHolder->PositionsX);
        mem_free(this->mpTracesHolder->PositionsY);

        this->mpTracesHolder->Traces = nullptr;
        this->mpTracesHolder->PositionsX = nullptr;
        this->mpTracesHolder->PositionsY = nullptr;
    }
    Gather *gather;
    {
        ScopeTimer t("IO::ReadSelectedShotFromSegyFile");
        gather = this->mpSeismicReader->Read({std::to_string(shot_number)});
    }
    if (gather == nullptr) {
        Logger->Error() << "Didn't find a suitable file to read shot ID "
                        << shot_number
                        << " from..." << '\n';
        exit(EXIT_FAILURE);
    } else {
        Logger->Info() << "Reading trace for shot ID "
                       << shot_number << '\n';
    }

    utils::io::ParseGatherToTraces(gather,
                                   &this->mpSourcePoint,
                                   this->mpTracesHolder,
                                   &this->mpTracesHolder->PositionsX,
                                   &this->mpTracesHolder->PositionsY,
                                   this->mpGridBox,
                                   this->mpParameters,
                                   &this->mTotalTime);
    delete gather;
}

void SeismicTraceManager::PreprocessShot() {
    Interpolator::Interpolate(this->mpTracesHolder,
                              this->mpGridBox->GetNT(),
                              this->mTotalTime,
                              this->mInterpolation);
    mpDTraces.Free();
    mpDPositionsX.Free();
    mpDPositionsY.Free();
    mpDTraces.Allocate(this->mpTracesHolder->SampleNT * this->mpTracesHolder->TraceSizePerTimeStep,
                       "Device traces");
    mpDPositionsX.Allocate(this->mpTracesHolder->TraceSizePerTimeStep, "trace x positions");
    mpDPositionsY.Allocate(this->mpTracesHolder->TraceSizePerTimeStep, "trace y positions");
    Device::MemCpy(mpDTraces.GetNativePointer(), this->mpTracesHolder->Traces->GetNativePointer(),
                   this->mpTracesHolder->SampleNT * this->mpTracesHolder->TraceSizePerTimeStep * sizeof(float),
                   Device::COPY_DEVICE_TO_DEVICE);
    Device::MemCpy(mpDPositionsX.GetNativePointer(), this->mpTracesHolder->PositionsX,
                   this->mpTracesHolder->TraceSizePerTimeStep * sizeof(uint),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(mpDPositionsY.GetNativePointer(), this->mpTracesHolder->PositionsY,
                   this->mpTracesHolder->TraceSizePerTimeStep * sizeof(uint),
                   Device::COPY_HOST_TO_DEVICE);
}

void SeismicTraceManager::ApplyIsotropicField() {
    /// @todo To be implemented.
}

void SeismicTraceManager::RevertIsotropicField() {
    /// @todo To be implemented.
}

TracesHolder *SeismicTraceManager::GetTracesHolder() {
    return this->mpTracesHolder;
}

Point3D *SeismicTraceManager::GetSourcePoint() {
    return &this->mpSourcePoint;
}

vector<uint> SeismicTraceManager::GetWorkingShots(
        vector<string> file_names, uint min_shot, uint max_shot, string type) {
    std::vector<TraceHeaderKey> gather_keys = {TraceHeaderKey::FLDR};
    std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;
    this->mpSeismicReader->Initialize(gather_keys, sorting_keys, file_names);
    auto keys = this->mpSeismicReader->GetIdentifiers();
    vector<uint> all_shots;
    for (auto key : keys) {
        size_t unique_val = std::stoull(key[0]);
        if (unique_val >= min_shot && unique_val <= max_shot) {
            all_shots.push_back(unique_val);
        }
    }

    vector<uint> selected_shots;
    for (int i = 0; i < all_shots.size(); i += this->mShotStride) {
        selected_shots.push_back(all_shots[i]);
    }

    return selected_shots;
}
