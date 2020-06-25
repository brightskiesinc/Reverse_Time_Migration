#ifndef TIMER_HPP
#define TIMER_HPP
#include "limits.h"
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <unistd.h>


#include <sstream>
#include "timer.hpp"
#include <fstream>
#include <iomanip>
#ifdef _OPENMP
#include <omp.h>
#else
#include <sys/time.h>
#endif
#define start_timer(function_name) _start_timer(function_name, __LINE__)




struct Data
{

	std::vector<double> runtimes;
	std::vector<int> lineNums;
	double average;
	double max = static_cast<double>(INT_MIN);
	double min = static_cast<double>(INT_MAX);
	double size_of_grid;
	double size_of_data;
	double points;
	int num_of_arrays;
	int num_of_opr;
    bool kernel=false;
};
class Timer
{

private:

	static Timer* timer_singleton;

    // string as a key and vector as value to store the run times of the function 
	std::unordered_map<std::string, Data* > Timing_dict;
	
	std::unordered_map<std::string, double > Running_Times;

	// Map structure to hold the data of each process using the rank of that process
	std::map<int, std::map<std::string, Data*>> process_data;


	static bool instanceFlag;


	// stores the duration of the function being timed
	double runtime;

	// The function being currently timed, set in timer_start() and initialized to "" for error checking
	static std::string current_func;
	std::string get_report();
	std::string report;
public:	

	Timer()
	{
	}
	static Timer* getInstance(int flag=0);
	std::string return_report();
	void _start_timer(std::string function_name, int line);
    void _start_timer_for_kernel(std::string function_name, double size,int arrays,bool single, int num_of_operations);
	void stop_timer(std::string functon_name);
	void print_report();
	void export_to_file(std::string filename,bool print);
	void Print_results();
	void Timer_Finalize();
	static void Timer_Cleanup();
	int active_timers(int i);
	int multip;                     // 1 if multi processes, 0 otherwise
	int prank;			// rank of current process
	int pcount;			// process count
	//bool kernel=false;
};






#endif 
