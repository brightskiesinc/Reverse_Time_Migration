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

#include <iostream>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#include <operations/components/independents/concrete/trace-writers/SeismicTraceWriter.hpp>

#include <operations/configurations/MapKeys.h>
#include <operations/utils/io/write_utils.h>

using namespace std;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::base::logger;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;

SeismicTraceWriter::SeismicTraceWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

SeismicTraceWriter::~SeismicTraceWriter() = default;

void SeismicTraceWriter::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    // Initialize writer.
    std::string writer_type = "segy";
    writer_type = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_TYPE,
                                                     writer_type);
    try {
        SeismicWriter::ToWriterType(writer_type);
    } catch (exception &e) {
        Logger->Error() << "Invalid trace writer type provided : " << e.what() << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    Logger->Info() << "Trace writer will use " << writer_type << " format." << '\n';
    nlohmann::json configuration_map =
            nlohmann::json::parse(this->mpConfigurationMap->ToString());
    bs::base::configurations::JSONConfigurationMap io_conf_map(configuration_map);
    this->mpSeismicWriter = new SeismicWriter(
            SeismicWriter::ToWriterType(writer_type), &io_conf_map);
    this->mpSeismicWriter->AcquireConfiguration();
    std::string output = "modeling_output";
    output = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_OUTPUT_FILE,
                                                output);
    this->mpSeismicWriter->Initialize(output);
    Logger->Info() << "Trace writer will write in " <<
                   output + this->mpSeismicWriter->GetExtension() << '\n';
}

void SeismicTraceWriter::SetComputationParameters(
        ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicTraceWriter::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicTraceWriter::StartRecordingInstance(TracesHolder &aTracesHolder) {
    mSampleNumber = aTracesHolder.SampleNT;
    mTraceNumber = aTracesHolder.TraceSizePerTimeStep;
    mTraceSampling = aTracesHolder.SampleDT;
    mpDTraces.Allocate(mSampleNumber * mTraceNumber,
                       "Device traces");
    mpDPositionsX.Allocate(mTraceNumber, "trace x positions");
    mpDPositionsY.Allocate(mTraceNumber, "trace y positions");


    uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;
    for (auto const &wave_field : this->mpGridBox->GetWaveFields()) {
        Device::MemSet(wave_field.second->GetNativePointer(), 0.0f, window_size * sizeof(float));
    }
    Device::MemSet(mpDTraces.GetNativePointer(), 0,
                   mSampleNumber * mTraceNumber * sizeof(float));
    Device::MemCpy(mpDPositionsX.GetNativePointer(), aTracesHolder.PositionsX,
                   mTraceNumber * sizeof(uint),
                   Device::COPY_HOST_TO_DEVICE);
    Device::MemCpy(mpDPositionsY.GetNativePointer(), aTracesHolder.PositionsY,
                   mTraceNumber * sizeof(uint),
                   Device::COPY_HOST_TO_DEVICE);
}

void SeismicTraceWriter::Finalize() {
    this->mpSeismicWriter->Finalize();
    delete this->mpSeismicWriter;
    this->mpSeismicWriter = nullptr;
}

void SeismicTraceWriter::FinishRecordingInstance(uint shot_id) {
    auto trace_values = mpDTraces.GetHostPointer();
    auto locations_x = mpDPositionsX.GetHostPointer();
    auto locations_y = mpDPositionsY.GetHostPointer();

    uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    auto gather = new Gather();
    for (int trace_index = 0; trace_index < mTraceNumber; trace_index++) {
        auto trace = new Trace(mSampleNumber);
        int16_t factor = -1000;
        trace->SetTraceHeaderKeyValue(TraceHeaderKey::SCALCO, factor);
        auto local_ix = this->mpGridBox->GetWindowStart(X_AXIS)
                        + locations_x[trace_index]
                        - this->mpParameters->GetBoundaryLength()
                        - this->mpParameters->GetHalfLength();
        auto local_iy = this->mpGridBox->GetWindowStart(Y_AXIS)
                        + locations_y[trace_index]
                        - this->mpParameters->GetBoundaryLength()
                        - this->mpParameters->GetHalfLength();

        float loc_x = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetReferencePoint() +
                      local_ix * this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();

        float loc_y = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint() +
                      local_iy * this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();

        if (ny == 1) {
            loc_y = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetReferencePoint();
        }


        trace->SetScaledCoordinateHeader(TraceHeaderKey::SX, loc_x);
        trace->SetScaledCoordinateHeader(TraceHeaderKey::SY, loc_y);
        trace->SetScaledCoordinateHeader(TraceHeaderKey::GX, loc_x);
        trace->SetScaledCoordinateHeader(TraceHeaderKey::GY, loc_y);
        uint16_t sampling = mTraceSampling * 1e6;
        trace->SetTraceHeaderKeyValue(TraceHeaderKey::DT, sampling);
        trace->SetTraceHeaderKeyValue(TraceHeaderKey::FLDR, shot_id);
        trace->SetTraceData(new float[mSampleNumber]);
        for (int is = 0; is < mSampleNumber; is++) {
            trace->GetTraceData()[is] =
                    trace_values[is * mTraceNumber + trace_index];
        }
        gather->AddTrace(trace);
    }
    gather->SetSamplingRate(mTraceSampling * 1e6f);
    string val = to_string(shot_id);
    gather->SetUniqueKeyValue(TraceHeaderKey::FLDR, val);
    this->mpSeismicWriter->Write(gather);
    delete gather;
    mpDTraces.Free();
    mpDPositionsX.Free();
    mpDPositionsY.Free();
}
