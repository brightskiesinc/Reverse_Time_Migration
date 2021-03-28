//
// Created by zeyad-osama on 07/03/2021.
//

#include <thoth/streams/concrete/writers/SegyWriter.hpp>
#include <thoth/configurations/concrete/JSONConfigurationMap.hpp>
#include <thoth/utils/timer/ExecutionTimer.hpp>

#include <libraries/nlohmann/json.hpp>

#include <iostream>

using namespace std;
using namespace thoth::streams;
using namespace thoth::configuration;
using namespace thoth::dataunits;
using namespace thoth::utils::timer;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
    json configuration_map;
    configuration_map[IO_K_PROPERTIES][IO_K_WRITE_LITTLE_ENDIAN] = false;

    std::string path = WRITE_PATH "/result.segy";
    std::vector<Gather *> gathers;

    SegyWriter w(new JSONConfigurationMap(configuration_map));
    w.AcquireConfiguration();
    w.Initialize(path);

    /* Get gather number */
    std::cout << std::endl << "Normal writing case:" << std::endl;
    ExecutionTimer::Evaluate([&]() {
        w.Write(gathers);
    }, true);

    /* Finalize and closes all opened internal streams. */
    w.Finalize();
}
