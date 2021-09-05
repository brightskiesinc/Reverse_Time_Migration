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

#include <operations/helpers/callbacks/concrete/WriterCallback.h>
#include <operations/common/DataTypes.h>
#include <operations/utils/io/write_utils.h>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <map>
#include <vector>
#include <sys/stat.h>

#define CAT_STR_TO_CHR(a, b) ((char *)string(a + b).c_str())

#define SPACE_SAMPLE_SCALE  1e3
#define TIME_SAMPLE_SCALE   1e6

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::utils::io;
using namespace bs::base::logger;
using namespace bs::base::configurations;
using namespace bs::io::streams;


/// Helper functions to be relocated/replaced
/// after further implementations
/// {
bool is_path_exist(const std::string &s) {
    struct stat buffer;
    return (stat(s.c_str(), &buffer) == 0);
}

u_int16_t get_callbacks_map(const string &key) {
    /// Initialized upon first call to the function.
    static std::map<std::string, u_int16_t> callbacks_params_map = {
            {"velocity", PARM | GB_VEL},
            {"delta",    PARM | GB_DLT},
            {"epsilon",  PARM | GB_EPS},
            {"theta",    PARM | GB_THT},
            {"phi",      PARM | GB_PHI},
            {"density",  PARM | GB_DEN}
    };
    return callbacks_params_map.at(key);
}

std::map<std::string, u_int16_t> get_params_map() {
    /// Initialized upon first call to the function.
    static std::map<std::string, u_int16_t> callbacks_params_map = {
            {"velocity", PARM | GB_VEL},
            {"delta",    PARM | GB_DLT},
            {"epsilon",  PARM | GB_EPS},
            {"theta",    PARM | GB_THT},
            {"phi",      PARM | GB_PHI},
            {"density",  PARM | GB_DEN}
    };
    return callbacks_params_map;
}

std::map<std::string, u_int16_t> get_pressure_map() {
    // Initialized upon the first call to the function
    static std::map<std::string, u_int16_t> callbacks_pressure_map = {
            {"vertical",   WAVE | GB_PRSS | CURR | DIR_Z},
            {"horizontal", WAVE | GB_PRSS | CURR | DIR_X}
    };
    return callbacks_pressure_map;
}
/// }
/// End of helper functions


float *Unpad(const float *apOriginalArray, uint nx, uint ny, uint nz,
             uint nx_original, uint ny_original, uint nz_original) {
    if (nx == nx_original && nz == nz_original && ny == ny_original) {
        return nullptr;
    } else {
        auto copy_array = new float[ny_original * nz_original * nx_original];
        for (uint iy = 0; iy < ny_original; iy++) {
            for (uint iz = 0; iz < nz_original; iz++) {
                for (uint ix = 0; ix < nx_original; ix++) {
                    copy_array[iy * nz_original * nx_original + iz * nx_original + ix] =
                            apOriginalArray[iy * nz * nx + iz * nx + ix];
                }
            }
        }
        return copy_array;
    }
}

WriterCallback::WriterCallback(uint aShowEach,
                               bool aWriteParams,
                               bool aWriteForward,
                               bool aWriteBackward,
                               bool aWriteReverse,
                               bool aWriteMigration,
                               bool aWriteReExtendedParams,
                               bool aWriteSingleShotCorrelation,
                               bool aWriteEachStackedShot, bool aWriteTracesRaw,
                               bool aWriteTracesPreprocessed,
                               const std::vector<std::string> &aVecParams,
                               const std::vector<std::string> &aVecReExtendedParams,
                               const string &aWritePath,
                               std::vector<std::string> &aTypes,
                               std::vector<std::string> &aUnderlyingConfigurations) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mShowEach = aShowEach;
    this->mShotCount = 0;
    this->mIsWriteParams = aWriteParams;
    this->mIsWriteForward = aWriteForward;
    this->mIsWriteBackward = aWriteBackward;
    this->mIsWriteReverse = aWriteReverse;
    this->mIsWriteMigration = aWriteMigration;
    this->mIsWriteReExtendedParams = aWriteReExtendedParams;
    this->mIsWriteSingleShotCorrelation = aWriteSingleShotCorrelation;
    this->mIsWriteEachStackedShot = aWriteEachStackedShot;
    this->mIsWriteTracesRaw = aWriteTracesRaw;
    this->mIsWriteTracesPreprocessed = aWriteTracesPreprocessed;
    this->mParamsVec = aVecParams;
    this->mReExtendedParamsVec = aVecReExtendedParams;

    if (this->mParamsVec.empty()) {
        for (auto const &pair : get_params_map()) {
            this->mParamsVec.push_back(pair.first);
        }
    }
    if (this->mReExtendedParamsVec.empty()) {
        for (auto const &pair: get_params_map()) {
            this->mReExtendedParamsVec.push_back(pair.first);
        }
    }

    this->mWritePath = aWritePath;
    this->mWriterTypes = aTypes;
    mkdir(aWritePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    Logger->Info() << "Writer callback aTypes to be written : " << '\n';
    for (int i = 0; i < aTypes.size(); i++) {
        auto type = aTypes[i];
        try {
            SeismicWriter::ToWriterType(type);
        } catch (exception &e) {
            Logger->Error() << "Invalid type provided to writer callback : " << e.what() << '\n';
            Logger->Error() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
        Logger->Info() << "\tType #" << (i + 1) << " : " << type << '\n';
        nlohmann::json configuration = nlohmann::json::parse(aUnderlyingConfigurations[i]);
        JSONConfigurationMap io_conf_map(configuration);
        this->mWriters.push_back(new SeismicWriter(SeismicWriter::ToWriterType(type),
                                                   &io_conf_map));
        this->mWriters.back()->AcquireConfiguration();
        std::string mod_path = this->mWritePath + "/output_" + type;
        mkdir(mod_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        if (aWriteReExtendedParams) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/parameters"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteSingleShotCorrelation) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/shots"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteEachStackedShot) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/stacked_shots"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteForward) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/forward"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteReverse) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/reverse"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteBackward) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/backward"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteTracesRaw) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/traces_raw"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        if (aWriteTracesPreprocessed) {
            mkdir(CAT_STR_TO_CHR(mod_path, "/traces"),
                  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
    }
}

WriterCallback::~WriterCallback() {
    for (auto writer : this->mWriters) {
        delete writer;
    }
}

void
WriterCallback::WriteResult(uint nx, uint ny, uint ns, float dx, float dy, float ds, const float *data,
                            const std::string &filename, float sample_scale) {
    auto gathers = TransformToGather(data, nx, ny, ns, dx, dy, ds,
                                     0, 0, 0, 0,
                                     1, 1e3, sample_scale);
    for (int i_writer = 0; i_writer < this->mWriterTypes.size(); i_writer++) {
        std::string path = this->mWritePath + "/output_" + this->mWriterTypes[i_writer] + filename;
        this->mWriters[i_writer]->Initialize(path);
        this->mWriters[i_writer]->Write(gathers);
        this->mWriters[i_writer]->Finalize();
    }
    for (auto g : gathers) {
        delete g;
    }
}

void
WriterCallback::BeforeInitialization(ComputationParameters *apParameters) {}

void
WriterCallback::AfterInitialization(GridBox *apGridBox) {
    if (this->mIsWriteParams) {

        uint pnx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        uint pny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
        uint pnz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

        uint nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
        uint ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
        uint nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &param : this->mParamsVec) {
            if (apGridBox->Has(get_callbacks_map(param))) {
                float *arr = apGridBox->Get(get_callbacks_map(param))->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pnx, pny, pnz, nx, ny, nz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                uint nt = apGridBox->GetNT();

                WriteResult(nx, ny, nz, dx, dy, dz, arr,
                            "/" + param, SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}

void
WriterCallback::BeforeShotPreprocessing(TracesHolder *traces) {
    if (this->mIsWriteTracesRaw) {
        uint nx = traces->TraceSizePerTimeStep;
        uint ny = 1;

        uint nt = traces->SampleNT;
        float dt = traces->SampleDT;

        float dx = 1.0;
        float dy = 1.0;

        WriteResult(nx, ny, nt, dx, dy, dt, traces->Traces->GetHostPointer(),
                    "/traces_raw/trace_" +
                    to_string(this->mShotCount), TIME_SAMPLE_SCALE);
    }
}

void
WriterCallback::AfterShotPreprocessing(TracesHolder *traces) {
    if (this->mIsWriteTracesPreprocessed) {
        uint nx = traces->TraceSizePerTimeStep;
        uint ny = 1;

        uint nt = traces->SampleNT;
        float dt = traces->SampleDT;

        float dx = 1.0;
        float dy = 1.0;

        WriteResult(nx, ny, nt, dx, dy, dt, traces->Traces->GetHostPointer(),
                    "/traces/trace_" +
                    to_string(this->mShotCount), TIME_SAMPLE_SCALE);
    }
}

void
WriterCallback::BeforeForwardPropagation(GridBox *apGridBox) {
    if (this->mIsWriteReExtendedParams) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &param : this->mReExtendedParamsVec) {
            if (apGridBox->Has(get_callbacks_map(param))) {
                float *arr = apGridBox->Get(get_callbacks_map(param) | WIND)->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                            "/parameters/" + param + "_" +
                            to_string(this->mShotCount), SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}


void
WriterCallback::AfterForwardStep(GridBox *apGridBox, int time_step) {
    if (mIsWriteForward && time_step % mShowEach == 0) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &pressure : get_pressure_map()) {
            if (apGridBox->Has(pressure.second)) {
                for (auto &type : this->mWriterTypes) {
                    if (!is_path_exist(string(this->mWritePath + "/output_"
                                              + type + "/forward/" + pressure.first)))
                        mkdir(CAT_STR_TO_CHR(this->mWritePath + "/output_"
                                             + type + "/forward/", pressure.first),
                              S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }

                float *arr = apGridBox->Get(pressure.second)->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                            "/forward/" + pressure.first
                            + "/forward_" + pressure.first + "_" +
                            to_string(time_step), SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}

void
WriterCallback::BeforeBackwardPropagation(GridBox *apGridBox) {
    if (mIsWriteReExtendedParams) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &param : this->mReExtendedParamsVec) {
            if (apGridBox->Has(get_callbacks_map(param))) {
                float *arr = apGridBox->Get(get_callbacks_map(param) | WIND)->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                            "/parameters/" + param + "_backward_" +
                            to_string(mShotCount), SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}

void
WriterCallback::AfterBackwardStep(
        GridBox *apGridBox, int time_step) {
    if (mIsWriteBackward && time_step % mShowEach == 0) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &pressure : get_pressure_map()) {
            if (apGridBox->Has(pressure.second)) {
                for (auto &type : this->mWriterTypes) {
                    if (!is_path_exist(string(this->mWritePath + "/output_"
                                              + type + "/backward/" + pressure.first)))
                        mkdir(CAT_STR_TO_CHR(this->mWritePath + "/output_"
                                             + type + "/backward/", pressure.first),
                              S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }

                float *arr = apGridBox->Get(pressure.second)->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                            "/backward/" + pressure.first
                            + "/backward_" + pressure.first + "_" +
                            to_string(time_step), SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}

void
WriterCallback::AfterFetchStep(
        GridBox *apGridBox, int time_step) {
    if (mIsWriteReverse && time_step % mShowEach == 0) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        for (const auto &pressure : get_pressure_map()) {
            if (apGridBox->Has(pressure.second)) {
                for (auto &type : this->mWriterTypes) {
                    if (!is_path_exist(string(this->mWritePath + "/output_"
                                              + type + "/reverse/" + pressure.first)))
                        mkdir(CAT_STR_TO_CHR(this->mWritePath + "/output_"
                                             + type + "/reverse/", pressure.first),
                              S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }


                float *arr = apGridBox->Get(pressure.second)->GetHostPointer();
                float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
                if (unpadded_arr) {
                    arr = unpadded_arr;
                }

                WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                            "/reverse/" + pressure.first
                            + "/reverse_" + pressure.first + "_" +
                            to_string(time_step), SPACE_SAMPLE_SCALE);
                delete unpadded_arr;
            }
        }
    }
}

void
WriterCallback::BeforeShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *shot_correlation) {
    if (mIsWriteSingleShotCorrelation) {

        uint pwnx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint pwny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint pwnz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        uint wnx = apGridBox->GetWindowAxis()->GetXAxis().GetLogicalAxisSize();
        uint wny = apGridBox->GetWindowAxis()->GetYAxis().GetLogicalAxisSize();
        uint wnz = apGridBox->GetWindowAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();

        float *arr = shot_correlation->GetHostPointer();
        float *unpadded_arr = Unpad(arr, pwnx, pwny, pwnz, wnx, wny, wnz);
        if (unpadded_arr) {
            arr = unpadded_arr;
        }

        WriteResult(wnx, wny, wnz, dx, dy, dz, arr,
                    "/shots/correlation_" +
                    to_string(mShotCount), SPACE_SAMPLE_SCALE);
        delete unpadded_arr;
    }
}

void
WriterCallback::AfterShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *stacked_shot_correlation) {
    if (mIsWriteEachStackedShot) {

        uint pnx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        uint pny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
        uint pnz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

        uint nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
        uint ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
        uint nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        float *arr = stacked_shot_correlation->GetHostPointer();
        float *unpadded_arr = Unpad(arr, pnx, pny, pnz, nx, ny, nz);
        if (unpadded_arr) {
            arr = unpadded_arr;
        }

        float dt = apGridBox->GetDT();

        WriteResult(nx, ny, nz, dx, dy, dz, arr,
                    "/stacked_shots/stacked_correlation_" +
                    to_string(mShotCount), SPACE_SAMPLE_SCALE);
        delete unpadded_arr;
    }

    this->mShotCount++;
}

void
WriterCallback::AfterMigration(
        GridBox *apGridBox, FrameBuffer<float> *stacked_shot_correlation) {
    if (mIsWriteMigration) {

        uint pnx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize();
        uint pny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize();
        uint pnz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize();

        uint nx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize();
        uint ny = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize();
        uint nz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize();

        float dx = apGridBox->GetAfterSamplingAxis()->GetXAxis().GetCellDimension();
        float dy = apGridBox->GetAfterSamplingAxis()->GetYAxis().GetCellDimension();
        float dz = apGridBox->GetAfterSamplingAxis()->GetZAxis().GetCellDimension();


        float *arr = stacked_shot_correlation->GetHostPointer();
        float *unpadded_arr = Unpad(arr, pnx, pny, pnz, nx, ny, nz);
        if (unpadded_arr) {
            arr = unpadded_arr;
        }


        WriteResult(nx, ny, nz, dx, dy, dz, arr,
                    "/migration", SPACE_SAMPLE_SCALE);
        delete unpadded_arr;
    }
}
