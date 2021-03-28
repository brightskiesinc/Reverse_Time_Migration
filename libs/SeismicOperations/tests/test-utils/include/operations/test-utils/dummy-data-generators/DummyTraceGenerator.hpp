//
// Created by zeyad-osama on 07/02/2021.
//

#ifndef OPERATIONS_LIB_TEST_UTILS_DUMMY_TRACE_GENERATOR_HPP
#define OPERATIONS_LIB_TEST_UTILS_DUMMY_TRACE_GENERATOR_HPP

#include <operations/data-units/concrete/holders/GridBox.hpp>

namespace operations {
    namespace testutils {

        float *generate_dummy_trace(const std::string &aFileName,
                                    dataunits::GridBox *apGridBox,
                                    int trace_stride_x,
                                    int trace_stride_y);

    } //namespace testutils
} //namespace operations

#endif //OPERATIONS_LIB_TEST_UTILS_DUMMY_TRACE_GENERATOR_HPP
