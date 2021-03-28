#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H

#include "limits.h"
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <unistd.h>


#include <sstream>
#include "Timer.h"
#include <fstream>
#include <iomanip>

#ifdef _OPENMP

#include <omp.h>

#else

#include <sys/time.h>

#endif

#define StartTimer(function_name) _start_timer(function_name, __LINE__)


struct Data {

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
    bool kernel = false;
};

class Timer {
public:
    Timer() = default;

    static Timer *GetInstance(int flag = 0);

    std::string GetReport();

    void _start_timer(std::string function_name, int line);

    void StartTimerKernel(std::string function_name,
                          double size, int arrays, bool single, int num_of_operations);

    void StopTimer(std::string function_name);

    void PrintReport();

    void ExportToFile(std::string filename, bool print);

    void PrintResults();

    void Finalize();

    static void Cleanup();

    int ActiveTimers(int i);


private:
    /// Map structure to hold the data of each process using the rank of that process
    std::map<int, std::map<std::string, Data *>> process_data;

    /// Function being currently timed, set in timer_start() and initialized to "" for error checking
    static std::string current_func;

    std::string mGetReport();

private:
    static Timer *mInstance;

    static bool mInstanceFlag;

    /// String as a key and vector as value to store the run times of the function
    std::unordered_map<std::string, Data *> mTimingMap;

    std::unordered_map<std::string, double> mRunningTimesMap;

    /// Stores the duration of the function being timed
    double mRuntime;

    std::string mReport;

    /// 1 if multi processes, 0 otherwise
    int mMultipleProcesses;

    /// Rank of current process
    int mProcessRank;

    /// process count
    int mProcessCount;
};

#endif
