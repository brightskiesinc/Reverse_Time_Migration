//
// Created by mirnamoawad on 1/16/20.
//

#ifndef ACOUSTIC2ND_RTM_FILE_HANDLER_H
#define ACOUSTIC2ND_RTM_FILE_HANDLER_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>

void bin_file_save(const char *fname, const float *data, const size_t size);
void bin_file_load(const char *fname, float *data, const size_t size);

#endif // ACOUSTIC2ND_RTM_FILE_HANDLER_H
