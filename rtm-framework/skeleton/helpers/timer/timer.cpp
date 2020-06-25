#include "timer.hpp"
/*

TIMER DOCUMENTATION:


Usage:


        1) Building the timer:

                if you want to use the timer in your cpp file, let's call it
sample.cpp, you would issue the following command to build the application:
                                mpiicpc -o sample -std=c++11 sample.cpp
timer.cpp -qopenmp note that any application using the timer needs to be built
with mpiicpc because it supports timing multiple processes using the MPI
framework. Make sure that you also include "timer.hpp" in your application.

        2) Using the timer:

                The timer follows the singleton design pattern, the object can
be initialized anywhere in the code and it will remmember the data initialized
in an older object. this is how the timer is initialized for single process use:

                        Timer* t = Timer::getInstance();

                for multiple processes it would be intialized like this:

                        Timer* t = Timer::getInstance(1);

                        -> Note that the multiple processes feature is not yet
fully tested Make sure that you point the pointer to the return value of the
static method getInstance(), merely initializing it like this: Timer* t is not
enough.

                To start timing a function the following method is called
t->start_timer(function_name) before the function call, then
t->stop_timer(function_name) after the function call. For example the following
code would be used to time a function called func1.

                        t->start_timer("func1");
                        func1();
                        t->stop_timer("func1");

                The string passed to the start_timer and stop_timer functions
doesn't have to be the real function name, any string would suffice but it is
generally reccomended to used the actual function read to understand the report
better. Just make sure that the strings passed to the start_timer function is
the same string passed to the stop_timer function when you intend to stop that
timer.

                Retreiving the timing report can be done in 3 ways, printing to
std::cout, exporting to a file or returning the report as a string, the
following three functions do that:

                        t->print_report();
                        t->export_to_file(filename);
                        t->get_report();

                If you want to print the report data in scientific notation
instead of its current format then line 184 should be modified, remove
"std::fixed" from the stream. The precision of the numbers


 */

Timer *Timer::getInstance(
    int flag) // Pass 1 if timer is intenden to run on multiprocesses
{

  static Timer TimerInstance;
  TimerInstance.multip = 0;

  TimerInstance.prank = 0;
  TimerInstance.pcount = 1;

  return &TimerInstance;
}

Data *DataObj_init() {

  Data *dataObj = new Data();
  dataObj->runtimes = std::vector<double>();
  dataObj->lineNums = std::vector<int>();
  return dataObj;
}

void Timer::_start_timer(std::string function_name, int line) {
  std::unordered_map<std::string, double>::iterator it_running =
      Running_Times.find(function_name);
  std::unordered_map<std::string, Data *>::iterator it =
      Timing_dict.find(function_name);

  if (it_running != Running_Times.end()) {
    std::cerr << "A timer is already running for the function " << function_name
              << std::endl;
    exit(1);
  }
  if (it == Timing_dict.end())
    Timing_dict[function_name] = DataObj_init();

#ifdef _OPENMP
  double start = omp_get_wtime();
#else
  timeval start_time;
  gettimeofday(&start_time, NULL);

  double start = start_time.tv_usec + start_time.tv_sec * 1000000;
  start /= 1000000;
#endif
  Running_Times[function_name] = start;
  Timing_dict[function_name]->lineNums.push_back(line);
}

void Timer::_start_timer_for_kernel(std::string function_name, double size,
                                    int arrays, bool single,
                                    int num_of_operations) {
  std::unordered_map<std::string, double>::iterator it_running =
      Running_Times.find(function_name);
  std::unordered_map<std::string, Data *>::iterator it =
      Timing_dict.find(function_name);
  if (it_running != Running_Times.end()) {
    std::cerr << "A timer is already running for the function " << function_name
              << std::endl;
    exit(1);
  }
  if (it == Timing_dict.end())
    Timing_dict[function_name] = DataObj_init();

#ifdef _OPENMP
  double start = omp_get_wtime();
#else
  timeval start_time;
  gettimeofday(&start_time, NULL);

  double start = start_time.tv_usec + start_time.tv_sec * 1000000;
  start /= 1000000;
#endif
  Running_Times[function_name] = start;
  // Timing_dict[function_name]->lineNums.push_back(line);
  Timing_dict[function_name]->size_of_grid = size;
  Timing_dict[function_name]->num_of_arrays = arrays;
  Timing_dict[function_name]->num_of_opr = num_of_operations;
  if (single) {
    Timing_dict[function_name]->size_of_data = 4.0f * size * arrays;
    Timing_dict[function_name]->points = 4.0f * arrays;
  } else {
    Timing_dict[function_name]->size_of_data = 8.0f * size * arrays;
    Timing_dict[function_name]->points = 8.0f * arrays;
  }
  Timing_dict[function_name]->kernel = true;
}

void Timer::stop_timer(std::string function_name) {
  std::unordered_map<std::string, Data *>::iterator it =
      Timing_dict.find(function_name);
  std::unordered_map<std::string, double>::iterator it_running =
      Running_Times.find(function_name);

  if (it_running == Running_Times.end()) {
    std::cerr << "Timer didn't start..." << std::endl;
    exit(1);
  }
  double start = Running_Times[function_name];
#ifdef _OPENMP
  double end = omp_get_wtime();
#else
  timeval end_time;
  gettimeofday(&end_time, NULL);

  double end = end_time.tv_usec + end_time.tv_sec * 1000000;
  end /= 1000000;
#endif
  double duration = end - start;
  Timing_dict[function_name]->runtimes.push_back(duration);
  Timing_dict[function_name]->max =
      std::max(duration, Timing_dict[function_name]->max);
  Timing_dict[function_name]->min =
      std::min(duration, Timing_dict[function_name]->min);

  Running_Times.erase(it_running);
}

int Timer::active_timers(int i = 0) {

  if (i == 1) {
    std::cout << "Active Timers: " << std::endl;
    for (auto element : Running_Times) {
#ifdef _OPENMP
      double end = omp_get_wtime();
#else
      timeval end_time;
      gettimeofday(&end_time, NULL);
      double end = end_time.tv_usec + end_time.tv_sec * 1000000;
      end /= 1000000;
#endif
      double duration = end - element.second;
      std::cout << "Function Name: " << element.first << ", Started "
                << duration << " seconds ago at line "
                << Timing_dict[element.first]->lineNums.back() << std::endl;
    }
  }
  return Running_Times.size();
}

double get_average(std::vector<double> vec) {

  double total = 0;
  for (double element : vec)
    total += element;
  return total / vec.size();
}

double get_total(std::vector<double> vec) {

  double total = 0;
  for (double element : vec)
    total += element;
  return total;
}

std::string get_funcData(Data *dataObj) {

  std::stringstream os;
#if DETAILED_TIMER == 1
  for (int i = 0; i < dataObj->runtimes.size(); i++) {
#if TIMER_HUMAN_READABLE == 1
    os << "run " << i << " at line " << std::setprecision(precision)
       << std::fixed << dataObj->lineNums[i] << ": " << dataObj->runtimes[i]
       << "\n";
    //#else
    //            os << dataObj->runtimes[i] << "\n";
#endif
  }
#endif
  os << "Minimum Runtime: " << dataObj->min << " secs"
     << "\n";
  os << "Maximum Runtime: " << dataObj->max << " secs"
     << "\n";
  os << "Average Runtime: " << get_average(dataObj->runtimes) << " secs"
     << "\n";
  os << "Total Runtime: " << get_total(dataObj->runtimes) << " secs"
     << "\n";
  float giga = (1024 * 1024 * 1024);
  float mega = (1024 * 1024);
  if (dataObj->kernel) {
    os << "Grid size: " << dataObj->size_of_grid / mega << " Mpts"
       << "\n";
    os << "Minimum Throughput: " << dataObj->size_of_grid / dataObj->max / mega
       << " Mpts/s"
       << "\n";
    os << "Maximum Throughput: " << dataObj->size_of_grid / dataObj->min / mega
       << " Mpts/s"
       << "\n";
    os << "Average Throughput: "
       << dataObj->size_of_grid / get_average(dataObj->runtimes) / mega
       << " Mpts/s"
       << "\n";
    os << "Size of data transfered per single execution: " << dataObj->size_of_data / mega << " Mpts"
       << "\n";
    os << "Minimum Bandwidth: " << dataObj->size_of_data / dataObj->max / giga
       << " GBytes/s"
       << "\n";
    os << "Maximum Bandwidth: " << dataObj->size_of_data / dataObj->min / giga
       << " GBytes/s"
       << "\n";
    os << "Average Bandwidth: "
       << dataObj->size_of_data / get_average(dataObj->runtimes) / giga
       << " GBytes/s"
       << "\n";
    os << "Minimum GFlops: "
       << ((dataObj->size_of_grid) * dataObj->num_of_opr) / dataObj->max / giga
       << " GFlops"
       << "\n";
    os << "Maximum GFlops: "
       << ((dataObj->size_of_grid) * dataObj->num_of_opr) / dataObj->min / giga
       << " GFlops"
       << "\n";
    os << "Average GFlops: "
       << ((dataObj->size_of_grid) * dataObj->num_of_opr) /
              get_average(dataObj->runtimes) / giga
       << " GFlops"
       << "\n";
  }
  os << "Number of calls: " << dataObj->runtimes.size() << "\n";
  return os.str();
}

std::string Timer::get_report() {

  std::ostringstream os;

  for (auto entry : Timing_dict) {
    os << "Function name: " << entry.first << "\n";
    os << get_funcData(entry.second);
    os << "\n";
  }
  return os.str();
}

std::string get_report_helper(std::map<std::string, Data *> Timing_dict) {

  std::ostringstream os;

  for (auto entry : Timing_dict) {
    os << "Function name: " << entry.first << "\n";
    os << get_funcData(entry.second);
    os << "\n";
  }
  return os.str();
}
// void send_str(int prank, std::string s)
//{
//        int len = (int)s.length();
//        MPI_Send(&len, 1, MPI_INT, 0, prank * 10, MPI_COMM_WORLD);
//        MPI_Send(s.data(), len, MPI_CHAR, 0, prank, MPI_COMM_WORLD);
//}
//
// std::string recv_str(int recv_rank)
//{
//
//        int len;
//        std::string s;
//        MPI_Recv(&len, 1, MPI_INT, recv_rank, recv_rank * 10, MPI_COMM_WORLD,
//        MPI_STATUS_IGNORE); std::vector<char> tmp(len); MPI_Recv(tmp.data(),
//        len, MPI_CHAR, recv_rank, recv_rank, MPI_COMM_WORLD,
//        MPI_STATUS_IGNORE); s.assign(tmp.begin(), tmp.end()); return s;
//
//}

// Function that takes the report as a string and returns a map containing all
// the information used to collect information from other processes aftewr
// failing to serialize the map and send it over MPI IF YOU CHANGE THE REPORT
// OUTPIUT THIS FUNCTION WILL NOT WORK AND WILL HAVE TO BE REIMPLENETED DO NOT
// CHANGE THE REPORT OUTPUT

std::map<std::string, Data *> parse_str(std::string report) {

  std::map<std::string, Data *> result_map;
  std::istringstream ss(report);
  std::string current_func_name;
  bool collecting = false;
  bool collect = false;
  std::vector<double> result_vec;
  std::vector<int> lineNums;
  Data *data;
  std::string line;
  while (std::getline(ss, line)) {
    if (line == "")
      break;
    if (line.substr(0, 8) == "Function") {
      current_func_name = line.substr(15, line.size() - 15);
      data = new Data();
      collecting = true;
      continue;
    }
    if (collecting && line.substr(0, 3) == "run") {
      // Parsing the running time
      std::size_t index = line.find(":");
      int numindex = (int)index + 2;
      std::string numstr = line.substr(numindex, line.size() - numindex);
      result_vec.push_back(stod(numstr));

      // Parsing the line number
      std::size_t line_marker_index = line.find("line");
      int line_index = (int)line_marker_index + 5;
      int line_len = (int)index - line_index;
      std::string line_str = line.substr(line_index, line_len);
      lineNums.push_back(stoi(line_str));
      collect = true;

    } else if (collect) {
      double avg = get_average(result_vec);
      double max = INT_MIN;
      double min = INT_MAX;
      for (auto element : result_vec) {
        max = std::max(max, element);
        min = std::min(min, element);
      }
      for (int i = 0; i < result_vec.size(); i++)
        std::cout << result_vec[i] << ", ";
      std::cout << std::endl;
      data->runtimes = result_vec;
      data->lineNums = lineNums;
      data->max = max;
      data->min = min;
      data->average = avg;
      result_map[current_func_name] = data;
      current_func_name = "";
      collecting = false;
      result_vec.clear();
      lineNums.clear();
      collect = false;
    }
  }

  return result_map;
}

void Timer::print_report() {
  Timer_Finalize();
  if (prank == 0)
    std::cout << report << std::endl;
}

std::string Timer::return_report() {
  Timer_Finalize();
  return report;
}

void Timer::Timer_Finalize() {
  //	std::stringstream ss;
  ////	if(prank == 0)
  ////	{
  //		ss << "Process 0:\n" << get_report() << "\n";
  //		report.append(ss.str());
  //		ss.str("");
  report = get_report();
  //		for(int i = 1; i < pcount; i++)
  //		{
  //			std::string local_report = recv_str(i);
  //			process_data[i] = parse_str(local_report);
  //			std::cout << get_report_helper(process_data[i]) <<
  // std::endl;
  //
  //			ss << "Process " << i << ":" << "\n" << local_report <<
  //"\n"; 			report.append(ss.str());
  // ss.str("");
  //		}
  //	}
  //
  //	else
  //	{
  //		std::string rank_report = get_report();
  //		send_str(prank, rank_report);
  //	}
}

void Timer::Timer_Cleanup() {
  Timer *TimerInstance = Timer::getInstance();
  for (auto entry : TimerInstance->Timing_dict) {
    delete entry.second;
  }
  TimerInstance->Timing_dict.clear();
}

void Timer::export_to_file(std::string filename,bool print){
  if (prank == 0) {
    Timer_Finalize();
    std::ofstream outputFile;
    outputFile.open(filename);
    outputFile << report;
    outputFile.close();
    if (print)
    {
    std::cout << report << std::endl;
    }
  }
}

void func1() {

  //	for(unsigned long i = 0; i < 9999999999; i++);
  //	sleep(2);
}

void func2() {

  for (int i = 0; i < 998574; i++)
    ;
}

void func3() {
  Timer *t = Timer::getInstance();
  t->start_timer("func2");
  for (int i = 0; i < 996549; i++)
    ;
  t->stop_timer("func2");
  for (int i = 0; i < 89434; i++)
    ;
}
/*
int main_test()
{

        Timer* t1;
        t1 = Timer::getInstance();

        t1->start_timer("func1");
        func1();
        t1->stop_timer("func1");

        t1->start_timer("func2");
        func2();
        t1->stop_timer("func2");


        Timer* t;
        t = Timer::getInstance();
        double start = omp_get_wtime();
        t->start_timer("sleep");
        double end = omp_get_wtime();
        std::cout << "start_timer runtime: " << (end - start) << std::endl;
        sleep(2);
        //t->active_timers(1);
        double start2 = omp_get_wtime();
        t->stop_timer("sleep");
        double end2 = omp_get_wtime();
        std::cout << "stop_timer runtime: " << (end - start) << std::endl;





        t->start_timer("func3");
        func3();
        t->stop_timer("func3");
        //t->start_timer("func1");
        t->start_timer("func1");
        func1();
        //t->active_timers(1);
        t->stop_timer("func1");
//	t->print_report();

        std::string report = t->return_report();
        std::cout << report << std::endl;
        std::map<std::string, Data*> parsed_dict = parse_str(report);
        std::string nReport = get_report_helper(parsed_dict);
        std::cout << nReport << std::endl;
        t->export_to_file("Timer_Report1");


}
 */
