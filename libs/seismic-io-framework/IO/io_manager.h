#ifndef IO_FRAMEWORK_MANAGER_H
#define IO_FRAMEWORK_MANAGER_H

#include "../datatypes.h"

#include <iostream>
#include <string>
#include <vector>

class IOManager {
public:
    /*!
     * De-constructors should be overridden to ensure correct memory management.

     */

    // auto *temp2 = new seisio::general_traces();

    //   virtual IOManager() = 0;

    virtual ~IOManager() {};

    virtual void ReadTracesDataFromFile(std::string file_name,
                                        std::string sort_type,
                                        SeIO *sio) = 0;

    virtual void ReadVelocityDataFromFile(std::string file_name,
                                          std::string sort_type,
                                          SeIO *sio) = 0;

    virtual void ReadDensityDataFromFile(std::string file_name,
                                         std::string sort_type,
                                         SeIO *sio) = 0;

    // how we would like to pass the conditions for selective read ??
    virtual void ReadSelectiveDataFromFile(std::string file_name,
                                           std::string sort_type,
                                           SeIO *sio, int cond) = 0;

    virtual void WriteTracesDataToFile(std::string file_name,
                                       std::string sort_type,
                                       SeIO *sio) = 0;

    virtual void WriteVelocityDataToFile(std::string file_name, std::string sort_type,
                                         SeIO *sio) = 0;

    virtual void WriteDensityDataToFile(std::string file_name, std::string sort_type,
                                        SeIO *sio) = 0;

    virtual std::vector<uint>
    GetUniqueOccurences(std::string file_name, std::string key_name, uint min_threshold, uint max_threshold) = 0;

    // virtual void ReadTraceMetaData() = 0;

    //   virtual void ReadTracesData() = 0 ;
};

#endif // IO_FRAMEWORK_MANAGER_H
