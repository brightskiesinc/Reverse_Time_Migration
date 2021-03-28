//
// Created by zeyad-osama on 07/03/2021.
//

#include <thoth/utils/timer/ExecutionTimer.hpp>
#include <thoth/indexers/FileIndexer.hpp>
#include <thoth/indexers/IndexMap.hpp>

#include <iostream>

using namespace std;
using namespace thoth::indexers;
using namespace thoth::dataunits;
using namespace thoth::utils::timer;


int main(int argc, char *argv[]) {
    vector<TraceHeaderKey> vec = {TraceHeaderKey::FLDR};
    string file_path = DATA_PATH "/shots0601_0800.segy";
    FileIndexer fi(file_path);
    fi.Initialize();

    ExecutionTimer::Evaluate([&]() {
        fi.Index(vec);
    }, true);
}
