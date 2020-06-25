#ifndef SEGY_IO_MANAGER_H
#define SEGY_IO_MANAGER_H

#include "../IO/io_manager.h"
#include "../datatypes.h"
#include "math.h"
#include "susegy.h"

inline float dBtoscale(float x) {
  int i = x / abs(x);

  return pow(10, i * log10(abs(x)));
}

class SEGYIOManager : public IOManager {

public:
  SEGYIOManager();

  ~SEGYIOManager();

  void ReadTracesDataFromFile(string file_name, string sort_type,
                              SeIO *sio) override;

  void ReadVelocityDataFromFile(string file_name, string sort_type,
                                SeIO *sio) override;

  void ReadDensityDataFromFile(string file_name, string sort_type,
                               SeIO *sio) override;
  //  passign the condtions , how would like to pass the parameters ???
  // currently will take hsoy id as cond , later will be more general
  void ReadSelectiveDataFromFile(string file_name, string sort_type, SeIO *sio,
                                 int cond) override;

  void WriteTracesDataToFile(string file_name, string sort_type,
                             SeIO *sio) override;

  void WriteVelocityDataToFile(string file_name, string sort_type,
                               SeIO *sio) override;

  void WriteDensityDataToFile(string file_name, string sort_type,
                              SeIO *sio) override;

  vector<uint> GetUniqueOccurences(string file_name, string key_name, uint min_threshold, uint max_threshold) override;
  // do we need structure for binary header , traces header or we should cancel
  // these functions ??
};

#endif // SEGY_IO_MANAGER_H
