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

#include <cmath>
#include <sys/stat.h>

#include <operations/helpers/callbacks/concrete/NormWriter.h>
#include <operations/helpers/callbacks/interface/Extensions.hpp>


#define CAT_STR(a, b) (a + b)

using namespace std;
using namespace operations::helpers::callbacks;
using namespace operations::dataunits;
using namespace operations::common;


NormWriter::NormWriter(uint aShowEach,
                       bool aWriteForward,
                       bool aWriteBackward,
                       bool aWriteReverse,
                       const string &aWritePath) {
    this->show_each = aShowEach;
    this->write_forward = aWriteForward;
    this->write_backward = aWriteBackward;
    this->write_reverse = aWriteReverse;
    this->write_path = aWritePath;
    mkdir(aWritePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    this->write_path = this->write_path + "/norm";
    mkdir(this->write_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (this->write_forward) {
        forward_norm_stream = new ofstream(this->write_path + "/forward_norm" + this->GetExtension());
    }
    if (this->write_reverse) {
        reverse_norm_stream = new ofstream(this->write_path + "/reverse_norm" + this->GetExtension());
    }
    if (this->write_backward) {
        backward_norm_stream =
                new ofstream(this->write_path + "/backward_norm" + this->GetExtension());
    }
}

NormWriter::~NormWriter() {
    if (this->write_forward) {
        delete forward_norm_stream;
    }
    if (this->write_reverse) {
        delete reverse_norm_stream;
    }
    if (this->write_backward) {
        delete backward_norm_stream;
    }
}

void
NormWriter::BeforeInitialization(ComputationParameters *apParameters) {
    this->offset = apParameters->GetBoundaryLength() + apParameters->GetHalfLength();
}

void
NormWriter::AfterInitialization(GridBox *apGridBox) {}

void
NormWriter::BeforeShotPreprocessing(TracesHolder *apTraces) {}

void
NormWriter::AfterShotPreprocessing(TracesHolder *apTraces) {}

void
NormWriter::BeforeForwardPropagation(GridBox *apGridBox) {}

void
NormWriter::AfterForwardStep(GridBox *apGridBox, int aTimeStep) {
    if (write_forward && aTimeStep % show_each == 0) {

        uint nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();


        float norm = NormWriter::Solve(apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer(), nx, nz, ny);
        (*this->forward_norm_stream) << aTimeStep << "\t" << norm << endl;
    }
}

void
NormWriter::BeforeBackwardPropagation(GridBox *apGridBox) {}

void
NormWriter::AfterBackwardStep(GridBox *apGridBox, int aTimeStep) {
    if (write_backward && aTimeStep % show_each == 0) {

        uint nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        float norm = NormWriter::Solve(apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer(), nx, nz, ny);
        (*this->backward_norm_stream) << aTimeStep << "\t" << norm << endl;
    }
}

void
NormWriter::AfterFetchStep(GridBox *apGridBox,
                           int aTimeStep) {
    if (write_reverse && aTimeStep % show_each == 0) {

        uint nx = apGridBox->GetWindowAxis()->GetXAxis().GetActualAxisSize();
        uint ny = apGridBox->GetWindowAxis()->GetYAxis().GetActualAxisSize();
        uint nz = apGridBox->GetWindowAxis()->GetZAxis().GetActualAxisSize();

        float norm = NormWriter::Solve(apGridBox->Get(WAVE | GB_PRSS | CURR | DIR_Z)->GetHostPointer(), nx, nz, ny);
        (*this->reverse_norm_stream) << aTimeStep << "\t" << norm << endl;
    }
}

void
NormWriter::BeforeShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *apShotCorrelation) {}

void
NormWriter::AfterShotStacking(
        GridBox *apGridBox, FrameBuffer<float> *apStackedShotCorrelation) {}

void
NormWriter::AfterMigration(
        GridBox *apGridBox, FrameBuffer<float> *apStackedShotCorrelation) {}


float NormWriter::Solve(const float *apMatrix, uint nx, uint nz, uint ny) {
    float sum = 0;
    uint nx_nz = nx * nz;

    int end_x = nx - this->offset;
    int end_z = nz - this->offset;
    int start_y = 0;
    int end_y = 1;

    if(ny > 1){
        start_y = this->offset;
        end_y = ny - this->offset;
    }

    for (int iy = start_y; iy < end_y; iy++) {
        for (int iz = this->offset; iz < end_z; iz++) {
            for (int ix = this->offset; ix < end_x; ix++) {
                auto value = apMatrix[iy * nx_nz + nx * iz + ix];
                sum += (value * value);
            }
        }
    }
    return sqrtf(sum);
}

std::string NormWriter::GetExtension() {
    return OP_K_EXT_NRM;
}
