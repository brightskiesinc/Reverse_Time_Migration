//
// Created by mirna-moawad on 1/16/20.
//

#ifndef OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_FILE_HANDLER_H
#define OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_FILE_HANDLER_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace operations {
    namespace components {
        namespace helpers {

            void bin_file_save(const char *file_name, const float *data, const size_t &size);

            void bin_file_load(const char *file_name, float *data, const size_t &size);

        }//namespace helpers
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTORS_FILE_HANDLER_H
