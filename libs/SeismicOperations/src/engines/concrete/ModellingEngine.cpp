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

#include <operations/engines/concrete/ModellingEngine.hpp>

#include <bs/timer/api/cpp/BSTimer.hpp>

#include <bs/base/memory/MemoryManager.hpp>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

#define PB_STR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PB_WIDTH 50

using namespace std;
using namespace operations::configurations;
using namespace operations::engines;
using namespace operations::common;
using namespace operations::dataunits;
using namespace operations::helpers::callbacks;
using namespace bs::timer;
using namespace bs::base::configurations;
using namespace bs::base::logger;
using namespace bs::base::memory;


void print_modelling_progress(double percentage, const char *str = nullptr) {
    int val = (int) (percentage * 100);
    int left_pad = (int) (percentage * PB_WIDTH);
    int right_pad = PB_WIDTH - left_pad;
    printf("\r%s\t%3d%% [%.*s%*s]", str, val, left_pad, PB_STR, right_pad, "");
    fflush(stdout);
}

/*!
 * GetInstance():This is the so-called Singleton design pattern.
 * Its distinguishing feature is that there can only ever be exactly one
 * instance of that class and the pattern ensures that. The class has a private
 * constructor and a statically-created instance that is returned with the
 * GetInstance method. You cannot create an instance from the outside and thus
 * get the object only through said method.Since instance is static in the
 * GetInstance method it will retain its value between multiple invocations. It
 * is allocated and constructed some where before it's first used. E.g. in this
 * answer it seems like GCC initializes the static variable at the time the
 * function is first used.
 */

/*!
 * Constructor to start the modelling engine given the appropriate engine
 * configuration.
 * @param apConfiguration
 * The configuration which will control the actual work of the engine.
 * @param apParameters
 * The computation parameters that will control the simulations settings like
 * boundary length, order of numerical solution.
 */
ModellingEngine::ModellingEngine(ModellingEngineConfigurations *apConfiguration,
                                 ComputationParameters *apParameters) {
    this->mpConfiguration = apConfiguration;
    /**
     * Callbacks are like hooks that you can register function to be called in
     * specific events(etc. after the forward propagation, after each time step,
     * and so on) this enables the user to add functionalities,take snapshots
     * or track state in different approaches. A callback is the class actually
     * implementing the functions to capture or track state like norm, images, csv
     * and so on. They are registered to a callback collection which is what links
     * them to the engine. The engine would inform the callback collection at the
     * time of each event, and the callback collection would then inform each
     * callback registered to it.
     */
    this->mpCallbacks = new CallbackCollection();
    this->mpParameters = apParameters;
}

/**
 * Constructor to start the modelling engine given the appropriate engine
 * configuration.
 * @param apConfiguration
 * The configuration which will control the actual work of the engine.
 * @param apParameters
 * The computation parameters that will control the simulations settings like
 * boundary length, order of numerical solution.
 * @param apCallbackCollection
 * The callbacks registered to be called in the right time.
 */
ModellingEngine::ModellingEngine(ModellingEngineConfigurations *apConfiguration,
                                 ComputationParameters *apParameters,
                                 helpers::callbacks::CallbackCollection *apCallbackCollection) {
    /// Modeling configuration of the class is that given as
    /// argument to the constructor
    this->mpConfiguration = apConfiguration;
    /// Callback of the class is that given as argument to the constructor
    this->mpCallbacks = apCallbackCollection;
    /// Computation parameters of this class is that given as
    /// argument to the constructor.
    this->mpParameters = apParameters;

}

ModellingEngine::~ModellingEngine() = default;

/**
 * Run the initialization steps for the modelling engine.
 */
GridBox *ModellingEngine::Initialize() {
    ScopeTimer t("Engine::Initialization");
#ifndef NDEBUG
    this->mpCallbacks->BeforeInitialization(this->mpParameters);
#endif

    /// Set computation parameters to all components with
    /// parameters given to the constructor for all needed functions.
    for (auto component :
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

    ElasticTimer timer("ModelHandler::ReadModel");
    timer.Start();
    auto gb = this->mpConfiguration->GetModelHandler()->ReadModel(
            this->mpConfiguration->GetModelFiles());
    timer.Stop();

    /// Set the GridBox with the parameters given to the constructor for
    /// all needed functions.
    for (auto component :
            this->mpConfiguration->GetComponents()->ExtractValues()) {
        component->SetGridBox(gb);
    }

    /**
     * All pre-processing needed to be done on the model before the beginning of
     * the reverse time migration, should be applied in this function.
     */
    {
        ScopeTimer timer("ModelHandler::PreprocessModel");
        this->mpConfiguration->GetComputationKernel()->PreprocessModel();
    }

    /**
     * Extends the velocities/densities to the added boundary parts to the
     * velocity/density of the model appropriately. This is only called once after
     * the initialization of the model.
     */
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

vector<uint> ModellingEngine::GetValidShots() {
    auto logger = LoggerSystem::GetInstance();
    logger->Info() << "Detecting available shots for processing..." << '\n';

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
        logger->Error() << "No valid shots detected... terminating." << '\n';
        exit(EXIT_FAILURE);
    }
    logger->Info() << "Valid shots detected to process\t: "
                   << possible_shots.size() << '\n';
    return possible_shots;
}

void ModellingEngine::MigrateShots(vector<uint> aShotNumbers, GridBox *apGridBox) {
    for (auto shot_number : aShotNumbers) {
        this->MigrateShots(shot_number, apGridBox);
    }
}

void ModellingEngine::MigrateShots(uint shot_id, GridBox *apGridBox) {
    ScopeTimer t("Engine::Model");
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
#ifndef NDEBUG
    /// If in the debug mode use the call back of BeforeForwardPropagation and
    /// give it our updated GridBox
    this->mpCallbacks->BeforeForwardPropagation(apGridBox);
#endif
    /// If not in the debug mode(release mode) call the Forward function of
    /// this class and give it our updated GridBox
    /*!
     * Begin the forward propagation and recording of the traces.
     */
    this->Forward(apGridBox, shot_id);
}

MigrationData *ModellingEngine::Finalize(GridBox *apGridBox) {
    this->mpConfiguration->GetTraceWriter()->Finalize();
    return nullptr;
}

void ModellingEngine::Forward(GridBox *apGridBox, uint shot_id) {
    ScopeTimer t("Engine::Forward");
    auto logger = LoggerSystem::GetInstance();
    {
        ScopeTimer timer("TraceWriter::StartRecordingInstance");
        this->mpConfiguration->GetTraceWriter()->StartRecordingInstance(
                *this->mpConfiguration->GetTraceManager()->GetTracesHolder()
        );
    }
    this->mpConfiguration->GetComputationKernel()->SetMode(
            components::KERNEL_MODE::FORWARD);
    int timesteps = this->mpConfiguration->GetSourceInjector()->GetPrePropagationNT();
    // Do prequel source injection before main forward propagation.
    for (int it = -timesteps; it < 1; it++) {
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
    uint onePercent = apGridBox->GetNT() / 100 + 1;
    for (uint t = 1; t < apGridBox->GetNT(); t++) {
        {
            ScopeTimer timer("SourceInjector::ApplySource");
            this->mpConfiguration->GetSourceInjector()->ApplySource(t);
        }
        {
            ScopeTimer timer("Forward::ComputationKernel::Step");
            this->mpConfiguration->GetComputationKernel()->Step();
        }

#ifndef NDEBUG
        /// If in the debug mode use the call back of AfterForwardStep and give
        /// it the updated gridBox
        this->mpCallbacks->AfterForwardStep(apGridBox, t);
#endif
        /// If not in the debug mode (release mode) we use call the RecordTrace()
        {
            ScopeTimer timer("TraceWriter::RecordTrace");
            this->mpConfiguration->GetTraceWriter()->RecordTrace(t);
        }
        /**
         * Records the traces from the domain according to the configuration given
         * in the initialize function.
         */
        if ((t % onePercent) == 0) {
            print_modelling_progress(((float) t) / apGridBox->GetNT(), "Forward Propagation");
        }
    }
    print_modelling_progress(1, "Forward Propagation");
    logger->Info() << " ... Done" << '\n';
    {
        ScopeTimer timer("TraceWriter::FinishRecordingInstance");
        this->mpConfiguration->GetTraceWriter()->FinishRecordingInstance(shot_id);
    }
}
