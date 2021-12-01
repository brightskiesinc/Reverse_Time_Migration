/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <bs/base/api/cpp/BSBase.hpp>

using namespace bs::base::logger;


int main() {
    /* Creating the logger System. */
    LoggerSystem *ls = LoggerSystem::GetInstance();;

    /* Registering the needed loggers. */
    ls->RegisterLogger(new FileLogger("./Output1.txt"));
    ls->RegisterLogger(new ConsoleLogger());

    /* Configuring the logger. */
    ls->ConfigureLoggers("Logger1", FILE_CONSOLE);

    /* Testing different functionalities. */
    ls->Info()
            << "Hello I'm the First info string , it appears in file & console with the default format " << '\n';
    ls->Error()
            << "Hello I'm the First Error statement ,it appears in file & console with the default format " << '\n';
    ls->Critical()
            << "Hello I'm the First critical statement , it appears in file & console with the default format " << '\n';
    ls->Debug()
            << "Hello I'm the First Debug statement , it appears in file & console with the default format " << '\n';

    /* Registering a  new output file. */
    ls->RegisterLogger(new FileLogger("./Output2.txt"));
    /* Configuring the new logger. */
    ls->ConfigureLoggers("Ostream", FILE_MODE);

    /* Testing different functionalities. */
    ls->Info() << "Hello I'm the Ostream info string , it appears in file format only " << '\n';
    ls->Error() << "Hello I'm the Ostream Error statement ,it appears in file format only" << '\n';
    ls->Critical() << "Hello I'm the Ostream critical statement , it appears in file format only"
                   << '\n';
    ls->Debug() << "Hello I'm the Ostream Debug statement , it appears in file format only" << '\n';


    /* Configuring the new logger. */
    ls->ConfigureLoggers("Console", CONSOLE_MODE);

    /* Testing different functionalities. */
    ls->Info() << "Hello I'm the Console info string , it appears in Console format only " << '\n';
    ls->Error() << "Hello I'm the Console Error statement ,it appears in Console format only" << '\n';
    ls->Critical() << "Hello I'm the Console critical statement , it appears in Console format only"
                   << '\n';
    ls->Debug() << "Hello I'm the Console Debug statement , it appears in Console format only" << '\n';

    return EXIT_SUCCESS;
}
