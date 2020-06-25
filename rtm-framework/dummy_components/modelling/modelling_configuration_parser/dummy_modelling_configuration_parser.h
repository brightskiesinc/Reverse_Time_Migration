//
// Created by amr on 12/11/2019.
//

#ifndef RTM_FRAMEWORK_DUMMYMODELLINGCONFIGURATIONPARSER_H
#define RTM_FRAMEWORK_DUMMYMODELLINGCONFIGURATIONPARSER_H

#include <skeleton/components/modelling/modelling_configuration_parser.h>

class DummyModellingConfigurationParser : public ModellingConfigurationParser {
public:
  /*!
   * Modelling configuration parser destroyer for correct destruction and is
   * overridden here.
   */
  ~DummyModellingConfigurationParser() override;
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
      *Example: this function may be used to read the file modelling.txt and
     parse it
      * in a proper format to the struct ModellingConfiguration
      */
  /*!this function is a virtual function and will be overridden here*/
  ModellingConfiguration ParseConfiguration(string filepath,
                                            bool is_2D) override;

  /*!
   * Sets the computation parameters to be used for the component.
   * @param parameters:Parameters of the simulation independent from the grid.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetComputationParameters(ComputationParameters *parameters) override;

  /*!
   * Sets the grid box to operate on.
   * @param grid_box
   * The designated grid box to run operations on.
   */
  /*!this function is a virtual function and will be overridden here*/
  void SetGridBox(GridBox *grid_box) override;
};
#endif // RTM_FRAMEWORK_DUMMYMODELLINGCONFIGURATIONPARSER_H
