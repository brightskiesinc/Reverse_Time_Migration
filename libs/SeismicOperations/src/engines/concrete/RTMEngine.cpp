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

#include <operations/engines/concrete/RTMEngine.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/memory/MemoryManager.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#define PB_STR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PB_WIDTH 50

using namespace std;
using namespace bs::base::configurations;
using namespace operations::configurations;
using namespace operations::engines;
using namespace bs::timer;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::helpers::callbacks;
using namespace bs::base::exceptions;
using namespace bs::base::logger;
using namespace bs::base::memory;


void print_progress(double percentage, const char *str = nullptr) {
    int val = (int) (percentage * 100);
    int left_pad = (int) (percentage * PB_WIDTH);
    int right_pad = PB_WIDTH - left_pad;
    printf("\r%s\t%3d%% [%.*s%*s]", str, val, left_pad, PB_STR, right_pad, "");
    fflush(stdout);
}

RTMEngine::RTMEngine(RTMEngineConfigurations *apConfiguration,
                     ComputationParameters *apParameters) {
    this->mpConfiguration = apConfiguration;
    this->mpParameters = apParameters;
    this->mpCallbacks = new CallbackCollection();
}

RTMEngine::RTMEngine(RTMEngineConfigurations *apConfiguration,
                     ComputationParameters *apParameters,
                     helpers::callbacks::CallbackCollection *apCallbackCollection) {
    this->mpConfiguration = apConfiguration;
    this->mpParameters = apParameters;
    this->mpCallbacks = apCallbackCollection;
}

RTMEngine::~RTMEngine() {
    delete this->mpConfiguration;
    delete this->mpParameters;
    delete this->mpCallbacks;
}

vector<uint>
RTMEngine::GetValidShots() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    Logger->Info() << "Detecting available shots for processing..." << '\n';
    ElasticTimer timer("Engine::GetValidShots");
    timer.Start();
    vector<uint> possible_shots =
            this->mpConfiguration->GetTraceManager()->GetWorkingShots(
                    this->mpConfiguration->GetTraceFiles(),
                    this->mpConfiguration->GetSortMin(),
                    this->mpConfiguration->GetSortMax(),
                    this->mpConfiguration->GetSortKey());
    timer.Stop();
    if (possible_shots.empty()) {
        Logger->Error() << "No valid shots detected... terminating." << '\n';
        exit(EXIT_FAILURE);
    }
    Logger->Info() << "Valid shots detected to process\t: "
                   << possible_shots.size() << '\n';
    return possible_shots;
}

GridBox *
RTMEngine::Initialize() {
    ScopeTimer t("Engine::Initialization");
#ifndef NDEBUG
    this->mpCallbacks->BeforeInitialization(this->mpParameters);
#endif

    /// Set computation parameters to all components with
    /// parameters given to the constructor for all needed functions.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetComputationParameters(this->mpParameters);
    }

    /// Set computation parameters to all dependent components with
    /// parameters given to the constructor for all needed functions.
    for (auto const &dependent_component :
            this->mpConfiguration->GetDependentComponents()->ExtractValues()) {
        dependent_component->SetComputationParameters(this->mpParameters);
    }

    /// Set dependent components to all components with for all
    /// needed functions.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetDependentComponents(
                this->mpConfiguration->GetDependentComponents());
    }

    /// Set Components Map to all components.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetComponentsMap(this->mpConfiguration->GetComponents());
    }

    /// Acquire Configuration to all components with
    /// parameters given to the constructor for all needed functions.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->AcquireConfiguration();
    }

    this->mpParameters->SetMaxPropagationFrequency(
            this->mpConfiguration->GetSourceInjector()->GetMaxFrequency());
    GridBox *gb;
    {
        ScopeTimer timer("ModelHandler::ReadModel");
        gb = this->mpConfiguration->GetModelHandler()->ReadModel(this->mpConfiguration->GetModelFiles());
    }
    /// Set the GridBox with the parameters given to the constructor for
    /// all needed functions.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetGridBox(gb);
    }
    {
        ScopeTimer timer("ModelHandler::PreprocessModel");
        this->mpConfiguration->GetComputationKernel()->PreprocessModel();
    }
    {
        ScopeTimer timer("BoundaryManager::ExtendModel");
        this->mpConfiguration->GetBoundaryManager()->ExtendModel();
    }

#ifndef NDEBUG
    this->mpCallbacks->AfterInitialization(gb);
#endif
    this->mpConfiguration->GetComputationKernel()->SetBoundaryManager(
            this->mpConfiguration->GetBoundaryManager());

    gb->Report(VERBOSE);
    return gb;
}

void
RTMEngine::MigrateShots(vector<uint> shot_numbers, GridBox *apGridBox) {
    for (auto shot_number : shot_numbers) {
        this->MigrateShots(shot_number, apGridBox);
    }
}

void
RTMEngine::MigrateShots(uint shot_id, GridBox *apGridBox) {
    ScopeTimer t("Engine::MigrateShot");

    this->mpConfiguration->GetMigrationAccommodator()->ResetShotCorrelation();
    {
        ScopeTimer timer("TraceManager::ReadShot");
        this->mpConfiguration->GetTraceManager()->ReadShot(
                this->mpConfiguration->GetTraceFiles(), shot_id, this->mpConfiguration->GetSortKey());
    }

#ifndef NDEBUG
    this->mpCallbacks->BeforeShotPreprocessing(
            this->mpConfiguration->GetTraceManager()->GetTracesHolder());
#endif

    {
        ScopeTimer timer("TraceManager::PreprocessShot");
        this->mpConfiguration->GetTraceManager()->PreprocessShot();
    }


#ifndef NDEBUG
    this->mpCallbacks->AfterShotPreprocessing(
            this->mpConfiguration->GetTraceManager()->GetTracesHolder());
#endif

    this->mpConfiguration->GetSourceInjector()->SetSourcePoint(
            this->mpConfiguration->GetTraceManager()->GetSourcePoint());

    {
        ScopeTimer timer("ModelHandler::SetupWindow");
        this->mpConfiguration->GetModelHandler()->SetupWindow();
    }
    {
        ScopeTimer timer("BoundaryManager::ReExtendModel");
        this->mpConfiguration->GetBoundaryManager()->ReExtendModel();
    }
    {
        ScopeTimer timer("ForwardCollector::ResetGrid(Forward)");
        this->mpConfiguration->GetForwardCollector()->ResetGrid(true);
    }

#ifndef NDEBUG
    this->mpCallbacks->BeforeForwardPropagation(apGridBox);
#endif

    this->Forward(apGridBox);
    {
        ScopeTimer timer("ForwardCollector::ResetGrid(Backward)");
        this->mpConfiguration->GetForwardCollector()->ResetGrid(false);
    }
    {
        ScopeTimer timer("BoundaryManager::AdjustModelForBackward");
        this->mpConfiguration->GetBoundaryManager()->AdjustModelForBackward();
    }

#ifndef NDEBUG
    this->mpCallbacks->BeforeBackwardPropagation(apGridBox);
#endif

    this->Backward(apGridBox);

#ifndef NDEBUG
    this->mpCallbacks->BeforeShotStacking(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetShotCorrelation());
#endif

    this->mpConfiguration->GetMigrationAccommodator()->SetSourcePoint(
            this->mpConfiguration->GetTraceManager()->GetSourcePoint());
    {
        ScopeTimer timer("Correlation::Stack");
        this->mpConfiguration->GetMigrationAccommodator()->Stack();
    }

#ifndef NDEBUG
    this->mpCallbacks->AfterShotStacking(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetStackedShotCorrelation());
#endif
}

MigrationData *
RTMEngine::Finalize(GridBox *apGridBox) {
#ifndef NDEBUG
    this->mpCallbacks->AfterMigration(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetStackedShotCorrelation());
#endif
    MigrationData *md;
    {
        ScopeTimer t("CorrelationKernel::GetMigrationData");
        md = this->mpConfiguration->GetMigrationAccommodator()->GetMigrationData();
    }
    {
        ScopeTimer t("ModelHandler::PostProcessMigration");
        this->mpConfiguration->GetModelHandler()->PostProcessMigration(md);
    }
    delete apGridBox;
    return md;
}

void
RTMEngine::Forward(GridBox *apGridBox) {
    ScopeTimer t("Engine::Forward");

    auto logger = LoggerSystem::GetInstance();
    this->mpConfiguration->GetComputationKernel()->SetMode(
            components::KERNEL_MODE::FORWARD);
    int time_steps = this->mpConfiguration->GetSourceInjector()->GetPrePropagationNT();
    // Do prequel source injection before main forward propagation.
    for (int it = -time_steps; it < 1; it++) {
        {
            ScopeTimer timer("SourceInjector::ApplySource");
            this->mpConfiguration->GetSourceInjector()->ApplySource(it);
        }
        {
            ScopeTimer timer("Forward::ComputationKernel::Step");
            this->mpConfiguration->GetComputationKernel()->Step();
        }
#ifndef NDEBUG
        this->mpCallbacks->AfterForwardStep(apGridBox, it);
#endif
    }
    uint one_percent = apGridBox->GetNT() / 100 + 1;
    for (int it = 1; it < apGridBox->GetNT(); it++) {
        {
            ScopeTimer timer("ForwardCollector::SaveForward");
            this->mpConfiguration->GetForwardCollector()->SaveForward();
        }
        {
            ScopeTimer timer("SourceInjector::ApplySource");
            this->mpConfiguration->GetSourceInjector()->ApplySource(it);
        }
        {
            ScopeTimer timer("Forward::ComputationKernel::Step");
            this->mpConfiguration->GetComputationKernel()->Step();
        }
#ifndef NDEBUG
        this->mpCallbacks->AfterForwardStep(apGridBox, it);
#endif
        if ((it % one_percent) == 0) {
            print_progress(((float) it) / apGridBox->GetNT(), "Forward Propagation");
        }
    }
    print_progress(1, "Forward Propagation");
    logger->Info() << " ... Done" << '\n';
}

void
RTMEngine::Backward(GridBox *apGridBox) {
    ScopeTimer t("Engine::Backward");
    auto logger = LoggerSystem::GetInstance();
    this->mpConfiguration->GetComputationKernel()->SetMode(
            components::KERNEL_MODE::ADJOINT);
    uint onePercent = apGridBox->GetNT() / 100 + 1;
    for (uint it = apGridBox->GetNT() - 1; it > 0; it--) {
        {
            ScopeTimer timer("TraceManager::ApplyTraces");
            this->mpConfiguration->GetTraceManager()->ApplyTraces(it);
        }
        {
            ScopeTimer timer("Backward::ComputationKernel::Step");
            this->mpConfiguration->GetComputationKernel()->Step();
        }
        {
            ScopeTimer timer("ForwardCollector::FetchForward");
            this->mpConfiguration->GetForwardCollector()->FetchForward();
        }
#ifndef NDEBUG
        this->mpCallbacks->AfterFetchStep(
                this->mpConfiguration->GetForwardCollector()->GetForwardGrid(), it);
        this->mpCallbacks->AfterBackwardStep(apGridBox, it);
#endif
        {
            ScopeTimer timer("Correlation::Correlate");
            this->mpConfiguration->GetMigrationAccommodator()->Correlate(
                    this->mpConfiguration->GetForwardCollector()->GetForwardGrid());
        }
        if ((it % onePercent) == 0) {
            print_progress(((float) (apGridBox->GetNT() - it)) / apGridBox->GetNT(), "Backward Propagation");
        }
    }
    print_progress(1, "Backward Propagation");
    logger->Info() << " ... Done" << '\n';
}
