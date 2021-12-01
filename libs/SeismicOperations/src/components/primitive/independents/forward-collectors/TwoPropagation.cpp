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

#include <sys/stat.h>

#include <bs/base/api/cpp/BSBase.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <operations/components/independents/concrete/forward-collectors/TwoPropagation.hpp>
#include <operations/configurations/MapKeys.h>
#include <operations/utils/compressor/Compressor.hpp>

using namespace std;
using namespace bs::timer;
using namespace bs::base::logger;
using namespace bs::base::memory;
using namespace operations::helpers;
using namespace operations::components;
using namespace operations::components::helpers;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::utils::compressors;

static float* initial_internalGridbox_curr = nullptr;

TwoPropagation::TwoPropagation(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mpInternalGridBox = new GridBox();
    this->mpForwardPressure = nullptr;
    this->mIsMemoryFit = false;
    this->mTimeCounter = 0;
    this->mIsCompression = false;
    this->mZFP_Tolerance = 0.01f;
    this->mZFP_Parallel = true;
    this->mZFP_IsRelative = false;
    this->mMaxNT = 0;
}

TwoPropagation::~TwoPropagation() {
    if (this->mpForwardPressureHostMemory != nullptr) {
        mem_free(this->mpForwardPressureHostMemory);
    }
    delete this->mpForwardPressure;
    this->mpInternalGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z, initial_internalGridbox_curr);
    this->mpWaveFieldsMemoryHandler->FreeWaveFields(this->mpInternalGridBox);
    delete this->mpInternalGridBox;
}

void TwoPropagation::AcquireConfiguration() {
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_COMPRESSION)) {
        this->mIsCompression = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_COMPRESSION,
                                                                  this->mIsCompression);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_WRITE_PATH)) {
        std::string write_path = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_WRITE_PATH,
                                                                    this->mWritePath);
        mkdir(write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        this->mWritePath = write_path + "/two_prop";
        mkdir(this->mWritePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_ZFP_TOLERANCE)) {
        this->mZFP_Tolerance = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_ZFP_TOLERANCE,
                                                                  this->mZFP_Tolerance);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_ZFP_PARALLEL)) {
        this->mZFP_Parallel = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_ZFP_PARALLEL,
                                                                 this->mZFP_Parallel);
    }
    if (this->mpConfigurationMap->Contains(OP_K_PROPRIETIES, OP_K_ZFP_RELATIVE)) {
        this->mZFP_IsRelative = this->mpConfigurationMap->GetValue(OP_K_PROPRIETIES, OP_K_ZFP_RELATIVE,
                                                                   this->mZFP_IsRelative);
    }
}

void TwoPropagation::FetchForward() {

    uint wnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();


    uint const window_size = wnx * wny * wnz;
    // Retrieve data from files to host buffer
    if ((this->mTimeCounter + 1) % this->mMaxNT == 0) {
        if (this->mIsCompression) { ;
            string str = this->mWritePath + "/temp_" + to_string(this->mTimeCounter / this->mMaxNT);
            {
                ScopeTimer t("ForwardCollector::Decompression");
                Compressor::Decompress(this->mpForwardPressureHostMemory, wnx, wny, wnz,
                                       this->mMaxNT,
                                       (double) this->mZFP_Tolerance,
                                       this->mZFP_Parallel,
                                       str.c_str(),
                                       this->mZFP_IsRelative);
            }
        } else {
            string str = this->mWritePath + "/temp_" + to_string(this->mTimeCounter / this->mMaxNT);
            {
                ScopeTimer t("IO::ReadForward");
                bin_file_load(str.c_str(), this->mpForwardPressureHostMemory, this->mMaxNT * window_size);
            }
        }
    }
    // Retrieve data from host buffer
    if ((this->mTimeCounter + 1) % this->mMaxDeviceNT == 0) {

        int host_index = (this->mTimeCounter + 1) / this->mMaxDeviceNT - 1;

        Device::MemCpy(this->mpForwardPressure->GetNativePointer(),
                       this->mpForwardPressureHostMemory +
                       (host_index % this->mpMaxNTRatio) * (this->mMaxDeviceNT * window_size),
                       this->mMaxDeviceNT * window_size * sizeof(float),
                       Device::COPY_HOST_TO_DEVICE);
    }
    this->mpInternalGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z,
                                 this->mpForwardPressure->GetNativePointer() +
                                 ((this->mTimeCounter) % this->mMaxDeviceNT) * window_size);
    this->mTimeCounter--;
}

void TwoPropagation::ResetGrid(bool aIsForwardRun) {


    uint wnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;

    if (aIsForwardRun) {
        this->mpMainGridBox->CloneMetaData(this->mpInternalGridBox);
        this->mpMainGridBox->CloneParameters(this->mpInternalGridBox);

        if (this->mpInternalGridBox->GetWaveFields().empty()) {
            this->mpWaveFieldsMemoryHandler->CloneWaveFields(this->mpMainGridBox,
                                                             this->mpInternalGridBox);

            // save the pressure pointer for deletion afterwards
            initial_internalGridbox_curr = mpInternalGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();

        } else {
            this->mpWaveFieldsMemoryHandler->CopyWaveFields(this->mpMainGridBox,
                                                            this->mpInternalGridBox);
        }


        this->mTimeCounter = 0;
        if (this->mpForwardPressureHostMemory == nullptr) {
            /// Add one for empty timeframe at the start of the simulation
            /// (The first previous) since SaveForward is called before each step.
            this->mMaxNT = this->mpMainGridBox->GetNT() + 1;


            this->mMaxDeviceNT = 100; // save 100 frames in the Device memory, then reflect to host memory

            this->mpForwardPressureHostMemory = (float *) mem_allocate(
                    (sizeof(float)), this->mMaxNT * window_size, "forward_pressure");

            this->mpForwardPressure = new FrameBuffer<float>();
            this->mpForwardPressure->Allocate(window_size * this->mMaxDeviceNT);

            if (this->mpForwardPressureHostMemory != nullptr) {
                this->mIsMemoryFit = true;
            } else {
                this->mIsMemoryFit = false;
                while (this->mpForwardPressureHostMemory == nullptr) {
                    this->mMaxNT = this->mMaxNT / 2;
                    this->mpForwardPressureHostMemory = (float *) mem_allocate(
                            (sizeof(float)), this->mMaxNT * window_size, "forward_pressure");
                }

                mem_free(this->mpForwardPressureHostMemory);

                // another iteration as a safety measure
                this->mMaxNT = this->mMaxNT / 2;
                this->mpForwardPressureHostMemory = (float *) mem_allocate(
                        (sizeof(float)), this->mMaxNT * window_size, "forward_pressure");
            }

            mpMaxNTRatio = mMaxNT / this->mMaxDeviceNT;

        }

        this->mpTempCurr = this->mpMainGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer();
        this->mpTempNext = this->mpMainGridBox->Get(WAVE | GB_PRSS | NEXT | DIR_Z)->GetNativePointer();

        Device::MemSet(this->mpForwardPressure->GetNativePointer(), 0.0f,
                       window_size * sizeof(float));
        Device::MemSet(this->mpForwardPressure->GetNativePointer() + window_size, 0.0f,
                       window_size * sizeof(float));


        if (this->mpParameters->GetEquationOrder() == SECOND) {
            this->mpTempPrev = this->mpMainGridBox->Get(WAVE | GB_PRSS | PREV | DIR_Z)->GetNativePointer();

            Device::MemSet(this->mpForwardPressure->GetNativePointer() + 2 * window_size, 0.0f,
                           window_size * sizeof(float));
        }


        if (this->mpParameters->GetEquationOrder() == SECOND) {
            this->mpMainGridBox->Set(WAVE | GB_PRSS | PREV | DIR_Z, this->mpForwardPressure->GetNativePointer());
            this->mpMainGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z,
                                     this->mpForwardPressure->GetNativePointer() + window_size);
            // Save forward is called before the kernel in the engine.
            // When called will advance pressure next to the right point.
            this->mpMainGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z,
                                     this->mpForwardPressure->GetNativePointer() + window_size);
        } else {
            this->mpMainGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z, this->mpForwardPressure->GetNativePointer());
            this->mpMainGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z,
                                     this->mpForwardPressure->GetNativePointer() + window_size);
        }
    } else {
        Device::MemSet(this->mpTempCurr, 0.0f, window_size * sizeof(float));
        if (this->mpParameters->GetEquationOrder() == SECOND) {
            Device::MemSet(this->mpTempPrev, 0.0f, window_size * sizeof(float));
        }

        for (auto const &wave_field : this->mpMainGridBox->GetWaveFields()) {
            if (GridBox::Includes(wave_field.first, GB_PRTC)) {
                Device::MemSet(wave_field.second->GetNativePointer(), 0.0f, window_size * sizeof(float));
            }
        }

        if (!this->mIsMemoryFit) {
            this->mTimeCounter++;
            this->mpInternalGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z,
                                         this->mpMainGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer());
        } else {
            // Pressure size will be minimized in FetchForward call at first step.
            this->mpInternalGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z,
                                         this->mpMainGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetNativePointer() +
                                         window_size);
        }

        if (this->mpParameters->GetEquationOrder() == SECOND) {
            this->mpMainGridBox->Set(WAVE | GB_PRSS | PREV | DIR_Z, this->mpTempPrev);
        }
        this->mpMainGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z, this->mpTempCurr);
        this->mpMainGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z, this->mpTempNext);
    }
}

void TwoPropagation::SaveForward() {


    uint wnx = this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;

    this->mTimeCounter++;

    // Transfer from Device memory to host memory
    if ((this->mTimeCounter + 1) % this->mMaxDeviceNT == 0) {

        int host_index = (this->mTimeCounter + 1) / this->mMaxDeviceNT - 1;

        Device::MemCpy(
                this->mpForwardPressureHostMemory +
                (host_index % this->mpMaxNTRatio) * (this->mMaxDeviceNT * window_size),
                this->mpForwardPressure->GetNativePointer(),
                this->mMaxDeviceNT * window_size * sizeof(float),
                Device::COPY_DEVICE_TO_HOST);
    }

    // Save host memory to file
    if ((this->mTimeCounter + 1) % this->mMaxNT == 0) {
        if (this->mIsCompression) {
            string str = this->mWritePath + "/temp_" + to_string(this->mTimeCounter / this->mMaxNT);
            {
                ScopeTimer t("ForwardCollector::Compression");;
                Compressor::Compress(this->mpForwardPressureHostMemory, wnx, wny, wnz,
                                     this->mMaxNT,
                                     (double) this->mZFP_Tolerance,
                                     this->mZFP_Parallel,
                                     str.c_str(),
                                     this->mZFP_IsRelative);
            }
        } else {
            string str =
                    this->mWritePath + "/temp_" + to_string(this->mTimeCounter / this->mMaxNT);
            {
                ScopeTimer t("IO::WriteForward");
                bin_file_save(str.c_str(), this->mpForwardPressureHostMemory, this->mMaxNT * window_size);
            }
        }
    }

    this->mpMainGridBox->Set(WAVE | GB_PRSS | CURR | DIR_Z,
                             this->mpForwardPressure->GetNativePointer() +
                             ((this->mTimeCounter) % this->mMaxDeviceNT) * window_size);
    this->mpMainGridBox->Set(WAVE | GB_PRSS | NEXT | DIR_Z,
                             this->mpForwardPressure->GetNativePointer() +
                             ((this->mTimeCounter + 1) % this->mMaxDeviceNT) * window_size);
    if (this->mpParameters->GetEquationOrder() == SECOND) {
        this->mpMainGridBox->Set(WAVE | GB_PRSS | PREV | DIR_Z,
                                 this->mpForwardPressure->GetNativePointer() +
                                 ((this->mTimeCounter - 1) % this->mMaxDeviceNT) * window_size);
    }
}

void TwoPropagation::SetComputationParameters(ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

void TwoPropagation::SetGridBox(GridBox *apGridBox) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpMainGridBox = apGridBox;
    if (this->mpMainGridBox == nullptr) {
        Logger->Error() << "Not a compatible GridBox... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    /*
     * in case of two propagation the next buffer and previous buffer get separated
     * so we need to register a new wave field with actual allocated memory
     */
    auto framebuffer = new FrameBuffer<float>();
    this->mpMainGridBox->RegisterWaveField(WAVE | GB_PRSS | NEXT | DIR_Z, framebuffer);


    framebuffer->Allocate(
            this->mpMainGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize() *
            this->mpMainGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize() *
            this->mpMainGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize(),
            mpParameters->GetHalfLength(),
            "next pressure");
}

void TwoPropagation::SetDependentComponents(
        ComponentsMap<DependentComponent> *apDependentComponentsMap) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    HasDependents::SetDependentComponents(apDependentComponentsMap);

    this->mpWaveFieldsMemoryHandler =
            (WaveFieldsMemoryHandler *)
                    this->GetDependentComponentsMap()->Get(MEMORY_HANDLER);
    if (this->mpWaveFieldsMemoryHandler == nullptr) {
        Logger->Error() << "No Wave Fields Memory Handler provided... " << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}

GridBox *TwoPropagation::GetForwardGrid() {
    return this->mpInternalGridBox;
}
