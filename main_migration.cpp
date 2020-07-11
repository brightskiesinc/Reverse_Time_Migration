// This should contain the main function that drives the RTM acoustic 2nd order
// wave equation execution.
#include <cmd_parser_utils.h>
#include <parsers/callback_parser.h>
#include <parsers/configuration_parser.h>
#include <parsers/parameter_parser.h>
#include <skeleton/base/engine_configuration.h>
#include <skeleton/engine/rtm_engine.h>
#include <write_utils.h>
#include <noise_filtering.h>

int main(int argc, char *argv[]) {
  string parameter_file = WORKLOAD_PATH "/computation_parameters.txt";
  string configuration_file = WORKLOAD_PATH "/rtm_configuration.txt";
  string callback_file = WORKLOAD_PATH "/callback_configuration.txt";
  string write_path = WRITE_PATH;
  string message = "perform reverse time migration";
  string line;
  ifstream myfile (write_path + "/timing_results.txt");
  parse_args_engine(parameter_file, configuration_file, callback_file,
                    write_path, argc, argv, message.c_str());
  ComputationParameters *p = ParseParameterFile(parameter_file);
  EngineConfiguration *rtm_configuration =
      parse_rtm_configuration(configuration_file, write_path);
  CallbackCollection *cbs = parse_callbacks(callback_file, write_path);
  RTMEngine engine(rtm_configuration, p, cbs);
  MigrationData *mig;
  cout << "Detecting available shots for processing..." << std::endl;
  Timer *timer = Timer::getInstance();
  timer->start_timer("Engine::GetValidShots");
  vector<uint> possible_shots = engine.GetValidShots();
  timer->stop_timer("Engine::GetValidShots");
  if (possible_shots.empty()) {
      cout << "No valid shots detected...terminating." << std::endl;
      exit(0);
  }
  cout << "Valid shots detected to process : " << possible_shots.size() << std::endl;
  mig = engine.Migrate(possible_shots);
  float *filtered_migration = new float[mig->nx * mig->nz * mig->ny];
  timer->start_timer("Engine::FilterMigration");
  filter_stacked_correlation(mig->stacked_correlation, filtered_migration,
          mig->nx, mig->nz, mig->ny,
          mig->dx, mig->dz, mig->dy);
  timer->stop_timer("Engine::FilterMigration");

  WriteSegy(mig->nx, mig->nz, mig->nt, mig->ny, mig->dx, mig->dz, mig->dt,
            mig->dy, filtered_migration,
            write_path + "/filtered_migration.segy", false);

  WriteSegy(mig->nx, mig->nz, mig->nt, mig->ny, mig->dx, mig->dz, mig->dt,
              mig->dy, mig->stacked_correlation,
              write_path + "/raw_migration.segy", false);

  WriteBinary(filtered_migration,mig->nx, mig->nz, (write_path + "/filtered_migration.bin").c_str());
  WriteBinary(filtered_migration,mig->nx, mig->nz, (write_path + "/raw_migration.bin").c_str());

  delete[] filtered_migration;
  cout <<endl<<"Timings of the application are: "<<endl;
  cout <<"------------------------------"<<endl;
  timer->export_to_file(write_path + "/timing_results.txt",1);
  delete rtm_configuration;
  delete cbs;
  delete p;
  return 0;
}
