//
// Created by amr-nasr on 20/10/2019.
//

#include <operations/engines/concrete/RTMEngine.hpp>

#include <memory-manager/MemoryManager.h>

#include <iostream>

#define PB_STR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PB_WIDTH 50

using namespace operations::engines;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::configuration;
using namespace operations::helpers::callbacks;
using namespace operations::exceptions;


void print_progress(double percentage, const char *str = nullptr) {
    int val = (int) (percentage * 100);
    int left_pad = (int) (percentage * PB_WIDTH);
    int right_pad = PB_WIDTH - left_pad;
    printf("\r%s\t%3d%% [%.*s%*s]", str, val, left_pad, PB_STR, right_pad, "");
    fflush(stdout);
}

using namespace std;

RTMEngine::RTMEngine(RTMEngineConfigurations *apConfiguration,
                     ComputationParameters *apParameters) {
    this->mpConfiguration = apConfiguration;
    this->mpParameters = apParameters;
    this->mpCallbacks = new CallbackCollection();
    this->mpTimer = Timer::GetInstance();
}

RTMEngine::RTMEngine(RTMEngineConfigurations *apConfiguration,
                     ComputationParameters *apParameters,
                     helpers::callbacks::CallbackCollection *apCallbackCollection) {
    this->mpConfiguration = apConfiguration;
    this->mpParameters = apParameters;
    this->mpCallbacks = apCallbackCollection;
    this->mpTimer = Timer::GetInstance();
}

RTMEngine::~RTMEngine() = default;

vector<uint> RTMEngine::GetValidShots() {
    cout << "Detecting available shots for processing..." << std::endl;
    Timer *timer = Timer::GetInstance();
    timer->StartTimer("Engine::GetValidShots");
    vector<uint> possible_shots =
            this->mpConfiguration->GetTraceManager()->GetWorkingShots(
                    this->mpConfiguration->GetTraceFiles(),
                    this->mpConfiguration->GetSortMin(),
                    this->mpConfiguration->GetSortMax(),
                    this->mpConfiguration->GetSortKey());
    timer->StopTimer("Engine::GetValidShots");
    if (possible_shots.empty()) {
        cout << "No valid shots detected... terminating." << std::endl;
        exit(EXIT_FAILURE);
    }
    cout << "Valid shots detected to process\t: "
         << possible_shots.size() << std::endl;
    return possible_shots;
}

GridBox *RTMEngine::Initialize() {
    this->mpTimer->StartTimer("Engine::Initialization");
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

    this->mpTimer->StartTimer("ModelHandler::ReadModel");
    GridBox *grid_box =
            this->mpConfiguration->GetModelHandler()->ReadModel(
                    this->mpConfiguration->GetModelFiles());
    this->mpTimer->StopTimer("ModelHandler::ReadModel");

    /// Set the GridBox with the parameters given to the constructor for
    /// all needed functions.
    for (auto const &component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetGridBox(grid_box);
    }

    this->mpTimer->StartTimer("ModelHandler::PreprocessModel");
    this->mpConfiguration->GetModelHandler()->PreprocessModel();
    this->mpTimer->StopTimer("ModelHandler::PreprocessModel");
    this->mpTimer->StartTimer("BoundaryManager::ExtendModel");
    this->mpConfiguration->GetBoundaryManager()->ExtendModel();
    this->mpTimer->StopTimer("BoundaryManager::ExtendModel");
#ifndef NDEBUG
    this->mpCallbacks->AfterInitialization(grid_box);
#endif
    this->mpConfiguration->GetComputationKernel()->SetBoundaryManager(
            this->mpConfiguration->GetBoundaryManager());
    this->mpTimer->StopTimer("Engine::Initialization");

    grid_box->Report(VERBOSE);
    return grid_box;
}

void RTMEngine::MigrateShots(vector<uint> shot_numbers, GridBox *apGridBox) {
    for (int shot_number:shot_numbers) {
        MigrateShots(shot_number, apGridBox);
    }
}

void RTMEngine::MigrateShots(uint shot_id, GridBox *apGridBox) {
    this->mpTimer->StartTimer("Engine::MigrateShot");
    this->mpConfiguration->GetMigrationAccommodator()->ResetShotCorrelation();

    this->mpTimer->StartTimer("TraceManager::ReadShot");
    this->mpConfiguration->GetTraceManager()->ReadShot(
            this->mpConfiguration->GetTraceFiles(), shot_id, this->mpConfiguration->GetSortKey());
    this->mpTimer->StopTimer("TraceManager::ReadShot");

#ifndef NDEBUG
    this->mpCallbacks->BeforeShotPreprocessing(
            this->mpConfiguration->GetTraceManager()->GetTracesHolder());
#endif

    this->mpTimer->StartTimer("TraceManager::PreprocessShot");
    this->mpConfiguration->GetTraceManager()->PreprocessShot(
            this->mpConfiguration->GetSourceInjector()->GetCutOffTimeStep());
    this->mpTimer->StopTimer("TraceManager::PreprocessShot");

#ifndef NDEBUG
    this->mpCallbacks->AfterShotPreprocessing(
            this->mpConfiguration->GetTraceManager()->GetTracesHolder());
#endif

    this->mpConfiguration->GetSourceInjector()->SetSourcePoint(
            this->mpConfiguration->GetTraceManager()->GetSourcePoint());

    this->mpTimer->StartTimer("ModelHandler::SetupWindow");
    this->mpConfiguration->GetModelHandler()->SetupWindow();
    this->mpTimer->StopTimer("ModelHandler::SetupWindow");

    this->mpTimer->StartTimer("BoundaryManager::ReExtendModel");
    this->mpConfiguration->GetBoundaryManager()->ReExtendModel();
    this->mpTimer->StopTimer("BoundaryManager::ReExtendModel");

    this->mpTimer->StartTimer("ForwardCollector::ResetGrid(Forward)");
    this->mpConfiguration->GetForwardCollector()->ResetGrid(true);
    this->mpTimer->StopTimer("ForwardCollector::ResetGrid(Forward)");

#ifndef NDEBUG
    this->mpCallbacks->BeforeForwardPropagation(apGridBox);
#endif

    this->Forward(apGridBox);

    this->mpTimer->StartTimer("ForwardCollector::ResetGrid(Backward)");
    this->mpConfiguration->GetForwardCollector()->ResetGrid(false);
    this->mpTimer->StopTimer("ForwardCollector::ResetGrid(Backward)");

    this->mpTimer->StartTimer("BoundaryManager::AdjustModelForBackward");
    this->mpConfiguration->GetBoundaryManager()->AdjustModelForBackward();
    this->mpTimer->StopTimer("BoundaryManager::AdjustModelForBackward");

#ifndef NDEBUG
    this->mpCallbacks->BeforeBackwardPropagation(apGridBox);
#endif

    this->Backward(apGridBox);
#ifndef NDEBUG
    this->mpCallbacks->BeforeShotStacking(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetShotCorrelation());
#endif

    this->mpTimer->StartTimer("CorrelationKernel::Stack");
    this->mpConfiguration->GetMigrationAccommodator()->Stack();
    this->mpTimer->StopTimer("CorrelationKernel::Stack");

#ifndef NDEBUG
    this->mpCallbacks->AfterShotStacking(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetStackedShotCorrelation());
#endif

    this->mpTimer->StopTimer("Engine::MigrateShot");

}

MigrationData *RTMEngine::Finalize(GridBox *apGridBox) {
#ifndef NDEBUG
    this->mpCallbacks->AfterMigration(
            apGridBox,
            this->mpConfiguration->GetMigrationAccommodator()->GetStackedShotCorrelation());
#endif

    this->mpTimer->StartTimer("CorrelationKernel::GetMigrationData");
    MigrationData *migration = this->mpConfiguration->GetMigrationAccommodator()->GetMigrationData();
    this->mpTimer->StopTimer("CorrelationKernel::GetMigrationData");

    delete apGridBox;
    return migration;
}

void RTMEngine::Forward(GridBox *apGridBox) {
    this->mpTimer->StartTimer("Engine::Forward");
    uint onePercent = apGridBox->GetNT() / 100 + 1;
    for (uint t = 1; t < apGridBox->GetNT(); t++) {
        this->mpTimer->StartTimer("ForwardCollector::SaveForward");
        this->mpConfiguration->GetForwardCollector()->SaveForward();
        this->mpTimer->StopTimer("ForwardCollector::SaveForward");

        this->mpTimer->StartTimer("SourceInjector::ApplySource");
        this->mpConfiguration->GetSourceInjector()->ApplySource(t);
        this->mpTimer->StopTimer("SourceInjector::ApplySource");

        this->mpTimer->StartTimer("Forward::ComputationKernel::Step");
        this->mpConfiguration->GetComputationKernel()->Step();
        this->mpTimer->StopTimer("Forward::ComputationKernel::Step");
#ifndef NDEBUG
        this->mpCallbacks->AfterForwardStep(apGridBox, t);
#endif
        if ((t % onePercent) == 0) {
            print_progress(((float) t) / apGridBox->GetNT(), "Forward Propagation");
        }
    }
    print_progress(1, "Forward Propagation");
    cout << " ... Done" << endl;
    this->mpTimer->StopTimer("Engine::Forward");
}

void RTMEngine::Backward(GridBox *apGridBox) {
    this->mpTimer->StartTimer("Engine::Backward");
    uint onePercent = apGridBox->GetNT() / 100 + 1;
    for (uint t = apGridBox->GetNT() - 1; t > 0; t--) {
        this->mpTimer->StartTimer("TraceManager::ApplyTraces");
        this->mpConfiguration->GetTraceManager()->ApplyTraces(t);
        this->mpTimer->StopTimer("TraceManager::ApplyTraces");

        this->mpTimer->StartTimer("Backward::ComputationKernel::Step");
        this->mpConfiguration->GetComputationKernel()->Step();
        this->mpTimer->StopTimer("Backward::ComputationKernel::Step");

        this->mpTimer->StartTimer("ForwardCollector::FetchForward");
        this->mpConfiguration->GetForwardCollector()->FetchForward();
        this->mpTimer->StopTimer("ForwardCollector::FetchForward");
#ifndef NDEBUG
        this->mpCallbacks->AfterFetchStep(
                this->mpConfiguration->GetForwardCollector()->GetForwardGrid(), t);
        this->mpCallbacks->AfterBackwardStep(apGridBox, t);
#endif
        this->mpTimer->StartTimer("CorrelationKernel::Correlate");
        this->mpConfiguration->GetMigrationAccommodator()->Correlate(
                this->mpConfiguration->GetForwardCollector()->GetForwardGrid());
        this->mpTimer->StopTimer("CorrelationKernel::Correlate");
        if ((t % onePercent) == 0) {
            print_progress(((float) (apGridBox->GetNT() - t)) / apGridBox->GetNT(), "Backward Propagation");
        }
    }
    print_progress(1, "Backward Propagation");

    cout << " ... Done" << endl;
    this->mpTimer->StopTimer("Engine::Backward");
}

