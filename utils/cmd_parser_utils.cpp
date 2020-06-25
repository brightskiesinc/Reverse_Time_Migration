#include <stdio.h>
#include <string>
#include <unistd.h>

using namespace std;

void print_help(char *argv[], const char *message) {
  char help_message[] =
      "Optional flags : \n"
      "\t-m <workload Path>: choose path to load workload configurations from, "
      "by default: ./workloads/bp_model\n"
      "\t-p <computation parameter file> : the file to parse for computation "
      "parameters eg: ./workloads/bp_model/computation_parameters.txt\n"
      "\t-s <system configuration file> : the file to parse for system "
      "configuration eg: ./workloads/bp_model/rtm_configuration.txt\n"
      "\t-c <callback configuration file> : the file to parse for callback "
      "configuration eg: ./workloads/bp_model/callback_configuration.txt\n"
      "\t-w <write path> : the path to write the results to eg : ./results\n"
      "\t-h : print the options for this command\n";
  printf("%s - %s\n%s", argv[0], message, help_message);
}

void parse_args_engine(string &parameter_file, string &configuration_file,
                       string &callback_file, string &write_path, int argc,
                       char *argv[], const char *message) {
  int opt;
  int len = string(WORKLOAD_PATH).length();
  string new_workload_path;
  while ((opt = getopt(argc, argv, ":m:p:c:w:s:h")) != -1) {
    switch (opt) {
    case 'm':
      new_workload_path = string(optarg);
      parameter_file.replace(parameter_file.begin(), parameter_file.begin() + len, new_workload_path);
      configuration_file.replace(configuration_file.begin(), configuration_file.begin() + len, new_workload_path);
      callback_file.replace(callback_file.begin(), callback_file.begin() + len, new_workload_path);
      break;
    case 'p':
      parameter_file = string(optarg);
      break;
    case 'c':
      callback_file = string(optarg);
      break;
    case 's':
      configuration_file = string(optarg);
      break;
    case 'w':
      write_path = string(optarg);
      break;
    case 'h':
      print_help(argv, message);
      exit(0);
      break;
    case ':':
      printf("Option needs a value\n");
      print_help(argv, message);
      exit(0);
      break;
    case '?':
      printf("Invalid option entered...\n");
      print_help(argv, message);
      exit(0);
      break;
    }
  }
  printf("Using computation parameter file : %s\n", parameter_file.c_str());
  printf("Using system configuration file : %s\n", configuration_file.c_str());
  printf("Using callback configuration file : %s\n", callback_file.c_str());
  printf("Using write path : %s\n", write_path.c_str());
}
