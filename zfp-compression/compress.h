#ifndef COMPRESS_H
#define COMPRESS_H

#include "omp.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#ifdef ZFP_COMPRESSION
#include <zfp.h>
#endif

#define ZFP_MAX_PAR_BLOCKS 512
#define MINBLOCKSIZE 4
#define MINBLOCKSIZE1D (1024 * 1024)


size_t compressZFP_Parallel(float *array, int nx, int ny, int nz,
                            double tolerance, FILE *file, int decompress,
                            bool zfp_is_relative);

size_t decompressZFP_Parallel(float *array, int nx, int ny, int nz,
                              double tolerance, FILE *file, int decompress,
                              bool zfp_is_relative);
size_t applyZFPOperation(float *array, int nx, int ny, int nz, double tolerance,
                         FILE *file, int decompress, bool zfp_is_relative);
void no_compression_save(FILE *file, const float *data,
                         const size_t size);
void no_compression_load(FILE *file, float *data, const size_t size);

void do_compression_save(float *array, int nx, int ny, int nz, int nt, double tolerance,
                 unsigned int codecType, const char *filename,
                 bool zfp_is_relative);
void do_decompression_load(float *array, int nx, int ny, int nz, int nt, double tolerance,
                   unsigned int codecType, const char *filename,
                   bool zfp_is_relative);

#endif
