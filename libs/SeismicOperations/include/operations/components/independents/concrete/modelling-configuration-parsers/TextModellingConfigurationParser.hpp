//
// Created by amr-nasr on 13/11/2019.
//

#ifndef OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_TEXT_MODELLING_CONFIGURATION_PARSER_HPP
#define OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_TEXT_MODELLING_CONFIGURATION_PARSER_HPP

#include <operations/components/independents/primitive/ModellingConfigurationParser.hpp>
#include <operations/components/dependency/concrete/HasNoDependents.hpp>


/**
 * @note
 * The file format should be as following :
 *      - source=(x,y,z)
 *      - receiver_start=(x,y,z)
 *      - receiver_inc=(x,y,z)
 *      - receiver_end=(x,y,z)
 *      - simulation_time=time
 *
 * With x, y, z, value and time replaced with integer values indicating the
 * actual values for these parameters.
 */

namespace operations {
    namespace components {

        class TextModellingConfigurationParser :
                public ModellingConfigurationParser, public dependency::HasNoDependents {
        public:
            ~TextModellingConfigurationParser() override;

            ModellingConfiguration ParseConfiguration(std::string filepath,
                                                      bool is_2D) override;

            void SetComputationParameters(common::ComputationParameters *apParameters) override;

            void SetGridBox(dataunits::GridBox *apGridBox) override;

            void AcquireConfiguration() override;

        private:
            common::ComputationParameters *mpParameters = nullptr;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_TRACE_WRITERS_TEXT_MODELLING_CONFIGURATION_PARSER_HPP
