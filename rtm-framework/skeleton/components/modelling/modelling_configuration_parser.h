//
// Created by amr on 12/11/2019.
//

#ifndef RTM_FRAMEWORK_MODELLING_CONFIGURATION_PARSER_H
#define RTM_FRAMEWORK_MODELLING_CONFIGURATION_PARSER_H

#include <skeleton/base/datatypes.h>
#include <skeleton/components/component.h>

#include <string>

using namespace std;

class ModellingConfigurationParser : public Component {
public:
  /*!
   * Modelling configuration parser destroyer for correct destruction.
   */
  virtual ~ModellingConfigurationParser(){};
  /*!
   * Parses a file with the proper format as the modelling configuration.
   * @param filepath
   * The file to be parsed.
   * @return
   * The parsed modelling configuration.
   * ModellingConfiguration:Parameters needed for the modelling operation.
          typedef struct {
              // Starting point for the receivers.
              Point3D receivers_start;
              // The increment step of the receivers.
              Point3D receivers_increment;
              // The end point of the receivers exclusive.
              Point3D receivers_end;
              // The source point for the modelling.
              Point3D source_point;
              // The total time of the simulation in seconds.
              float total_time;
           } ModellingConfiguration;
   *Example: this function may be used to read the file modelling.txt and parse
   it
   * in a proper format to the struct ModellingConfiguration
   */
  virtual ModellingConfiguration ParseConfiguration(string filepath,
                                                    bool is_2D) = 0;
};

#endif // RTM_FRAMEWORK_MODELLING_CONFIGURATION_PARSER_H
