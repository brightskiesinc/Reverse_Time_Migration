//
// Created by mirna-moawad on 1/16/20.
//

#ifndef OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H
#define OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H

#include <operations/common/ComputationParameters.hpp>
#include <operations/data-units/concrete/holders/GridBox.hpp>

namespace operations {
    namespace components {
        namespace helpers {

            void save_boundaries(dataunits::GridBox *apGridBox, common::ComputationParameters *apParameters,
                                 float *backup_boundaries, uint step, uint boundary_size);

            void restore_boundaries(dataunits::GridBox *apMainGrid, dataunits::GridBox *apInternalGrid,
                                    common::ComputationParameters *apParameters,
                                    const float *backup_boundaries, uint step, uint boundary_size);

        }//namespace helpers
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_BOUNDARY_SAVER_H
