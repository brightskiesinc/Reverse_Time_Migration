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

#include <operations/components/independents/concrete/model-handlers/SeismicModelHandler.hpp>

#include <operations/utils/sampling/Sampler.hpp>
#include <operations/utils/interpolation/Interpolator.hpp>
#include <operations/utils/io/read_utils.h>
#include <operations/configurations/MapKeys.h>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <bs/io/api/cpp/BSIO.hpp>

#include <prerequisites/libraries/nlohmann/json.hpp>

#include <set>

using namespace std;
using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::helpers;
using namespace operations::utils::sampling;
using namespace operations::utils::io;
using namespace bs::base::logger;
using namespace bs::io::streams;
using namespace bs::io::dataunits;
using namespace bs::timer;


SeismicModelHandler::SeismicModelHandler(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mpGridBox = new GridBox();
    this->mReaderType = "segy";
    // Default as unit will be centimeter and scaling to meter.
    this->mDepthSamplingScaler = 1e3;
};

SeismicModelHandler::~SeismicModelHandler() = default;

void SeismicModelHandler::AcquireConfiguration() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mReaderType = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_TYPE,
                                                           this->mReaderType);
    try {
        SeismicReader::ToReaderType(this->mReaderType);
    } catch (exception &e) {
        Logger->Error() << "Invalid model handler type provided : " << e.what() << '\n';
        Logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    this->mDepthSamplingScaler = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES,
                                                                    OP_K_DEPTH_SAMPLING_SCALING,
                                                                    this->mDepthSamplingScaler);
    Logger->Info() << "Model handler will use " << this->mReaderType << " format." << '\n';
}

void SeismicModelHandler::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicModelHandler::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpGridBox = apGridBox;
    if (this->mpGridBox == nullptr) {
        Logger->Error() << "No GridBox provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void SeismicModelHandler::SetDependentComponents(
        ComponentsMap<DependentComponent> *apDependentComponentsMap) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    HasDependents::SetDependentComponents(apDependentComponentsMap);
    this->mpWaveFieldsMemoryHandler =
            (WaveFieldsMemoryHandler *)
                    this->GetDependentComponentsMap()->Get(MEMORY_HANDLER);
    if (this->mpWaveFieldsMemoryHandler == nullptr) {
        Logger->Error() << "No Wave Fields Memory Handler provided... "
                        << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

GridBox *SeismicModelHandler::ReadModel(map<string, string> file_names) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->Initialize(file_names);

    int initial_nx = this->mpGridBox->GetInitialAxis()->GetXAxis().GetLogicalAxisSize();
    int initial_ny = this->mpGridBox->GetInitialAxis()->GetYAxis().GetLogicalAxisSize();
    int initial_nz = this->mpGridBox->GetInitialAxis()->GetZAxis().GetLogicalAxisSize();

    int actual_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int actual_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int actual_nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    int logical_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
    int logical_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
    int logical_nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

    int model_size = actual_nx * actual_ny * actual_nz;
    int logical_size = logical_nx * logical_ny * logical_nz;
    int initial_size = initial_nx * initial_ny * initial_nz;

    int offset = this->mpParameters->GetBoundaryLength() + this->mpParameters->GetHalfLength();
    int offset_y = actual_ny > 1 ? this->mpParameters->GetBoundaryLength() + this->mpParameters->GetHalfLength() : 0;
    nlohmann::json configuration_map =
            nlohmann::json::parse(this->mpConfigurationMap->ToString());
    bs::base::configurations::JSONConfigurationMap io_conf_map(
            configuration_map);
    Reader *seismic_io_reader = new SeismicReader(
            SeismicReader::ToReaderType(this->mReaderType),
            &io_conf_map);
    seismic_io_reader->AcquireConfiguration();
    map<string, float> maximums;
    for (auto const &parameter : this->PARAMS_NAMES) {
        maximums[parameter.second] = 0.0f;
    }
    for (auto const &parameter : this->PARAMS_NAMES) {
        GridBox::Key param_key = parameter.first;
        string param_name = parameter.second;
        vector<Gather *> gathers;
        if (file_names.find(param_name) == file_names.end() ||
            file_names[param_name].empty()) {
        } else {
            auto channelName = "IO::Read" + param_name + "FromInputFile";

            ElasticTimer timer(channelName.c_str());
            timer.Start();
            std::vector<TraceHeaderKey> empty_gather_keys;
            std::vector<std::pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys;
            std::vector<std::string> paths = {file_names[param_name]};
            seismic_io_reader->Initialize(empty_gather_keys, sorting_keys,
                                          paths);
            gathers = seismic_io_reader->ReadAll();
            timer.Stop();
        }

        auto parameter_host_buffer = new float[initial_size];
        auto resized_host_buffer = new float[model_size];
        auto logical_host_buffer = new float[logical_size];

        memset(parameter_host_buffer, 0, initial_size * sizeof(float));
        memset(resized_host_buffer, 0, model_size * sizeof(float));
        memset(logical_host_buffer, 0, logical_size * sizeof(float));

        uint index = 0;

        if (gathers.empty()) {
            Logger->Info() << "Please provide " << param_name << " model file..." << '\n';
            float default_value = 0.0f;
            // Unless density, assume acoustic and fill with zeros.
            // For density, assume constant density.
            if (param_name == "density") {
                default_value = 1.0f;
            }

            for (unsigned int k = offset_y; k < initial_ny - offset_y; k++) {
                for (unsigned int i = offset; i < initial_nx - offset; i++) {
                    for (unsigned int j = offset; j < initial_nz - offset; j++) {
                        index = k * initial_nx * initial_nz + j * initial_nx + i;
                        parameter_host_buffer[index] = default_value;
                    }
                }
            }
        } else {
            auto gather = CombineGather(gathers);
            RemoveDuplicatesFromGather(gather);
            /// sort data
            vector<pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys = {
                    {TraceHeaderKey::SY, Gather::SortDirection::ASC},
                    {TraceHeaderKey::SX, Gather::SortDirection::ASC}
            };
            gather->SortGather(sorting_keys);

            /// Reading and maximum identification
            for (unsigned int k = offset_y; k < initial_ny - offset_y; k++) {
                for (unsigned int i = offset; i < initial_nx - offset; i++) {
                    for (unsigned int j = offset; j < initial_nz - offset; j++) {
                        uint index = k * initial_nx * initial_nz + j * initial_nx + i;
                        uint trace_index = (k - offset_y) * (initial_nx - 2 * offset) + (i - offset);
                        float temp =
                                parameter_host_buffer[index] =
                                        gather->GetTrace(trace_index)->GetTraceData()[j - offset];
                        if (temp > maximums[param_name]) {
                            maximums[param_name] = temp;
                        }
                    }
                }
            }
            delete gather;
            gathers.clear();
            seismic_io_reader->Finalize();
        }

        Sampler::Resize(parameter_host_buffer, logical_host_buffer,
                        mpGridBox->GetInitialAxis(), mpGridBox->GetAfterSamplingAxis(),
                        mpParameters);

        for (unsigned int k = offset_y; k < logical_ny - offset_y; k++) {
            for (unsigned int j = offset; j < logical_nz - offset; j++) {
                for (unsigned int i = offset; i < logical_nx - offset; i++) {
                    uint actual_index = k * actual_nx * actual_nz + j * actual_nx + i;
                    uint logical_index = k * logical_nx * logical_nz + j * logical_nx + i;
                    resized_host_buffer[actual_index] = logical_host_buffer[logical_index];
                }
            }
        }

        auto parameter_ptr = this->mpGridBox->Get(param_key)->GetNativePointer();
        Device::MemCpy(parameter_ptr, resized_host_buffer, sizeof(float) * model_size, Device::COPY_HOST_TO_DEVICE);
        delete[] parameter_host_buffer;
        delete[] resized_host_buffer;
        delete[] logical_host_buffer;

    }

    this->mpGridBox->SetDT(GetSuitableDT(
            this->mpParameters->GetSecondDerivativeFDCoefficient(),
            maximums, this->mpParameters->GetHalfLength(), this->mpParameters->GetRelaxedDT()));

    delete seismic_io_reader;
    return this->mpGridBox;
}

void SeismicModelHandler::Initialize(map<string, string> file_names) {

    LoggerSystem *Logger = LoggerSystem::GetInstance();

    if (file_names["velocity"].empty()) {
        Logger->Error() << "Please provide a velocity model... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    nlohmann::json configuration_map =
            nlohmann::json::parse(this->mpConfigurationMap->ToString());
    bs::base::configurations::JSONConfigurationMap io_conf_map(
            configuration_map);
    Reader *seismic_io_reader = new SeismicReader(
            SeismicReader::ToReaderType(this->mReaderType),
            &io_conf_map);

    ElasticTimer timer("IO::ReadVelocityMetadata");
    timer.Start();
    std::vector<TraceHeaderKey> empty_gather_keys;
    vector<pair<TraceHeaderKey, Gather::SortDirection>> sorting_keys = {
            {TraceHeaderKey::SY, Gather::SortDirection::ASC},
            {TraceHeaderKey::SX, Gather::SortDirection::ASC}
    };
    std::vector<std::string> paths = {file_names["velocity"]};
    seismic_io_reader->Initialize(empty_gather_keys, sorting_keys,
                                  paths);
    seismic_io_reader->SetHeaderOnlyMode(true);
    vector<Gather *> gathers;
    gathers = seismic_io_reader->ReadAll();
    timer.Stop();
    auto gather = CombineGather(gathers);
    RemoveDuplicatesFromGather(gather);
    gather->SortGather(sorting_keys);
    set<float> x_locations;
    set<float> y_locations;
    for (uint i = 0; i < gather->GetNumberTraces(); i++) {
        x_locations.emplace(
                gather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SX));
        y_locations.emplace(
                gather->GetTrace(i)->GetScaledCoordinateHeader(TraceHeaderKey::SY));
    }

    uint x_size = x_locations.size();
    uint y_size = y_locations.size();
    uint z_size = gather->GetTrace(0)->GetNumberOfSamples();

    if (y_size > 1) {
        this->mpGridBox->SetInitialAxis(new Axis3D<unsigned int>(x_size, y_size, z_size));
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH,
                                                                  this->mpParameters->GetBoundaryLength());
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                           this->mpParameters->GetHalfLength());
    } else {
        this->mpGridBox->SetInitialAxis(new Axis3D<unsigned int>(x_size, 1, z_size));
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, 0);
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, 0);
    }

    this->mpGridBox->GetInitialAxis()->GetXAxis().AddBoundary(OP_DIREC_BOTH,
                                                              this->mpParameters->GetBoundaryLength()); //x_size =5395
    this->mpGridBox->GetInitialAxis()->GetZAxis().AddBoundary(OP_DIREC_BOTH, this->mpParameters->GetBoundaryLength());
    this->mpGridBox->GetInitialAxis()->GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                       this->mpParameters->GetHalfLength());
    this->mpGridBox->GetInitialAxis()->GetZAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                       this->mpParameters->GetHalfLength());

    float dx, dy, dz, dt;
    dx = *(++x_locations.begin()) - (*x_locations.begin());
    if (y_locations.size() > 1) {
        dy = *(++y_locations.begin()) - (*y_locations.begin());
    } else {
        dy = 0;
    }
    // If given model is a 2D line.
    if (IsLineGather(gather)) {
        dx = sqrt(dx * dx + dy * dy);
        dy = 0;

        y_size = 1;
        this->mpGridBox->GetInitialAxis()->SetYAxis(y_size);
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, 0);
        this->mpGridBox->GetInitialAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, 0);

    }
    if (this->mpGridBox->GetInitialAxis()->GetYAxis().GetLogicalAxisSize() == 1) {
        Logger->Info() << "Operating on a 2D model" << '\n';

    } else {
        Logger->Info() << "Operating on a 3D model" << '\n';
    }

    dz = gather->GetSamplingRate() / this->mDepthSamplingScaler;
    this->mpGridBox->GetInitialAxis()->GetXAxis().SetCellDimension(dx);
    this->mpGridBox->GetInitialAxis()->GetYAxis().SetCellDimension(dy);
    this->mpGridBox->GetInitialAxis()->GetZAxis().SetCellDimension(dz);

    this->mpGridBox->GetInitialAxis()->GetXAxis().SetReferencePoint(gather->GetTrace(0)->GetScaledCoordinateHeader(
            TraceHeaderKey::SX));
    this->mpGridBox->GetInitialAxis()->GetZAxis().SetReferencePoint(0);
    this->mpGridBox->GetInitialAxis()->GetYAxis().SetReferencePoint(gather->GetTrace(0)->GetScaledCoordinateHeader(
            TraceHeaderKey::SY));

    this->mpGridBox->SetAfterSamplingAxis(new Axis3D<unsigned int>(*this->mpGridBox->GetInitialAxis()));


    Logger->Info() << "Reference X \t: " << this->mpGridBox->GetInitialAxis()->GetXAxis().GetReferencePoint() << '\n';
    Logger->Info() << "Reference Y \t: " << this->mpGridBox->GetInitialAxis()->GetYAxis().GetReferencePoint() << '\n';
    Logger->Info() << "Reference Z \t: " << this->mpGridBox->GetInitialAxis()->GetZAxis().GetReferencePoint() << '\n';


    /// If there is no window then window size equals full model size
    if (this->mpParameters->IsUsingWindow()) {
        this->mpGridBox->SetWindowStart(X_AXIS, 0);
        this->mpGridBox->SetWindowStart(Y_AXIS, 0);
        this->mpGridBox->SetWindowStart(Z_AXIS, 0);
        unsigned int window_size = this->mpParameters->GetLeftWindow() + this->mpParameters->GetRightWindow() + 1;

        this->mpGridBox->SetWindowAxis(new Axis3D<unsigned int>(0, 0, 0)); //so solve segmentation fault

        if (this->mpParameters->GetLeftWindow() == 0 && this->mpParameters->GetRightWindow() == 0) {
            this->mpGridBox->GetWindowAxis()->SetXAxis(x_size);
            this->mpGridBox->GetWindowAxis()->GetXAxis().AddBoundary(OP_DIREC_BOTH,
                                                                     this->mpParameters->GetBoundaryLength());
            this->mpGridBox->GetWindowAxis()->GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                              this->mpParameters->GetHalfLength());

        } else {
            this->mpGridBox->GetWindowAxis()->SetXAxis(min(window_size, x_size));
            this->mpGridBox->GetWindowAxis()->GetXAxis().AddBoundary(OP_DIREC_BOTH,
                                                                     this->mpParameters->GetBoundaryLength());
            this->mpGridBox->GetWindowAxis()->GetXAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                              this->mpParameters->GetHalfLength());

        }
        if (this->mpParameters->GetDepthWindow() == 0) {
            this->mpGridBox->GetWindowAxis()->SetZAxis(z_size);
            this->mpGridBox->GetWindowAxis()->GetZAxis().AddBoundary(OP_DIREC_BOTH,
                                                                     this->mpParameters->GetBoundaryLength());
            this->mpGridBox->GetWindowAxis()->GetZAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                              this->mpParameters->GetHalfLength());

        } else {
            this->mpGridBox->GetWindowAxis()->SetZAxis(min(window_size, z_size));
            this->mpGridBox->GetWindowAxis()->GetZAxis().AddBoundary(OP_DIREC_BOTH,
                                                                     this->mpParameters->GetBoundaryLength());
            this->mpGridBox->GetWindowAxis()->GetZAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                              this->mpParameters->GetHalfLength());

        }
        if ((this->mpParameters->GetFrontWindow() == 0 &&
             this->mpParameters->GetBackWindow() == 0) ||
            y_size == 1) {
            this->mpGridBox->GetWindowAxis()->SetYAxis(y_size);
            this->mpGridBox->GetWindowAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH, 0);
            this->mpGridBox->GetWindowAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH, 0);

        } else {
            this->mpGridBox->GetWindowAxis()->SetYAxis(min(window_size, y_size));
            this->mpGridBox->GetWindowAxis()->GetYAxis().AddBoundary(OP_DIREC_BOTH,
                                                                     this->mpParameters->GetBoundaryLength());
            this->mpGridBox->GetWindowAxis()->GetYAxis().AddHalfLengthPadding(OP_DIREC_BOTH,
                                                                              this->mpParameters->GetHalfLength());

        }

        this->mpGridBox->GetWindowAxis()->GetXAxis().SetCellDimension(dx);
        this->mpGridBox->GetWindowAxis()->GetYAxis().SetCellDimension(dy);
        this->mpGridBox->GetWindowAxis()->GetZAxis().SetCellDimension(dz);
    } else {

        this->mpGridBox->SetWindowStart(X_AXIS, 0);
        this->mpGridBox->SetWindowStart(Y_AXIS, 0);
        this->mpGridBox->SetWindowStart(Z_AXIS, 0);

        this->mpGridBox->SetWindowAxis(new Axis3D<unsigned int>(*this->mpGridBox->GetInitialAxis()));
    }

    // Assume minimum velocity to be 1500 m/s for now.
    uint initial_logical_x = this->mpGridBox->GetInitialAxis()->GetXAxis().GetLogicalAxisSize();
    uint initial_logical_y = this->mpGridBox->GetInitialAxis()->GetYAxis().GetLogicalAxisSize();
    uint initial_logical_z = this->mpGridBox->GetInitialAxis()->GetZAxis().GetLogicalAxisSize();

    Sampler::CalculateAdaptiveCellDimensions(mpGridBox, mpParameters, 1500,
                                             this->mpParameters->GetMaxPropagationFrequency());

    this->SetupPadding();

    this->RegisterWaveFields(initial_logical_x, initial_logical_y, initial_logical_z);
    this->RegisterParameters(initial_logical_x, initial_logical_y, initial_logical_z);

    this->AllocateWaveFields();
    this->AllocateParameters();
    seismic_io_reader->Finalize();
    delete seismic_io_reader;
    this->mpGridBox->SetParameterGatherHeader(gather);

}

void SeismicModelHandler::RegisterWaveFields(uint nx, uint ny, uint nz) {
    /// Register wave field by order
    if (this->mpParameters->GetEquationOrder() == FIRST) {
        this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRTC | CURR | DIR_Z);
        this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRTC | CURR | DIR_X);
        if (ny > 1) {
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRTC | CURR | DIR_Y);
        }
        /// Register wave field by approximation
        if (this->mpParameters->GetApproximation() == ISOTROPIC) {
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | CURR);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | NEXT);
        }
    } else if (this->mpParameters->GetEquationOrder() == SECOND) {
        /// Register wave field by approximation
        if (this->mpParameters->GetApproximation() == ISOTROPIC) {
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | CURR);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | PREV);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | NEXT);
        } else if (this->mpParameters->GetApproximation() == VTI ||
                   this->mpParameters->GetApproximation() == TTI) {
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | CURR | DIR_Z);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | PREV | DIR_Z);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | NEXT | DIR_Z);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | CURR | DIR_X);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | PREV | DIR_X);
            this->WAVE_FIELDS_NAMES.push_back(WAVE | GB_PRSS | NEXT | DIR_X);
        }
    }
}

void SeismicModelHandler::RegisterParameters(uint nx, uint ny, uint nz) {
    /// Register parameters by order
    this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_VEL, "velocity"));
    if (this->mpParameters->GetEquationOrder() == FIRST) {
        this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_DEN, "density"));
    }
    if (this->mpParameters->GetApproximation() == VTI ||
        this->mpParameters->GetApproximation() == TTI) {
        this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_DLT, "delta"));
        this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_EPS, "epsilon"));
    }
    if (this->mpParameters->GetApproximation() == TTI) {
        this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_THT, "theta"));
        this->PARAMS_NAMES.push_back(std::make_pair(PARM | GB_PHI, "phi"));
    }
}

void SeismicModelHandler::AllocateWaveFields() {
    uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint window_size = wnx * wny * wnz;
    /// Allocating and zeroing wave fields.
    for (auto wave_field : this->WAVE_FIELDS_NAMES) {
        if (!GridBox::Includes(wave_field, NEXT)) {
            auto frame_buffer = new FrameBuffer<float>();
            frame_buffer->Allocate(window_size,
                                   mpParameters->GetHalfLength(),
                                   GridBox::Stringify(wave_field));
            this->mpWaveFieldsMemoryHandler->FirstTouch(frame_buffer->GetNativePointer(), this->mpGridBox, true);
            this->mpGridBox->RegisterWaveField(wave_field, frame_buffer);
            if (GridBox::Includes(wave_field, GB_PRSS | PREV)) {
                GridBox::Replace(&wave_field, PREV, NEXT);
                this->mpGridBox->RegisterWaveField(wave_field, frame_buffer);
            } else if (this->mpParameters->GetEquationOrder() == FIRST &&
                       GridBox::Includes(wave_field, GB_PRSS | CURR)) {
                GridBox::Replace(&wave_field, CURR, NEXT);
                this->mpGridBox->RegisterWaveField(wave_field, frame_buffer);
            }
        }
    }
}

void SeismicModelHandler::AllocateParameters() {
    uint nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    uint nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();
    uint grid_size = nx * nz * ny;
    uint wnx = this->mpGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();
    uint window_size = wnx * wnz * wny;

    if (this->mpParameters->IsUsingWindow()) {
        for (auto const &parameter : this->PARAMS_NAMES) {
            GridBox::Key param_key = parameter.first;
            string param_name = parameter.second;
            auto frame_buffer = new FrameBuffer<float>();
            frame_buffer->Allocate(grid_size,
                                   mpParameters->GetHalfLength(),
                                   param_name);
            this->mpWaveFieldsMemoryHandler->FirstTouch(frame_buffer->GetNativePointer(), this->mpGridBox);
            auto frame_buffer_window = new FrameBuffer<float>();
            frame_buffer_window->Allocate(window_size,
                                          mpParameters->GetHalfLength(),
                                          param_name);
            this->mpWaveFieldsMemoryHandler->FirstTouch(frame_buffer_window->GetNativePointer(), this->mpGridBox, true);
            this->mpGridBox->RegisterParameter(param_key, frame_buffer, frame_buffer_window);
        }
    } else {
        for (auto const &parameter : this->PARAMS_NAMES) {
            GridBox::Key param_key = parameter.first;
            string param_name = parameter.second;
            auto frame_buffer = new FrameBuffer<float>();
            frame_buffer->Allocate(grid_size,
                                   mpParameters->GetHalfLength(),
                                   param_name);
            this->mpWaveFieldsMemoryHandler->FirstTouch(frame_buffer->GetNativePointer(), this->mpGridBox);
            this->mpGridBox->RegisterParameter(param_key, frame_buffer);
        }
    }
}

float SeismicModelHandler::GetSuitableDT
        (float *coefficients, map<string, float> maximums, int half_length, float dt_relax) {

    float dx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
    float dy = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
    float dz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();
    uint ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();

    // Calculate dt through finite difference stability equation
    float dxSquare = 1 / (dx * dx);
    float dySquare;
    if (ny != 1)
        dySquare = 1 / (dy * dy);
    else // case of 2D
        dySquare = 0.0;
    float dzSquare = 1 / (dz * dz);
    float distanceM = 1 / (sqrtf(dxSquare + dySquare + dzSquare));
    /// The sum of absolute value of weights for second derivative if
    /// du per dt. we use second order so the weights are (-1,2,1)
    float a1 = 4;
    float a2 = 0;
    for (int i = 1; i <= half_length; i++) {
        a2 += fabs(coefficients[i]);
    }
    a2 *= 2.0;
    /// The sum of absolute values for second derivative id
    /// du per dx ( one dimension only )
    a2 += fabs(coefficients[0]);
    if (this->mpParameters->GetApproximation() == VTI) {
        a2 *= (2 + 4 * maximums["epsilon"] + sqrtf(1 + 2 * maximums["delta"]));
    } else if (this->mpParameters->GetApproximation() == TTI) {
        float tti_coefficients = (powf(cos(maximums["theta"]), 2) * sin(2 * maximums["phi"])) +
                                 (sin(2 * maximums["theta"]) * (sin(maximums["phi"]) + cos(maximums["phi"])));
        float b1 = (1 + 2 * maximums["epsilon"]) * (2 - tti_coefficients);
        float b2 = sqrtf(1 + 2 * maximums["delta"]);
        a2 *= (b1 + b2);
    }
    float dt = ((sqrtf(a1 / a2)) * distanceM) / maximums["velocity"] * dt_relax;
    return dt;
}

void SeismicModelHandler::PostProcessMigration(MigrationData *apMigrationData) {
    int initial_nx = this->mpGridBox->GetInitialAxis()->GetXAxis().GetLogicalAxisSize(); //logical
    int initial_ny = this->mpGridBox->GetInitialAxis()->GetYAxis().GetLogicalAxisSize();
    int initial_nz = this->mpGridBox->GetInitialAxis()->GetZAxis().GetLogicalAxisSize();

    int actual_nx = apMigrationData->GetGridSize(X_AXIS);
    int actual_ny = apMigrationData->GetGridSize(Y_AXIS);
    int actual_nz = apMigrationData->GetGridSize(Z_AXIS);
    size_t gather_num = apMigrationData->GetGatherDimension();
    int offset = mpParameters->GetBoundaryLength() + mpParameters->GetHalfLength();

    int base_nx = this->mpGridBox->GetInitialAxis()->GetXAxis().GetAxisSize();
    int base_ny = this->mpGridBox->GetInitialAxis()->GetYAxis().GetAxisSize();
    int base_nz = this->mpGridBox->GetInitialAxis()->GetZAxis().GetAxisSize();

    int logical_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
    int logical_ny = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
    int logical_nz = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

    int after_actual_nx = this->mpGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
    int after_actual_nz = this->mpGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
    int after_actual_ny = this->mpGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

    size_t initial_frame = initial_nx * initial_ny * initial_nz;
    size_t base_frame = base_nx * base_ny * base_nz;
    size_t base_size = base_frame * gather_num;
    size_t actual_frame = actual_nx * actual_nz * actual_ny;
    size_t after_logical_frame = logical_nx * logical_ny * logical_nz;
    auto parameter_host_buffer = new float[initial_frame];
    auto after_logical_buffer = new float[after_logical_frame];
    for (auto const &result : apMigrationData->GetResults()) {
        auto unpadded_parameter_host_buffer = new float[base_size];
        auto original = result->GetData();
        for (uint frame_index = 0; frame_index < gather_num; frame_index++) {
            auto frame_ptr = &unpadded_parameter_host_buffer[frame_index * base_frame];
            memset(parameter_host_buffer, 0, initial_frame * sizeof(float));
            memset(frame_ptr, 0, base_frame * sizeof(float));
            auto original_frame = &(original[frame_index * actual_frame]);
            // Set data to without computational/non-computation padding
            for (int iy = 0; iy < logical_ny; iy++) {
                for (int iz = 0; iz < logical_nz; iz++) {
                    for (int ix = 0; ix < logical_nx; ix++) {
                        int index = iy * logical_nz * logical_nx + iz * logical_nx + ix;
                        int alt_index = iy * after_actual_nz * after_actual_nx + iz * after_actual_nx + ix;
                        after_logical_buffer[index] = original_frame[alt_index];
                    }
                }
            }

            // Resize to initial
            Sampler::Resize(after_logical_buffer, parameter_host_buffer,
                            mpGridBox->GetAfterSamplingAxis(),
                            mpGridBox->GetInitialAxis(),
                            mpParameters);

            // Remove padding.
            for (int iy = 0; iy < base_ny; iy++) {
                for (int iz = 0; iz < base_nz; iz++) {
                    for (int ix = 0; ix < base_nx; ix++) {
                        int index = iy * base_nz * base_nx + iz * base_nx + ix;
                        int padded_index = (iz + offset) * initial_nx + ix + offset;
                        if (initial_ny != 1) {
                            padded_index += ((iy + offset) * initial_nx * initial_nz);
                        }
                        frame_ptr[index] = parameter_host_buffer[padded_index];
                    }
                }
            }
        }
        // Set result.
        delete[] original;
        result->SetData(unpadded_parameter_host_buffer);
    }
    delete[] parameter_host_buffer;
    delete[] after_logical_buffer;
    // Update migration data results.
    apMigrationData->SetGridSize(X_AXIS, base_nx);
    apMigrationData->SetGridSize(Z_AXIS, base_nz);
    apMigrationData->SetGridSize(Y_AXIS, base_ny);
    apMigrationData->SetCellDimensions(X_AXIS, mpGridBox->GetInitialAxis()->GetXAxis().GetCellDimension());
    apMigrationData->SetCellDimensions(Z_AXIS, mpGridBox->GetInitialAxis()->GetYAxis().GetCellDimension());
    apMigrationData->SetCellDimensions(Y_AXIS, mpGridBox->GetInitialAxis()->GetZAxis().GetCellDimension());
}
