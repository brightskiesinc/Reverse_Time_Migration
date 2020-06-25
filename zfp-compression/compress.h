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

namespace zfp {
static std::string write_path = ".";
// Supporting function for creating compressed filename
// concatenating the given model name and timestep
void setPath(std::string path);
char *getFileName(const char *model, unsigned int curTimestep);
char *getBlockFileName(const char *filename, unsigned int blockId);
size_t compressZFP_Parallel_1d(float *array, size_t nz, double tolerance,
                               char *filename, bool zfp_is_relative);
size_t compressZFP_Parallel(float *array, int nx, int ny, int nz,
                            double tolerance, char *filename, int decompress,
                            bool zfp_is_relative);
size_t decompressZFP_Parallel_1d(float *array, size_t nz, double tolerance,
                                 char *filename, int decompress,
                                 bool zfp_is_relative);
size_t decompressZFP_Parallel(float *array, int nx, int ny, int nz,
                              double tolerance, char *filename, int decompress,
                              bool zfp_is_relative);
size_t applyZFPOperation(float *array, int nx, int ny, int nz, double tolerance,
                         char *filename, int decompress, bool zfp_is_relative);
void no_compression_save(const char *fname, const float *data,
                         const size_t size);
void no_compression_load(const char *fname, float *data, const size_t size);
void compression(float *array, int nx, int ny, int nz, double tolerance,
                 unsigned int codecType, unsigned int curTimestep,
                 size_t *resultSize, const char *inputModel,
                 bool zfp_is_relative);
void decompression(float *array, int nx, int ny, int nz, double tolerance,
                   unsigned int codecType, unsigned int curTimestep,
                   size_t *resultSize, const char *inputModel,
                   bool zfp_is_relative);
} // namespace zfp

#endif