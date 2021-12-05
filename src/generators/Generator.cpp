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

#include <iostream>
#include <string>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>

#include <stbx/generators/Generator.hpp>

#include <stbx/generators/common/Keys.hpp>
#include <stbx/generators/primitive/ConfigurationsGenerator.hpp>
#include <stbx/generators/primitive/CallbacksGenerator.hpp>
#include <stbx/generators/concrete/computation-parameters/computation_parameters_generator.h>
#include <stbx/generators/primitive/ComponentsGenerator.hpp>


using namespace std;

using namespace bs::base::exceptions;
using namespace bs::base::logger;
using namespace bs::base::configurations;

using namespace stbx::agents;
using namespace stbx::writers;
using namespace stbx::generators;

using namespace operations::configurations;
using namespace operations::common;
using namespace operations::helpers::callbacks;
using namespace operations::engines;
using namespace operations::components;


Generator::Generator(const nlohmann::json &mMap) {
    this->mMap = mMap;
    this->mConfigurationsGenerator = new ConfigurationsGenerator(this->mMap);
    this->mOrder = this->mConfigurationsGenerator->GetEquationOrder();
    this->mSampling = this->mConfigurationsGenerator->GetGridSampling();
    this->mApproximation = this->mConfigurationsGenerator->GetApproximation();
}


CallbackCollection *
Generator::GenerateCallbacks(const string &aWritePath) {
    auto callbacks_map = this->mMap[K_CALLBACKS];
    auto callbacks_generator = new CallbacksGenerator(aWritePath, callbacks_map);
    return callbacks_generator->GenerateCallbacks();
}

ModellingEngineConfigurations *
Generator::GenerateModellingEngineConfiguration(const string &aWritePath) {
    auto logger = LoggerSystem::GetInstance();
    auto configuration = new ModellingEngineConfigurations();

    logger->Info() << "Reading model files..." << '\n';
    configuration->SetModelFiles(this->mConfigurationsGenerator->GetModelFiles());

    logger->Info() << "Reading trace files..." << '\n';
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
        logger->Error() << "Unsupported settings" << '\n';
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
    auto logger = LoggerSystem::GetInstance();
    auto configuration = new RTMEngineConfigurations();

    logger->Info() << "Reading model files..." << '\n';
    configuration->SetModelFiles(this->mConfigurationsGenerator->GetModelFiles());

    logger->Info() << "Reading trace files..." << '\n';
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
        logger->Error() << "Unsupported settings" << '\n';
        exit(EXIT_FAILURE);
    }
    return configuration;
}

Agent *
Generator::GenerateAgent() {
    auto logger = LoggerSystem::GetInstance();
    auto agents_map = this->mMap[K_SYSTEM][K_AGENT];

    Agent *agent;
    if (agents_map[OP_K_TYPE].get<string>() == "normal") {
        agent = new NormalAgent();
        logger->Info() << "using single Agent" << '\n';
    }
#if defined(USING_MPI)
        else if (agents_map[OP_K_TYPE].get<string>() == "mpi-static-server") {
            logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Static With Server" << '\n';
            agent = new StaticServerAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-static-serverless") {
            logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Static Without Server" << '\n';
            agent = new StaticServerlessAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-dynamic-server") {
            logger->Info() << "Using MPI Shot Distribution: "
                              "\n\tDistribution Type: Dynamic With Server" << '\n';
            agent = new DynamicServerAgent();
        } else if (agents_map[OP_K_TYPE].get<string>() == "mpi-dynamic-serverless") {
            logger->Info() << "Using MPI Shot Distribution:"
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
    auto migration_accommodator_map = this->mMap[K_COMPONENTS][K_MIGRATION_ACCOMMODATOR];
    auto writer_map = this->mMap[K_SYSTEM][K_WRITER];

    Writer *writer;
    // If common image gather type, create specified writer. Otherwise,
    // use normal one (i.e. Should make it smarter by supporting stacking
    // option inside the writer if we have an output with gathers).
    if (!migration_accommodator_map.empty()) {
        writer = new NormalWriter();
    } else if (!writer_map.empty()) {
        writer = new NormalWriter();
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

ConfigurationMap *
Generator::GenerateTimerConfiguration() {
    /// @todo: Check if timer is configured
    auto unit = this->mMap[K_SYSTEM][K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT];
    if (unit == "sec") {
        unit = 1;
    } else if (unit == "milli") {
        unit = 1e-3;
    } else if (unit == "micro") {
        unit = 1e-6;
    } else if (unit == "nano") {
        unit = 1e-9;
    }
    this->mMap[K_SYSTEM][K_TIMER][K_TIMER_PROPERTIES][K_TIME_UNIT] = unit;
    return new JSONConfigurationMap(this->mMap[K_SYSTEM][K_TIMER]);
}
