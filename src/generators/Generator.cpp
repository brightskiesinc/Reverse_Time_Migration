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

#include <stbx/generators/Generator.hpp>

#include <stbx/generators/common/Keys.hpp>
#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>
#include <stbx/generators/primitive/CallbacksGenerator.hpp>
#include <stbx/generators/concrete/computation-parameters/computation_parameters_generator.h>
#include <stbx/generators/primitive/ComponentsGenerator.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <iostream>
#include <string>

using namespace std;

using namespace stbx::agents;
using namespace stbx::writers;
using namespace stbx::generators;

using namespace bs::base::configurations;
using namespace operations::configurations;
using namespace operations::common;
using namespace operations::helpers::callbacks;
using namespace operations::engines;
using namespace operations::components;
using namespace bs::base::exceptions;
using namespace bs::base::logger;


Generator::Generator(const nlohmann::json &mMap) {
    this->mMap = mMap;
    this->mConfigurationsGenerator = new ConfigurationsGenerator(this->mMap);
    this->mOrder = this->mConfigurationsGenerator->GetEquationOrder();
    this->mSampling = this->mConfigurationsGenerator->GetGridSampling();
    this->mApproximation = this->mConfigurationsGenerator->GetApproximation();
}


CallbackCollection *
Generator::GenerateCallbacks(const string &aWritePath) {
    nlohmann::json callbacks_map = this->mMap[K_CALLBACKS];
    auto *callbacksGenerator = new CallbacksGenerator(aWritePath, callbacks_map);
    return callbacksGenerator->GenerateCallbacks();
}

ModellingEngineConfigurations *
Generator::GenerateModellingEngineConfiguration(const string &aWritePath) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto *configuration = new ModellingEngineConfigurations();

    Logger->Info() << "Reading model files..." << '\n';
    configuration->SetModelFiles(this->mConfigurationsGenerator->GetModelFiles());

    Logger->Info() << "Reading trace files..." << '\n';
    configuration->SetTraceFiles(this->mConfigurationsGenerator->GetTraceFiles());
    configuration->SetSortMin(this->mConfigurationsGenerator->GetTracesMin());
    configuration->SetSortMax(this->mConfigurationsGenerator->GetTracesMax());
    configuration->SetSortKey(this->mConfigurationsGenerator->GetTracesSortKey());

    auto g = new ComponentsGenerator(this->mMap[K_COMPONENTS],
                                     this->mOrder,
                                     this->mSampling,
                                     this->mApproximation);

    if ((this->mOrder == FIRST) || (this->mOrder == SECOND) && (this->mSampling == UNIFORM)) {
        configuration->SetComputationKernel(g->GenerateComputationKernel());
        configuration->SetModelHandler(g->GenerateModelHandler());
        configuration->SetSourceInjector(g->GenerateSourceInjector());
        configuration->SetBoundaryManager(g->GenerateBoundaryManager());
        configuration->SetTraceManager(g->GenerateTraceManager());
        configuration->SetTraceWriter(g->GenerateTraceWriter());
    } else {
        Logger->Error() << "Unsupported settings" << '\n';
        exit(EXIT_FAILURE);
    }
    return configuration;
}

ComputationParameters *
Generator::GenerateParameters() {
    return generate_parameters(this->mMap);
}

RTMEngineConfigurations *
Generator::GenerateRTMConfiguration(const string &aWritePath) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    auto *configuration = new RTMEngineConfigurations();

    Logger->Info() << "Reading model files..." << '\n';
    configuration->SetModelFiles(this->mConfigurationsGenerator->GetModelFiles());

    Logger->Info() << "Reading trace files..." << '\n';
    configuration->SetTraceFiles(this->mConfigurationsGenerator->GetTraceFiles());
    configuration->SetSortMin(this->mConfigurationsGenerator->GetTracesMin());
    configuration->SetSortMax(this->mConfigurationsGenerator->GetTracesMax());
    configuration->SetSortKey(this->mConfigurationsGenerator->GetTracesSortKey());

    auto g = new ComponentsGenerator(this->mMap[K_COMPONENTS],
                                     this->mOrder,
                                     this->mSampling,
                                     this->mApproximation);

    if ((this->mOrder == FIRST) || (this->mOrder == SECOND) && (this->mSampling == UNIFORM)) {
        configuration->SetComputationKernel(g->GenerateComputationKernel());
        configuration->SetModelHandler(g->GenerateModelHandler());
        configuration->SetSourceInjector(g->GenerateSourceInjector());
        configuration->SetBoundaryManager(g->GenerateBoundaryManager());
        configuration->SetForwardCollector(g->GenerateForwardCollector(aWritePath));
        configuration->SetMigrationAccommodator(g->GenerateMigrationAccommodator());
        configuration->SetTraceManager(g->GenerateTraceManager());
    } else {
        Logger->Error() << "Unsupported settings" << '\n';
        exit(EXIT_FAILURE);
    }
    return configuration;
}

Agent *
Generator::GenerateAgent() {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    nlohmann::json agents_map = this->mMap[K_SYSTEM][K_AGENT];

    Agent *agent;
    if (agents_map[OP_K_TYPE].get<string>() == "normal") {
        agent = new NormalAgent();
        Logger->Info() << "using single Agent" << '\n';
    }
#if defined(USING_MPI)
        else if (agents_map[OP_K_TYPE].get<string>() == "mpi-static-server") {
            Logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Static With Server" << '\n';
            agent = new StaticServerAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-static-serverless") {
            Logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Static Without Server" << '\n';
            agent = new StaticServerlessAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-dynamic-server") {
            Logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Dynamic With Server" << '\n';
            agent = new DynamicServerAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-dynamic-serverless") {
            Logger->Info() << "Using MPI Shot Distribution:"
                              "\n\tDistribution Type: Dynamic Without Server" << '\n';
            agent = new DynamicServerlessAgent();
        }
#endif
    else {
        throw UNDEFINED_EXCEPTION();
    }
    return agent;
}

Writer *
Generator::GenerateWriter() {
    nlohmann::json migration_accommodator_map = this->mMap[K_COMPONENTS][K_MIGRATION_ACCOMMODATOR];

    Writer *writer;
    // If common image gather type, create specified writer. Otherwise,
    // use normal one(should make it smarter by supporting stacking
    // option inside the writer if we have an output with gathers.
    if (migration_accommodator_map[OP_K_TYPE].get<string>() == "adcig") {
        writer = new ADCIGWriter();
    } else {
        writer = new NormalWriter();
    }
    return writer;
}

EngineConfigurations *
Generator::GenerateEngineConfiguration(const string &aWritePath) {
    auto logger = LoggerSystem::GetInstance();
    auto algorithm = this->mMap[K_SYSTEM][K_ALGORITHM][OP_K_TYPE];

    EngineConfigurations *engine_configuration;
    if (algorithm == "rtm") {
        engine_configuration = this->GenerateRTMConfiguration(aWritePath);
    } else {
        logger->Error() << "Unsupported algorithm..."
                        << "Terminating...\n";
        exit(EXIT_FAILURE);
    }
    return engine_configuration;
}

Engine *
Generator::GenerateEngine(const string &aWritePath) {
    auto logger = LoggerSystem::GetInstance();
    auto algorithm = this->mMap[K_SYSTEM][K_ALGORITHM][OP_K_TYPE];

    Engine *engine;
    if (algorithm == "rtm") {
        engine = new RTMEngine(this->GenerateRTMConfiguration(aWritePath),
                               this->GenerateParameters(),
                               this->GenerateCallbacks(aWritePath));
        logger->Info() << "RTM engine generated successfully...\n";
    } else {
        logger->Error() << "Unsupported algorithm..."
                        << "Terminating...\n";
        exit(EXIT_FAILURE);
    }
    return engine;
}

bs::base::configurations::ConfigurationMap *
Generator::GenerateTimerConfiguration() {
    //to-do: Check if timer is configured
    auto unit = this->mMap[K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT];
    if (unit == "sec") {
        this->mMap[K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT] = 1;
    } else if (unit == "milli") {
        this->mMap[K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT] = 1e-3;
    } else if (unit == "micro") {
        this->mMap[K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT] = 1e-6;
    } else if (unit == "nano") {
        this->mMap[K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT] = 1e-9;
    }
    return new bs::base::configurations::JSONConfigurationMap(this->mMap[K_TIMER]);
}

