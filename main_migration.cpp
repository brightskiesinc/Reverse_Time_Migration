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

///
/// @brief This should contain the main function that drives the
/// Seismic Engine execution.
///

#include <stbx/agents/Agents.h>
#include <stbx/writers/Writers.h>
#include <stbx/parsers/Parser.hpp>
#include <stbx/parsers/ArgumentsParser.hpp>
#include <stbx/generators/Generator.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/base/logger/concrete/FileLogger.hpp>
#include <bs/base/logger/concrete/ConsoleLogger.hpp>

#include <bs/timer/api/cpp/BSTimer.hpp>

using namespace std;
using namespace stbx::parsers;
using namespace stbx::generators;
using namespace stbx::writers;
using namespace operations::dataunits;
using namespace operations::engines;
using namespace bs::base::logger;
using namespace bs::timer::configurations;


int main(int argc, char *argv[]) {
    string parameter_file = WORKLOAD_PATH "/computation_parameters.json";
    string configuration_file = WORKLOAD_PATH "/engine_configuration.json";
    string callback_file = WORKLOAD_PATH "/callback_configuration.json";
    string system_file = WORKLOAD_PATH "/system_configuration.json";
    string write_path = WRITE_PATH;

    auto logger = LoggerSystem::GetInstance();

    /* Registering the needed loggers. */
    logger->RegisterLogger(new FileLogger(write_path + "/log.txt"));
    logger->RegisterLogger(new ConsoleLogger());

    /* Configuring the logger. */
    logger->ConfigureLoggers("Main logger", FILE_CONSOLE, DATE_TIME);
    logger->Info() << "Starting Seismic Engine..." << '\n';

    ArgumentsParser::Parse(parameter_file,
                           configuration_file,
                           callback_file,
                           system_file,
                           write_path,
                           argc, argv);

    Parser *p = Parser::GetInstance();
    p->RegisterFile(parameter_file);
    p->RegisterFile(configuration_file);
    p->RegisterFile(callback_file);
    p->RegisterFile(system_file);
    p->BuildMap();

    auto g = new Generator(p->GetMap());
    auto engine = g->GenerateEngine(write_path);

    TimerManager::GetInstance()->Configure(g->GenerateTimerConfiguration());

    auto agent = g->GenerateAgent();
    agent->AssignEngine(engine);
    agent->AssignArgs(argc, argv);
    MigrationData *md = agent->Execute();

    auto writer = g->GenerateWriter();
    writer->AssignMigrationData(md);
    writer->Write(write_path);

    TimerManager::GetInstance()->Terminate(true);
    TimerManager::Kill();

    delete engine;

    return 0;
}