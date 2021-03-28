//
// Created by marwan on 11/01/2021.
//

#ifndef OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_GRID_BOX_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_GRID_BOX_GENERATOR_HPP

#include <operations/data-units/concrete/holders/GridBox.hpp>
#include <operations/test-utils/TestEnums.hpp>

namespace operations {
    namespace testutils {

        dataunits::GridBox *generate_grid_box(OP_TU_DIMS aDims, OP_TU_WIND aWindow);

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_DUMMY_DATA_GENERATORS_DUMMY_GRID_BOX_GENERATOR_HPP
