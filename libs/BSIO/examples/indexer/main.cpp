/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <bs/io/utils/timer/ExecutionTimer.hpp>
#include <bs/io/indexers/FileIndexer.hpp>
#include <bs/io/indexers/IndexMap.hpp>


using namespace std;
using namespace bs::io::indexers;
using namespace bs::io::dataunits;
using namespace bs::io::utils::timer;


int main(int argc, char *argv[]) {
    vector<TraceHeaderKey> vec = {TraceHeaderKey::FLDR};
    string file_path = DATA_PATH "/shots0601_0800.segy";
    std::string key = "FLDR";
    FileIndexer fi(file_path, key);
    fi.Initialize();

    ExecutionTimer::Evaluate([&]() {
        fi.Index(vec);
    }, true);
}
