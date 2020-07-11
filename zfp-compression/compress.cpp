#include "compress.h"
#include <fstream>
#include <skeleton/helpers/timer/timer.hpp>

namespace zfp {

size_t compressZFP_Parallel(float *array, int nx, int ny, int nz,
                            double tolerance, FILE *file,
                            bool zfp_is_relative) {
  // this function is used for compression of a multidimensional array
  int status = 0;                       /* return value: 0 = success */
  zfp_type type;                        /* array scalar type */
  zfp_field *field[ZFP_MAX_PAR_BLOCKS]; /* array meta data */
  zfp_stream *zfp[ZFP_MAX_PAR_BLOCKS];  /* compressed stream */
  void *buffer[ZFP_MAX_PAR_BLOCKS];     /* storage for compressed stream */
  size_t bufsize[ZFP_MAX_PAR_BLOCKS];   /* byte size of compressed buffer */
  bitstream
      *stream[ZFP_MAX_PAR_BLOCKS];    /* bit stream to write to or read from */
  size_t zfpsize[ZFP_MAX_PAR_BLOCKS]; /* byte size of compressed stream */
  size_t resultSize = 0; /*for compression resultSize=zfpsize else nx*ny*nz */
  int blockDim[ZFP_MAX_PAR_BLOCKS];
  unsigned int totalBlocks = 0;

  totalBlocks =
      (nz + MINBLOCKSIZE - 1) /
      MINBLOCKSIZE; // calculating the number of blocks to be written, a block
                    // will consist of a number of rows in nx dimension
  size_t bytesWritten = 0;
  Timer *timer = Timer::getInstance();
  timer->start_timer("IO::ZFP::Compression::Header");
  bytesWritten = fwrite(&totalBlocks, 4, 1, file); // write the number of blocks to be written
  timer->stop_timer("IO::ZFP::Compression::Header");
  resultSize += 4;            // accumulate the size of the written data
  timer->start_timer("ZFP::Compress");
#pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks; block++) {
    // loop on blocks, each is compressed and then written
    type = zfp_type_float;

    if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
      blockDim[block] = ((block < (totalBlocks - 1))
                             ? MINBLOCKSIZE
                             : nz % MINBLOCKSIZE); // determine the size of a
                                                   // block before compression
      if (blockDim[block] == 0) {
        blockDim[block] = MINBLOCKSIZE;
      }
      field[block] = zfp_field_3d(&array[block * MINBLOCKSIZE * ny * nx], type,
                                  nx, ny, blockDim[block]);
    } else { // handle 2d
      blockDim[block] = ((block < (totalBlocks - 1))
                             ? MINBLOCKSIZE
                             : nz % MINBLOCKSIZE); // determine the size of a
                                                   // block before compression
      if (blockDim[block] == 0) {
        blockDim[block] = MINBLOCKSIZE;
      }
      field[block] = zfp_field_2d(&array[block * MINBLOCKSIZE * nx], type, nx,
                                  blockDim[block]);
    }
    /* allocate meta data for a compressed stream */
    zfp[block] = zfp_stream_open(NULL);

    if (zfp_is_relative) { // we are concerned with relative error (precision)
      zfp_stream_set_precision(zfp[block], tolerance);
    } else { // we are concerned with absolute error (accuracy)
      // zfp_stream_set_accuracy(zfp[block], tolerance, type); // Old ZFP
      // version
      zfp_stream_set_accuracy(zfp[block], tolerance); // New ZFP version
    }

    /* allocate buffer for compressed data */
    bufsize[block] = zfp_stream_maximum_size(zfp[block], field[block]);
    buffer[block] = malloc(bufsize[block]);

    /* associate bit stream with allocated buffer */
    stream[block] = stream_open(buffer[block], bufsize[block]);
    zfp_stream_set_bit_stream(zfp[block], stream[block]);
    zfp_stream_rewind(zfp[block]);

    /* compress entire array */
    zfpsize[block] = zfp_compress(zfp[block], field[block]);
    if (!zfpsize[block]) {
      fprintf(stderr, "compression failed\n");
      status = 1;
    }
  }

  timer->stop_timer("ZFP::Compress");
  timer->start_timer("IO::ZFP::Compression");
    // Write Block Sizes in serial
  for (int block = 0; block < totalBlocks; block++) {
    bytesWritten = fwrite(&zfpsize[block], sizeof(zfpsize[block]), 1, file); // write the size of the block after compression
    resultSize += sizeof(zfpsize[block]); // accumulate the written data size
    bytesWritten = fwrite(buffer[block], 1, zfpsize[block], file); // write the actual compressed data
    resultSize += zfpsize[block]; // accumulate the written data size
  }
  timer->stop_timer("IO::ZFP::Compression");
  /* clean up */
  for (int block = 0; block < totalBlocks; block++) {
    zfp_field_free(field[block]);
    zfp_stream_close(zfp[block]);
    stream_close(stream[block]);
    free(buffer[block]);
  }
  return resultSize;
}

size_t decompressZFP_Parallel(float *array, int nx, int ny, int nz,
                              double tolerance, FILE *file,
                              bool zfp_is_relative) {
  int status = 0;                       /* return value: 0 = success */
  zfp_type type;                        /* array scalar type */
  zfp_field *field[ZFP_MAX_PAR_BLOCKS]; /* array meta data */
  zfp_stream *zfp[ZFP_MAX_PAR_BLOCKS];  /* compressed stream */
  void *buffer[ZFP_MAX_PAR_BLOCKS];     /* storage for compressed stream */
  size_t bufsize[ZFP_MAX_PAR_BLOCKS];   /* byte size of compressed buffer */
  bitstream
      *stream[ZFP_MAX_PAR_BLOCKS];    /* bit stream to write to or read from */
  size_t zfpsize[ZFP_MAX_PAR_BLOCKS]; /* byte size of compressed stream */
  size_t resultSize = 0; /*for compression resultSize=zfpsize else nx*ny*nz */
  int blockDim[ZFP_MAX_PAR_BLOCKS];

  size_t resultSize_block[ZFP_MAX_PAR_BLOCKS] = {0};

  unsigned int totalBlocks = 0;

  Timer *timer = Timer::getInstance();
  timer->start_timer("IO::ZFP::Decompression::Header");
  fread(&totalBlocks, 4, 1, file);
  timer->stop_timer("IO::ZFP::Decompression::Header");
  timer->start_timer("ZFP::Decompression::SetupProperties");
#pragma omp parallel for schedule(static)
  // Boilerplate ZFP setup in serial
  for (unsigned int block = 0; block < totalBlocks; block++) {

    /* allocate meta data for the 3D array a[nz][ny][nx] */

    type = zfp_type_float;

    // blockDim[block] = ((block < (numThreads - 1)) ? nz/numThreads :
    // (nz/numThreads) + (nz % numThreads));

    if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
      blockDim[block] = ((block < (totalBlocks - 1))
                             ? MINBLOCKSIZE
                             : nz % MINBLOCKSIZE); // determine the size of a
                                                   // block before compression
      if (blockDim[block] == 0) {
        blockDim[block] = MINBLOCKSIZE;
      }
      field[block] = zfp_field_3d(&array[block * MINBLOCKSIZE * ny * nx], type,
                                  nx, ny, blockDim[block]);
    } else { // handle 2d
      blockDim[block] =
          ((block < (totalBlocks - 1)) ? MINBLOCKSIZE : nz % MINBLOCKSIZE);
      if (blockDim[block] == 0) {
        blockDim[block] = MINBLOCKSIZE;
      }
      field[block] = zfp_field_2d(&array[block * MINBLOCKSIZE * nx], type, nx,
                                  blockDim[block]);
    }

    /* allocate meta data for a compressed stream */
    zfp[block] = zfp_stream_open(NULL);

    if (zfp_is_relative) {
      // we were concerned with relative error (precision)
      zfp_stream_set_precision(zfp[block], tolerance);
    } else {
      // we were concerned with absolute error (accuracy)
      // zfp_stream_set_accuracy(zfp[block], tolerance, type); // Old ZFP
      // version
      zfp_stream_set_accuracy(zfp[block], tolerance); // New ZFP version
    }

    bufsize[block] = zfp_stream_maximum_size(zfp[block], field[block]);
    buffer[block] = malloc(bufsize[block]);

    /* associate bit stream with allocated buffer */
    stream[block] = stream_open(buffer[block], bufsize[block]);
    zfp_stream_set_bit_stream(zfp[block], stream[block]);
    zfp_stream_rewind(zfp[block]);
  }
  timer->stop_timer("ZFP::Decompression::SetupProperties");
  timer->start_timer("IO::ZFP::Decompression");
  // read the data serially
  for (unsigned int block = 0; block < totalBlocks; block++) {
    fread(&zfpsize[block], sizeof(zfpsize[block]), 1, file);
    size_t readsize = fread(buffer[block], 1, zfpsize[block], file);
  }
  timer->stop_timer("IO::ZFP::Decompression");
  timer->start_timer("ZFP::Decompress");
#pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks; block++) {

    /* decompress entire array */
    if (!zfp_decompress(zfp[block], field[block])) {
      fprintf(stderr, "decompression failed\n");
      status = 1;
    } else {
      if ((nx > 1) && (ny > 1) && (nz > 1)) // handle 3d
        resultSize_block[block] = nx * ny * blockDim[block] * sizeof(float);
      else
        resultSize_block[block] = nx * blockDim[block] * sizeof(float);
    }
  }
  timer->stop_timer("ZFP::Decompress");
  for (int block = 0; block < totalBlocks; block++) {
    resultSize += resultSize_block[block];
  }

  // fprintf(stderr, "\n resultSize  = %d", resultSize);
  /* clean up */
  for (int block = 0; block < totalBlocks; block++) {
    zfp_field_free(field[block]);
    zfp_stream_close(zfp[block]);
    stream_close(stream[block]);
    free(buffer[block]);
  }

  return resultSize;
}

size_t applyZFPOperation(float *array, int nx, int ny, int nz, double tolerance,
                         FILE *file, int decompress, bool zfp_is_relative) {
  zfp_type type;     /* array scalar type */
  zfp_field *field;  /* array meta data */
  zfp_stream *zfp;   /* compressed stream */
  void *buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream *stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */
  size_t resultSize; /*for compression resultSize=zfpsize else nx*ny*nz */

  /* allocate meta data for the 3D array a[nz][ny][nx] */
  type = zfp_type_float;
  Timer *timer = Timer::getInstance();
  timer->start_timer("ZFP::SetupProperties");
  // To choose between 1d,2d or 3d representation
  if ((nx > 1) && (ny > 1) && (nz > 1)) // handle 3d
    field = zfp_field_3d(array, type, nx, ny, nz);
  else if ((nx > 1) && (nz > 1)) // handle 2d
    field = zfp_field_2d(array, type, nx, nz);
  else // handle 1d
    field = zfp_field_1d(array, type, nz);

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set compression mode and parameters via one of three functions */
  /*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
  /*  zfp_stream_set_precision(zfp, precision, type); */
  if (zfp_is_relative) {
    // we are concerned with relative error (precision)
    zfp_stream_set_precision(zfp, tolerance);
  } else {
    // we are concerned with absolute error (accuracy)
    // zfp_stream_set_accuracy(zfp[block], tolerance, type); // Old ZFP version
    zfp_stream_set_accuracy(zfp, tolerance); // New ZFP version
  }

  /* allocate buffer for compressed data */
  bufsize = zfp_stream_maximum_size(zfp, field);
  buffer = malloc(bufsize);

  /* associate bit stream with allocated buffer */
  stream = stream_open(buffer, bufsize);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);
  timer->stop_timer("ZFP::SetupProperties");
  /* compress or decompress entire array */
  if (decompress) {
    /* read compressed stream and decompress array */
    timer->start_timer("IO::ZFP::Decompression");
    fread(&zfpsize, 1, sizeof(zfpsize), file);
    fread(buffer, 1, zfpsize, file);
    timer->stop_timer("IO::ZFP::Decompression");
    timer->start_timer("ZFP::Decompress");
    if (!zfp_decompress(zfp, field)) {
      fprintf(stderr, "decompression failed\n");
      exit(0);
    } else {
      resultSize = nx * ny * nz * sizeof(float);
    }
    timer->stop_timer("ZFP::Decompress");
  } else {
    /* compress array and output compressed stream */
    timer->start_timer("ZFP::Compress");
    zfpsize = zfp_compress(zfp, field);
    timer->stop_timer("ZFP::Compress");
    if (!zfpsize) {
      fprintf(stderr, "compression failed\n");
      exit(0);
    } else {
      timer->start_timer("IO::ZFP::Compression");
      fwrite(&zfpsize, 1, sizeof(zfpsize), file);
      fwrite(buffer, 1, zfpsize, file);
      timer->stop_timer("IO::ZFP::Compression");
      resultSize = zfpsize;
    }
  }

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);

  return resultSize;
}

void no_compression_save(FILE *file, const float *data,
                         const size_t size) {
    fwrite(data, 1, size * sizeof(float), file);
}

void no_compression_load(FILE *file, float *data, const size_t size) {
    fread(data, 1, size * sizeof(float), file);
}

// This is the main-entry point for all compression algorithms
// Currently using a naive switch based differentiation of algo used
void compression(float *array, int nx, int ny, int nz, int nt, double tolerance,
                 unsigned int codecType, const char *filename,
                 bool zfp_is_relative) {
  FILE *compressed_file = fopen(filename, "wb");
  if (compressed_file == NULL) {
    fprintf(stderr, "Opening compressed file<%s> to write to failed...\n", filename);
    exit(0);
  }
  int pressure_size = nx * ny * nz;
  float *off_arr;
#ifdef ZFP_COMPRESSION
  for (int i = 0; i < nt; i++) {
      off_arr = &array[i * pressure_size];
      switch (codecType) {
          case 1:
              applyZFPOperation(off_arr, nx, ny, nz, tolerance, compressed_file, 0, zfp_is_relative);
              break;
          case 2:
              compressZFP_Parallel(off_arr, nx, ny, nz, tolerance, compressed_file, zfp_is_relative);
              break;
          default:
              fprintf(stderr, "***Invalid codec Type, Using ZFP***\n");
              applyZFPOperation(off_arr, nx, ny, nz, tolerance, compressed_file, 0, zfp_is_relative);
              break;
      }
  }
#else
  no_compression_save(compressed_file, array, nx * nz * ny * nt);
#endif
  fclose(compressed_file);
}

// This is the main-entry point for all decompression algorithms
// Currently using a naive switch based differentiation of algo used
void decompression(float *array, int nx, int ny, int nz, int nt, double tolerance,
                   unsigned int codecType, const char *filename,
                   bool zfp_is_relative) {
  FILE *compressed_file = fopen(filename, "rb");
  if (compressed_file == NULL) {
    fprintf(stderr, "Opening compressed file<%s> to read from failed...\n", filename);
    exit(0);
  }
  int pressure_size = nx * ny * nz;
  float *off_arr;
#ifdef ZFP_COMPRESSION
  for (int i = 0; i < nt; i++) {
      off_arr = &array[i * pressure_size];
      switch (codecType) {
          case 1:
              applyZFPOperation(off_arr, nx, ny, nz, tolerance, compressed_file, 1, zfp_is_relative);
              break;
          case 2:
              decompressZFP_Parallel(off_arr, nx, ny, nz, tolerance, compressed_file, zfp_is_relative);
              break;
          default:
              applyZFPOperation(off_arr, nx, ny, nz, tolerance, compressed_file, 1, zfp_is_relative);
              break;
      }
  }
#else
  no_compression_load(compressed_file, array, nx * ny * nz * nt);
#endif
  fclose(compressed_file);
}
} // namespace zfp
