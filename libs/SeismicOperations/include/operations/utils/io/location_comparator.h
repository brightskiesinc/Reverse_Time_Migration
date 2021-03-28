//
// Created by zeyad-osama on 11/01/2021.
//

#ifndef OPERATIONS_LIB_UTILS_READ_LOCATION_COMPARATOR_H
#define OPERATIONS_LIB_UTILS_READ_LOCATION_COMPARATOR_H

#include <seismic-io-framework/datatypes.h>

namespace operations {
    namespace utils {
        namespace io {
            bool compare_location_by_source(const GeneralTraces &a, const GeneralTraces &b);

            bool compare_location_by_receiver(const GeneralTraces &a, const GeneralTraces &b);
        } //namespace io
    } //namespace utils
} //namespace operations

#endif //OPERATIONS_LIB_UTILS_READ_LOCATION_COMPARATOR_H
