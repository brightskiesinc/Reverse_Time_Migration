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

#include <bs/base/api/cpp/BSBase.hpp>

#include <operations/data-units/concrete/holders/GridBox.hpp>

using namespace bs::base::exceptions;
using namespace bs::base::logger;
using namespace operations::dataunits;

GridBox::GridBox() {

    this->mpParameterHeadersGather = nullptr;
    this->mpWindowProperties = new WindowProperties();
    this->mpInitialAxis = nullptr;
    this->mpAfterSamplingAxis = nullptr;
    this->mpWindowAxis = nullptr;


}

GridBox::~GridBox() {

    delete this->mpWindowProperties;
    delete this->mpInitialAxis;
    delete this->mpAfterSamplingAxis;
    delete this->mpWindowAxis;
};

void GridBox::SetDT(float _dt) {
    if (_dt <= 0) {
        throw ILLOGICAL_EXCEPTION();
    }
    this->mDT = _dt;
}

void GridBox::SetNT(float _nt) {

    if (_nt <= 0) {
        throw bs::base::exceptions::ILLOGICAL_EXCEPTION();
    }
    this->mNT = _nt;
}


/**
 * @brief Window start per axe setter.
 * @param[in] axis      Axe direction
 * @param[in] val       Value to be set
 * @throw ILLOGICAL_EXCEPTION()
 */
void GridBox::SetWindowStart(uint axis, uint val) {

    if (is_out_of_range(axis)) {
        throw AXIS_EXCEPTION();
    }

    if (axis == Y_AXIS) {
        this->mpWindowProperties->window_start.y = val;
    } else if (axis == Z_AXIS) {
        this->mpWindowProperties->window_start.z = val;
    } else if (axis == X_AXIS) {
        this->mpWindowProperties->window_start.x = val;
    }
}

uint GridBox::GetWindowStart(uint axis) {

    if (is_out_of_range(axis)) {
        throw AXIS_EXCEPTION();
    }

    uint val;
    if (axis == Y_AXIS) {
        val = this->mpWindowProperties->window_start.y;
    } else if (axis == Z_AXIS) {
        val = this->mpWindowProperties->window_start.z;
    } else if (axis == X_AXIS) {
        val = this->mpWindowProperties->window_start.x;
    }
    return val;
}


void GridBox::RegisterWaveField(u_int16_t key, FrameBuffer<float> *ptr_wave_field) {

    key |= WAVE;
    this->mWaveFields[key] = ptr_wave_field;
}

void GridBox::RegisterParameter(u_int16_t key,
                                FrameBuffer<float> *ptr_parameter,
                                FrameBuffer<float> *ptr_parameter_window) {

    key |= PARM;
    this->mParameters[key] = ptr_parameter;

    key |= WIND;
    if (ptr_parameter_window == nullptr) {
        this->mWindowParameters[key] = ptr_parameter;
    } else {
        this->mWindowParameters[key] = ptr_parameter_window;
    }
}

void GridBox::RegisterMasterWaveField() {
    /// @todo To be implemented.
    throw NOT_IMPLEMENTED_EXCEPTION();
}

float *GridBox::GetMasterWaveField() {
    /// @todo To be implemented.
    throw NOT_IMPLEMENTED_EXCEPTION();
}

FrameBuffer<float> *GridBox::Get(u_int16_t key) {

    if (this->mWaveFields.find(key) != this->mWaveFields.end()) {
        return this->mWaveFields[key];
    } else if (this->mParameters.find(key) != this->mParameters.end()) {
        return this->mParameters[key];
    } else if (this->mWindowParameters.find(key) != this->mWindowParameters.end()) {
        return this->mWindowParameters[key];
    }
    throw NO_KEY_FOUND_EXCEPTION();
}

void GridBox::Set(u_int16_t key, FrameBuffer<float> *val) {

    if (this->mWaveFields.find(key) != this->mWaveFields.end()) {
        this->mWaveFields[key] = val;
    } else if (this->mParameters.find(key) != this->mParameters.end()) {
        this->mParameters[key] = val;
    } else if (this->mWindowParameters.find(key) != this->mWindowParameters.end()) {
        this->mWindowParameters[key] = val;
    } else {
        throw NO_KEY_FOUND_EXCEPTION();
    }
}

void GridBox::Set(u_int16_t key, float *val) {

    if (this->mWaveFields.find(key) != this->mWaveFields.end()) {
        this->mWaveFields[key]->SetNativePointer(val);
    } else if (this->mParameters.find(key) != this->mParameters.end()) {
        this->mParameters[key]->SetNativePointer(val);
    } else if (this->mWindowParameters.find(key) != this->mWindowParameters.end()) {
        this->mWindowParameters[key]->SetNativePointer(val);
    } else {
        throw NO_KEY_FOUND_EXCEPTION();
    }
}

void GridBox::Reset(u_int16_t key) {
    this->Set(key, (FrameBuffer<float> *)
            nullptr);
}

void GridBox::Swap(u_int16_t _src, u_int16_t _dst) {
    if (this->mWaveFields.find(_src) == this->mWaveFields.end() ||
        this->mWaveFields.find(_dst) == this->mWaveFields.end()) {
        throw NO_KEY_FOUND_EXCEPTION();
    }

    FrameBuffer<float> *src, *dest;
    if (this->mWaveFields.find(_src) != this->mWaveFields.end() &&
        this->mWaveFields.find(_dst) != this->mWaveFields.end()) {
        src = this->mWaveFields[_src];
        dest = this->mWaveFields[_dst];
        this->mWaveFields[_dst] = src;
        this->mWaveFields[_src] = dest;
    }
}

void GridBox::Clone(GridBox *apGridBox) {

    this->CloneMetaData(apGridBox);
    this->CloneWaveFields(apGridBox);
    this->CloneParameters(apGridBox);
}

void GridBox::CloneMetaData(GridBox *apGridBox) {

    apGridBox->SetNT(this->GetNT());
    apGridBox->SetDT(this->GetDT());
    apGridBox->SetInitialAxis(new Axis3D<unsigned int>(*this->GetInitialAxis()));
    apGridBox->SetAfterSamplingAxis(new Axis3D<unsigned int>(*this->GetAfterSamplingAxis()));
    apGridBox->SetWindowAxis(new Axis3D<unsigned int>(*this->GetWindowAxis()));

}


void GridBox::CloneWaveFields(GridBox *apGridBox) {

    for (auto const &parameter : this->GetParameters()) {
        apGridBox->RegisterWaveField(parameter.first,
                                     parameter.second);
    }
}


void GridBox::CloneParameters(GridBox *apGridBox) {

    for (auto const &parameter : this->GetParameters()) {
        apGridBox->RegisterParameter(parameter.first,
                                     this->Get(parameter.first),
                                     this->Get(WIND | parameter.first));
    }
}


void GridBox::Report(REPORT_LEVEL aReportLevel) {

    LoggerSystem *Logger = LoggerSystem::GetInstance();

    Logger->Info() << "GridBox Report " << '\n';
    Logger->Info() << "==============================" << '\n';

    uint index = 0;


    Logger->Info() << "Actual Grid Size: " << '\n';
    Logger->Info() << "- nx\t: " << this->GetAfterSamplingAxis()->GetXAxis().GetActualAxisSize() << '\n';
    Logger->Info() << "- ny\t: " << this->GetAfterSamplingAxis()->GetYAxis().GetActualAxisSize() << '\n';
    Logger->Info() << "- nz\t: " << this->GetAfterSamplingAxis()->GetZAxis().GetActualAxisSize() << '\n';
    Logger->Info() << "- nt\t: " << GetNT() << '\n';


    Logger->Info() << "Logical Grid Size: " << '\n';
    Logger->Info() << "- nx\t: " << this->GetAfterSamplingAxis()->GetXAxis().GetLogicalAxisSize() << '\n';
    Logger->Info() << "- ny\t: " << this->GetAfterSamplingAxis()->GetYAxis().GetLogicalAxisSize() << '\n';
    Logger->Info() << "- nz\t: " << this->GetAfterSamplingAxis()->GetZAxis().GetLogicalAxisSize() << '\n';

    Logger->Info() << "Actual Window Size: " << '\n';
    Logger->Info() << "- wnx\t: " << this->GetWindowAxis()->GetXAxis().GetActualAxisSize() << '\n';
    Logger->Info() << "- wny\t: " << this->GetWindowAxis()->GetYAxis().GetActualAxisSize() << '\n';
    Logger->Info() << "- wnz\t: " << this->GetWindowAxis()->GetZAxis().GetActualAxisSize() << '\n';


    Logger->Info() << "Logical Window Size: " << '\n';
    Logger->Info() << "- wnx\t: " << this->GetWindowAxis()->GetXAxis().GetLogicalAxisSize() << '\n';
    Logger->Info() << "- wny\t: " << this->GetWindowAxis()->GetYAxis().GetLogicalAxisSize() << '\n';
    Logger->Info() << "- wnz\t: " << this->GetWindowAxis()->GetZAxis().GetLogicalAxisSize() << '\n';

    Logger->Info() << "Computation Grid Size: " << '\n';
    Logger->Info() << "- x elements\t: " << this->GetWindowAxis()->GetXAxis().GetComputationAxisSize() << '\n';
    Logger->Info() << "- y elements\t: " << this->GetWindowAxis()->GetYAxis().GetComputationAxisSize() << '\n';
    Logger->Info() << "- z elements\t: " << this->GetWindowAxis()->GetZAxis().GetComputationAxisSize() << '\n';


    Logger->Info() << "Cell Dimensions: " << '\n';
    Logger->Info() << "- dx\t: " << this->GetAfterSamplingAxis()->GetXAxis().GetCellDimension() << '\n';
    Logger->Info() << "- dy\t: " << this->GetAfterSamplingAxis()->GetYAxis().GetCellDimension() << '\n';
    Logger->Info() << "- dz\t: " << this->GetAfterSamplingAxis()->GetZAxis().GetCellDimension() << '\n';
    Logger->Info() << "- dt\t: " << GetDT() << '\n';

    Logger->Info() << "Wave Fields: " << '\n';
    Logger->Info() << "- Count\t: " << GetWaveFields().size() << '\n';
    Logger->Info() << "- Names\t: " << '\n';
    index = 0;
    for (auto const &wave_field : GetWaveFields()) {
        Logger->Info() << "\t" << ++index << ". "
                       << Beautify(Stringify(wave_field.first))
                       << '\n';
    }

    Logger->Info() << "Parameters: " << '\n';
    Logger->Info() << "- Count\t: " << GetParameters().size() << '\n';
    Logger->Info() << "- Names\t: " << '\n';
    index = 0;
    for (auto const &parameter : GetParameters()) {
        Logger->Info() << "\t" << ++index << ". "
                       << Beautify(Stringify(parameter.first)) << '\n';
    }
}

bool GridBox::Has(u_int16_t key) {
    if (this->mWaveFields.find(key) != this->mWaveFields.end()) {
        return true;
    } else if (this->mParameters.find(key) != this->mParameters.end()) {
        return true;
    } else if (this->mWindowParameters.find(key) != this->mWindowParameters.end()) {
        return true;
    }
    return false;
}

void GridBox::Replace(u_int16_t *key, u_int16_t _src, u_int16_t _dest) {
    *key = ~_src & *key;
    SetBits(key, _dest);
}

/**
 * @brief Converts a given key (i.e. u_int16_t) to string.
 * @param[in] key
 * @return[out] String value
 */
std::string GridBox::Stringify(u_int16_t key) {
    std::string str = "";

    /// Window
    if (Includes(key, WIND)) {
        str += "window_";
    }

    /// Wave Field / Parameter
    if (Includes(key, WAVE)) {
        str += "wave_";

        /// Pressure / Particle
        if (Includes(key, GB_PRSS)) {
            str += "pressure_";
        } else if (Includes(key, GB_PRTC)) {
            str += "particle_";
        }
    } else if (Includes(key, PARM)) {
        str += "parameter_";

        /// Parameter
        if (Includes(key, GB_DEN)) {
            str += "density_";
        } else if (Includes(key, GB_PHI)) {
            str += "phi_";
        } else if (Includes(key, GB_THT)) {
            str += "theta_";
        } else if (Includes(key, GB_EPS)) {
            str += "epsilon_";
        } else if (Includes(key, GB_DLT)) {
            str += "delta_";
        } else if (Includes(key, GB_VEL)) {
            str += "velocity_";
        } else if (Includes(key, GB_VEL_RMS)) {
            str += "velocity_rms_";
        } else if (Includes(key, GB_VEL_AVG)) {
            str += "velocity_avg_";
        } else if (Includes(key, GB_TIME_AVG)) {
            str += "time_avg_";
        }
    }

    /// Time
    if (Includes(key, PREV)) {
        str += "prev_";
    } else if (Includes(key, NEXT)) {
        str += "next_";
    } else if (Includes(key, CURR)) {
        str += "curr_";
    }

    /// Direction
    if (Includes(key, DIR_X)) {
        str += "x_";
    } else if (Includes(key, DIR_Y)) {
        str += "y_";
    } else if (Includes(key, DIR_Z)) {
        str += "z_";
    }
    return str;
}

std::string GridBox::Beautify(std::string str) {
    return this->Capitalize(this->ReplaceAll(str, "_", " "));
}

std::string GridBox::Capitalize(std::string str) {
    bool cap = true;
    for (unsigned int i = 0; i <= str.length(); i++) {
        if (isalpha(str[i]) && cap == true) {
            str[i] = toupper(str[i]);
            cap = false;
        } else if (isspace(str[i])) {
            cap = true;
        }
    }
    return str;
}

std::string GridBox::ReplaceAll(std::string str,
                                const std::string &from,
                                const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
