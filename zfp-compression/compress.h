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


void compressZFP_Parallel(float *array, int nx, int ny, int nz, int nt,
                            double tolerance, FILE *file, bool zfp_is_relative);

void decompressZFP_Parallel(float *array, int nx, int ny, int nz, int nt,
                              double tolerance, FILE *file, bool zfp_is_relative);
void applyZFPOperation(float *array, int nx, int ny, int nz, int nt,
                         FILE *file, int decompress, bool zfp_is_relative, double tolerance);
void no_compression_save(FILE *file, const float *data, int nx, int ny, int nz, int nt);
void no_compression_load(FILE *file, float *data, int nx, int ny, int nz, int nt);

void do_compression_save(float *array, int nx, int ny, int nz, int nt, double tolerance,
                 unsigned int codecType, const char *filename,
                 bool zfp_is_relative);
void do_decompression_load(float *array, int nx, int ny, int nz, int nt, double tolerance,
                   unsigned int codecType, const char *filename,
                   bool zfp_is_relative);

#endif
