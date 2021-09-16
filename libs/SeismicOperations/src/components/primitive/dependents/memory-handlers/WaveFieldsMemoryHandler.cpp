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

#include "operations/components/dependents/concrete/memory-handlers/WaveFieldsMemoryHandler.hpp"
#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/memory/MemoryManager.hpp>

#include <cmath>

using namespace operations::components;
using namespace operations::common;
using namespace operations::dataunits;
using namespace bs::base::logger;
using namespace bs::base::memory;


WaveFieldsMemoryHandler::WaveFieldsMemoryHandler(
        bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
}

WaveFieldsMemoryHandler::~WaveFieldsMemoryHandler() = default;

void WaveFieldsMemoryHandler::AcquireConfiguration() {}

void WaveFieldsMemoryHandler::CloneWaveFields(GridBox *_src, GridBox *_dst) {

    uint wnx = _src->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = _src->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = _src->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;

    /// Allocating and zeroing wave fields.
    for (auto wave_field : _src->GetWaveFields()) {
        if (!GridBox::Includes(wave_field.first, NEXT)) {
            auto frame_buffer = new FrameBuffer<float>();

            frame_buffer->Allocate(window_size,
                                   mpParameters->GetHalfLength(),
                                   GridBox::Stringify(wave_field.first));

            this->FirstTouch(frame_buffer->GetNativePointer(), _src, true);
            _dst->RegisterWaveField(wave_field.first, frame_buffer);


            Device::MemCpy(_dst->Get(wave_field.first)->GetNativePointer(),
                           _src->Get(wave_field.first)->GetNativePointer(),
                           window_size * sizeof(float));

            GridBox::Key wave_field_new = wave_field.first;

            if (GridBox::Includes(wave_field_new, GB_PRSS | PREV)) {
                GridBox::Replace(&wave_field_new, PREV, NEXT);
                _dst->RegisterWaveField(wave_field_new, frame_buffer);
            } else if (this->mpParameters->GetEquationOrder() == FIRST &&
                       GridBox::Includes(wave_field_new, GB_PRSS | CURR)) {
                GridBox::Replace(&wave_field_new, CURR, NEXT);
                _dst->RegisterWaveField(wave_field_new, frame_buffer);
            }
        }
    }
}

void WaveFieldsMemoryHandler::CopyWaveFields(GridBox *_src, GridBox *_dst) {

    LoggerSystem *Logger = LoggerSystem::GetInstance();

    uint wnx = _src->GetWindowAxis()->GetXAxis().GetActualAxisSize();
    uint wny = _src->GetWindowAxis()->GetYAxis().GetActualAxisSize();
    uint wnz = _src->GetWindowAxis()->GetZAxis().GetActualAxisSize();

    uint const window_size = wnx * wny * wnz;

    for (auto wave_field : _src->GetWaveFields()) {
        auto src = _src->Get(wave_field.first)->GetNativePointer();
        auto dst = _dst->Get(wave_field.first)->GetNativePointer();

        if (src != nullptr && dst != nullptr) {
            Device::MemCpy(dst, src, window_size * sizeof(float));
        } else {
            Logger->Error() << "No Wave Fields allocated to be copied... "
                            << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }

    }
}

void WaveFieldsMemoryHandler::FreeWaveFields(GridBox *apGridBox) {
    for (auto &wave_field : apGridBox->GetWaveFields()) {
        if (!GridBox::Includes(wave_field.first, NEXT)) {
            wave_field.second->Free();
        }
    }
}

void WaveFieldsMemoryHandler::SetComputationParameters(
        ComputationParameters *apParameters) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    this->mpParameters = (ComputationParameters *) apParameters;
    if (this->mpParameters == nullptr) {
        Logger->Error() << "No computation parameters provided... Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
}
