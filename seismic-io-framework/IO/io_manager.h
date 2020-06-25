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

  virtual ~IOManager(){};

  virtual void ReadTracesDataFromFile(string file_name, string sort_type,
                                      SeIO *sio) = 0;

  virtual void ReadVelocityDataFromFile(string file_name, string sort_type,
                                        SeIO *sio) = 0;

  virtual void ReadDensityDataFromFile(string file_name, string sort_type,
                                       SeIO *sio) = 0;

  // how we would like to pass the conditions for selective read ??
  virtual void ReadSelectiveDataFromFile(string file_name, string sort_type,
                                         SeIO *sio, int cond) = 0;

  virtual void WriteTracesDataToFile(string file_name, string sort_type,
                                     SeIO *sio) = 0;

  virtual void WriteVelocityDataToFile(string file_name, string sort_type,
                                       SeIO *sio) = 0;

  virtual void WriteDensityDataToFile(string file_name, string sort_type,
                                      SeIO *sio) = 0;

  virtual vector<uint> GetUniqueOccurences(string file_name, string key_name, uint min_threshold, uint max_threshold) = 0;

  // virtual void ReadTraceMetaData() = 0;

  //   virtual void ReadTracesData() = 0 ;
};

#endif // IO_FRAMEWORK_MANAGER_H
