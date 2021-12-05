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

#include <stbx/generators/primitive/ComponentsGenerator.hpp>

#include <stbx/generators/common/Keys.hpp>
#include <operations/configurations/MapKeys.h>
#include <bs/base/logger/concrete/LoggerSystem.hpp>

using namespace std;
using namespace stbx::generators;
using namespace bs::base::configurations;
using namespace operations::common;
using namespace operations::components;
using namespace bs::base::exceptions;
using namespace bs::base::logger;


ComponentsGenerator::ComponentsGenerator(const nlohmann::json &aMap,
                                         EQUATION_ORDER aOrder,
                                         GRID_SAMPLING aSampling,
                                         APPROXIMATION aApproximation) {
    this->mMap = aMap;
    this->mOrder = aOrder;
    this->mSampling = aSampling;
    this->mApproximation = aApproximation;
}

ComputationKernel *
ComponentsGenerator::GenerateComputationKernel() {
    auto logger = LoggerSystem::GetInstance();
    auto map = this->TruncateMap(K_COMPUTATION_KERNEL);

    if (this->mOrder == FIRST && this->mSampling == UNIFORM) {
        switch (this->mApproximation) {
            case ISOTROPIC:
                logger->Info() << "Generating Staggered First Order Isotropic Computation Kernel...\n";
                return new StaggeredComputationKernel(map);
        }
    } else if (this->mOrder == SECOND && this->mSampling == UNIFORM) {
        switch (this->mApproximation) {
            case ISOTROPIC:
                logger->Info() << "Generating Second Order Isotropic Computation Kernel...\n";
                return new SecondOrderComputationKernel(map);
        }
        logger->Error() << "No entry for wave->physics to identify Computation Kernel..." << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    logger->Error() << "No entry for wave->physics to identify Computation Kernel..." << '\n';
    logger->Error() << "Terminating..." << '\n';
    exit(EXIT_FAILURE);
}

ModelHandler *
ComponentsGenerator::GenerateModelHandler() {
    JSONConfigurationMap *model_handler_map = this->TruncateMap(K_MODEL_HANDLER);
    ModelHandler *model_handler;
    model_handler = new SeismicModelHandler(model_handler_map);
    return model_handler;
}

SourceInjector *
ComponentsGenerator::GenerateSourceInjector() {
    auto logger = LoggerSystem::GetInstance();
    if (this->mMap[K_SOURCE_INJECTOR].empty()) {
        logger->Error() << "No entry for source-injector key : supported values [ ricker ]" << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    auto type = this->mMap[K_SOURCE_INJECTOR][OP_K_TYPE].get<string>();
    auto map = this->TruncateMap(K_SOURCE_INJECTOR);
    SourceInjector *source_injector;

    if (type == "ricker") {
        logger->Info() << "Generating Ricker Source Injector...\n";
        source_injector = new RickerSourceInjector(map);
    } else {
        logger->Error() << "Invalid value for source-injector key : supported values [ ricker ]" << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    return source_injector;
}

BoundaryManager *
ComponentsGenerator::GenerateBoundaryManager() {
    auto logger = LoggerSystem::GetInstance();
    if (this->mMap[K_BOUNDARY_MANAGER].empty()) {
        logger->Error() << "No entry for boundary-manager key : supported values "
                        << K_SUPPORTED_VALUES_BOUNDARY_MANAGER << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    auto type = this->mMap[K_BOUNDARY_MANAGER][OP_K_TYPE].get<string>();
    auto map = this->TruncateMap(K_BOUNDARY_MANAGER);
    BoundaryManager *boundary_manager = nullptr;

    if (type == "none") {
        logger->Info() << "Generating None Boundary Manager...\n";
        boundary_manager = new NoBoundaryManager(map);
    } else if (type == "random") {
        logger->Info() << "Generating Random Boundary Manager...\n";
        boundary_manager = new RandomBoundaryManager(map);
    } else if (type == "sponge") {
        logger->Info() << "Generating Sponge Boundary Manager...\n";
        boundary_manager = new SpongeBoundaryManager(map);
    } else if (type == "cpml") {
        if (this->mApproximation == ISOTROPIC) {
            if (this->mOrder == FIRST) {
                logger->Info() << "Generating Staggered First Order Isotropic CPML Boundary Manager...\n";
                boundary_manager = new StaggeredCPMLBoundaryManager(map);
            } else if (this->mOrder == SECOND) {
                logger->Info() << "Generating Second Order Isotropic CPML Boundary Manager...\n";
                boundary_manager = new CPMLBoundaryManager(map);
            }
        }
        if (boundary_manager == nullptr) {
            logger->Error() << "Invalid value for boundary-manager key : "
                            << "Unsupported for this approximation/order pair" << '\n';
            logger->Error() << "Terminating..." << '\n';
            exit(EXIT_FAILURE);
        }
    }

    if (boundary_manager == nullptr) {
        logger->Error() << "Invalid value for boundary-manager key : supported values "
                        << K_SUPPORTED_VALUES_BOUNDARY_MANAGER << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    return boundary_manager;
}

ForwardCollector *
ComponentsGenerator::GenerateForwardCollector(const string &write_path) {
    auto logger = LoggerSystem::GetInstance();
    if (this->mMap[K_FORWARD_COLLECTOR].empty()) {
        logger->Error() << "No entry for forward-collector key : supported values "
                        << K_SUPPORTED_VALUES_FORWARD_COLLECTOR << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    auto type = this->mMap[K_FORWARD_COLLECTOR][OP_K_TYPE].get<string>();
    auto map = this->TruncateMap(K_FORWARD_COLLECTOR);

    map->WriteValue(OP_K_PROPRIETIES, OP_K_WRITE_PATH, write_path);
    ForwardCollector *forward_collector = nullptr;

    if (type == "two") {
        logger->Info() << "Generating Two Propagation Forward Collector...\n";
        forward_collector = new TwoPropagation(map);
    } else if (type == "three") {
        logger->Info() << "Generating Three Propagation Forward Collector...\n";
        forward_collector = new ReversePropagation(map);
    }

    if (forward_collector == nullptr) {
        logger->Error() << "Invalid value for forward-collector key : supported values "
                        << K_SUPPORTED_VALUES_FORWARD_COLLECTOR << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    return forward_collector;
}


MigrationAccommodator *
ComponentsGenerator::GenerateMigrationAccommodator() {
    auto logger = LoggerSystem::GetInstance();
    if (this->mMap[K_MIGRATION_ACCOMMODATOR].empty()) {
        logger->Error() << "No entry for migration-accommodator key : supported values [ ""cross-correlation ]" << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }

    auto type = this->mMap[K_MIGRATION_ACCOMMODATOR][OP_K_TYPE].get<string>();
    auto map = this->TruncateMap(K_MIGRATION_ACCOMMODATOR);
    MigrationAccommodator *correlation_kernel = nullptr;

    if (type == "cross-correlation") {
        logger->Info() << "Generating Cross Correlation Migration Accommodator...\n";
        correlation_kernel = new CrossCorrelationKernel(map);
    }

    if (correlation_kernel == nullptr) {
        logger->Error()
                << "Invalid value for migration-accommodator key : supported values [cross-correlation]"
                << '\n';
        logger->Error() << "Terminating..." << '\n';
        exit(EXIT_FAILURE);
    }
    return correlation_kernel;
}

TraceManager *
ComponentsGenerator::GenerateTraceManager() {
    auto map = this->TruncateMap(K_TRACE_MANAGER);
    TraceManager *trace_manager;
    trace_manager = new SeismicTraceManager(map);
    return trace_manager;
}

TraceWriter *
ComponentsGenerator::GenerateTraceWriter() {
    auto map = this->TruncateMap(K_TRACE_WRITER);
    TraceWriter *trace_writer;
    trace_writer = new SeismicTraceWriter(map);
    return trace_writer;
}

JSONConfigurationMap *
ComponentsGenerator::TruncateMap(const string &aComponentName) {
    auto map = this->GetWaveMap();
    map.merge_patch(this->mMap[aComponentName]);
    return new JSONConfigurationMap(map);
}

nlohmann::json ComponentsGenerator::GetWaveMap() {
    nlohmann::json map;
    map[K_WAVE][K_APPROXIMATION] = this->mApproximation;
    map[K_WAVE][K_SAMPLING] = this->mSampling;
    map[K_WAVE][K_EQUATION_ORDER] = this->mOrder;
    return map;
}
