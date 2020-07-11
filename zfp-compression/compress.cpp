#include "compress.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zfp.h>
#include <skeleton/helpers/timer/timer.hpp>

void compressZFP_Parallel(float *array, int nx, int ny, int nz, int nt,
                            double tolerance, FILE *file,
                            bool zfp_is_relative) {
  // this function is used for compression of a multidimensional array
  unsigned int totalBlocks = 0;

  totalBlocks = (nz + MINBLOCKSIZE - 1) / MINBLOCKSIZE; // calculating the number of blocks to be written, a block
                                                        // will consist of a number of rows in nx dimension
  int pressure_size = nx * ny * nz;
  zfp_type type;                        /* array scalar type */
  zfp_field **field = (zfp_field **)malloc(sizeof(zfp_field *) * totalBlocks); /* array meta data */
  zfp_stream **zfp = (zfp_stream **)malloc(sizeof(zfp_stream *) * totalBlocks);  /* compressed stream */
  void **buffer = (void **)malloc(sizeof(void *) * totalBlocks);     /* storage for compressed stream */
  size_t *bufsize = (size_t *)malloc(sizeof(size_t) * totalBlocks);   /* byte size of compressed buffer */
  bitstream **stream = (bitstream **)malloc(sizeof(bitstream *) * totalBlocks);    /* bit stream to write to or read from */
  size_t *zfpsize = (size_t *)malloc(sizeof(size_t) * totalBlocks); /* byte size of compressed stream */
  int *blockDim = (int *)malloc(sizeof(int) * totalBlocks);
  type = zfp_type_float;
  #pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks; block++) {
        blockDim[block] = ((block < (totalBlocks - 1)) ? MINBLOCKSIZE : nz % MINBLOCKSIZE);
        if (blockDim[block] == 0) {
          blockDim[block] = MINBLOCKSIZE;
        }
        // determine the size of a block before compression
        if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
            field[block] = zfp_field_3d(&array[block * MINBLOCKSIZE * ny * nx], type, nx, ny, blockDim[block]);
        } else { // handle 2d
            field[block] = zfp_field_2d(&array[block * MINBLOCKSIZE * nx], type, nx, blockDim[block]);
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
  }
  /* clean up */
  for (int block = 0; block < totalBlocks; block++) {
    zfp_field_free(field[block]);
  }
  float *off_arr;
  for (int i = 0; i < nt; i++) {
      off_arr = &array[i * pressure_size];
      Timer *timer = Timer::getInstance();
      timer->start_timer("ZFP::Compress");
      // loop on blocks, each is compressed and then written
      #pragma omp parallel for schedule(static)
      for (unsigned int block = 0; block < totalBlocks; block++) {
          // determine the size of a block before compression
          if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
              field[block] = zfp_field_3d(&off_arr[block * MINBLOCKSIZE * ny * nx], type, nx, ny, blockDim[block]);
          } else { // handle 2d
              field[block] = zfp_field_2d(&off_arr[block * MINBLOCKSIZE * nx], type, nx, blockDim[block]);
          }
          zfp_stream_rewind(zfp[block]);
          /* compress entire array */
          zfpsize[block] = zfp_compress(zfp[block], field[block]);
          if (!zfpsize[block]) {
              fprintf(stderr, "compression failed\n");
              exit(0);
          }
      }
      timer->stop_timer("ZFP::Compress");
      timer->start_timer("IO::ZFP::Compression");
      // Write Block Sizes in serial
      for (int block = 0; block < totalBlocks; block++) {
          fwrite(&zfpsize[block], sizeof(zfpsize[block]), 1, file); // write the size of the block after compression
          fwrite(buffer[block], 1, zfpsize[block], file); // write the actual compressed data
      }
      timer->stop_timer("IO::ZFP::Compression");
      /* clean up */
      for (int block = 0; block < totalBlocks; block++) {
          zfp_field_free(field[block]);
      }
  }
  for (int block = 0; block < totalBlocks; block++) {
      zfp_stream_close(zfp[block]);
      stream_close(stream[block]);
      free(buffer[block]);
  }
  free(field);
  free(zfp);
  free(buffer);
  free(bufsize);
  free(stream);
  free(zfpsize);
  free(blockDim);
}

void decompressZFP_Parallel(float *array, int nx, int ny, int nz, int nt,
                              double tolerance, FILE *file,
                              bool zfp_is_relative) {
    // this function is used for compression of a multidimensional array
    unsigned int totalBlocks = 0;

    totalBlocks = (nz + MINBLOCKSIZE - 1) / MINBLOCKSIZE; // calculating the number of blocks to be written, a block
    // will consist of a number of rows in nx dimension
    int pressure_size = nx * ny * nz;
    zfp_type type;                        /* array scalar type */
    zfp_field **field = (zfp_field **)malloc(sizeof(zfp_field *) * totalBlocks); /* array meta data */
    zfp_stream **zfp = (zfp_stream **)malloc(sizeof(zfp_stream *) * totalBlocks);  /* compressed stream */
    void **buffer = (void **)malloc(sizeof(void *) * totalBlocks);     /* storage for compressed stream */
    size_t *bufsize = (size_t *)malloc(sizeof(size_t) * totalBlocks);   /* byte size of compressed buffer */
    bitstream **stream = (bitstream **)malloc(sizeof(bitstream *) * totalBlocks);    /* bit stream to write to or read from */
    size_t *zfpsize = (size_t *)malloc(sizeof(size_t) * totalBlocks); /* byte size of compressed stream */
    int *blockDim = (int *)malloc(sizeof(int) * totalBlocks);
    type = zfp_type_float;
#pragma omp parallel for schedule(static)
    for (unsigned int block = 0; block < totalBlocks; block++) {
        blockDim[block] = ((block < (totalBlocks - 1)) ? MINBLOCKSIZE : nz % MINBLOCKSIZE);
        if (blockDim[block] == 0) {
            blockDim[block] = MINBLOCKSIZE;
        }
        // determine the size of a block before compression
        if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
            field[block] = zfp_field_3d(&array[block * MINBLOCKSIZE * ny * nx], type, nx, ny, blockDim[block]);
        } else { // handle 2d
            field[block] = zfp_field_2d(&array[block * MINBLOCKSIZE * nx], type, nx, blockDim[block]);
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
    }
    /* clean up */
    for (int block = 0; block < totalBlocks; block++) {
        zfp_field_free(field[block]);
    }
    float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &array[i * pressure_size];
        Timer *timer = Timer::getInstance();
        timer->start_timer("IO::ZFP::Decompression");
        // Read Block Sizes in serial
        for (int block = 0; block < totalBlocks; block++) {
            fread(&zfpsize[block], sizeof(zfpsize[block]), 1, file); // write the size of the block after compression
            fread(buffer[block], 1, zfpsize[block], file); // write the actual compressed data
        }
        timer->stop_timer("IO::ZFP::Decompression");

        timer->start_timer("ZFP::Decompress");
        // loop on blocks, each is compressed and then written
#pragma omp parallel for schedule(static)
        for (unsigned int block = 0; block < totalBlocks; block++) {
            // determine the size of a block before compression
            if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
                field[block] = zfp_field_3d(&off_arr[block * MINBLOCKSIZE * ny * nx], type, nx, ny, blockDim[block]);
            } else { // handle 2d
                field[block] = zfp_field_2d(&off_arr[block * MINBLOCKSIZE * nx], type, nx, blockDim[block]);
            }
            zfp_stream_rewind(zfp[block]);
            /* decompress entire array */
            if (!zfp_decompress(zfp[block], field[block])) {
                fprintf(stderr, "decompression failed\n");
                exit(0);
            }
        }
        timer->stop_timer("ZFP::Decompress");
        /* clean up */
        for (int block = 0; block < totalBlocks; block++) {
            zfp_field_free(field[block]);
        }
    }
    for (int block = 0; block < totalBlocks; block++) {
        zfp_stream_close(zfp[block]);
        stream_close(stream[block]);
        free(buffer[block]);
    }
    free(field);
    free(zfp);
    free(buffer);
    free(bufsize);
    free(stream);
    free(zfpsize);
    free(blockDim);
}

void applyZFPOperation(float *array, int nx, int ny, int nz, int nt,
        FILE *file, int decompress, bool zfp_is_relative, double tolerance) {
  zfp_stream *zfp;   /* compressed stream */
  void *buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream *stream; /* bit stream to write to or read from */
  zfp_field *size_field;  /* array meta data */
  zfp_type type;     /* array scalar type */
  type = zfp_type_float;
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
  // To choose between 1d,2d or 3d representation
  if ((nx > 1) && (ny > 1) && (nz > 1)) {// handle 3d
      size_field = zfp_field_3d(array, type, nx, ny, nz);
  } else if ((nx > 1) && (nz > 1)) {// handle 2d
      size_field = zfp_field_2d(array, type, nx, nz);
  } else {// handle 1d
      size_field = zfp_field_1d(array, type, nz);
  }
  /* allocate buffer for compressed data */
  bufsize = zfp_stream_maximum_size(zfp, size_field);
  zfp_field_free(size_field);
  buffer = malloc(bufsize);
  /* associate bit stream with allocated buffer */
  stream = stream_open(buffer, bufsize);
  zfp_stream_set_bit_stream(zfp, stream);
  int pressure_size = nx * ny * nz;
  float *off_arr;
  for (int i = 0; i < nt; i++) {
      off_arr = &array[i * pressure_size];

      zfp_field *field;  /* array meta data */
      size_t zfpsize;    /* byte size of compressed stream */
      /* allocate meta data for the 3D array a[nz][ny][nx] */
      Timer *timer = Timer::getInstance();
      timer->start_timer("ZFP::SetupProperties");
      // To choose between 1d,2d or 3d representation
      if ((nx > 1) && (ny > 1) && (nz > 1)) {// handle 3d
          field = zfp_field_3d(off_arr, type, nx, ny, nz);
      } else if ((nx > 1) && (nz > 1)) {// handle 2d
          field = zfp_field_2d(off_arr, type, nx, nz);
      } else {// handle 1d
          field = zfp_field_1d(off_arr, type, nz);
      }
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
          }
      }
      zfp_field_free(field);
  }
  /* clean up */
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
}

void no_compression_save(FILE *file, const float *data, int nx, int ny, int nz, int nt) {
    int pressure_size = nx * ny * nz;
    const float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &data[i * pressure_size];
        fwrite(off_arr, 1, pressure_size * sizeof(float), file);
    }
}

void no_compression_load(FILE *file, float *data, int nx, int ny, int nz, int nt) {
    int pressure_size = nx * ny * nz;
    float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &data[i * pressure_size];
        fread(off_arr, 1, pressure_size * sizeof(float), file);
    }
}


// This is the main-entry point for all compression algorithms
// Currently using a naive switch based differentiation of algo used
void do_compression_save(float *array, int nx, int ny, int nz, int nt, double tolerance,
                 unsigned int codecType, const char *filename,
                 bool zfp_is_relative) {
  FILE *compressed_file = fopen(filename, "wb");
  if (compressed_file == NULL) {
    fprintf(stderr, "Opening compressed file<%s> to write to failed...\n", filename);
    exit(0);
  }
#ifdef ZFP_COMPRESSION
  switch (codecType) {
      case 1:
          applyZFPOperation(array, nx, ny, nz, nt, compressed_file, 0, zfp_is_relative, tolerance);
      break;
      case 2:
          compressZFP_Parallel(array, nx, ny, nz, nt, tolerance, compressed_file, zfp_is_relative);
          break;
      default:
          fprintf(stderr, "***Invalid codec Type, Terminating\n");
          exit(0);
  }
#else
  no_compression_save(compressed_file, array, nx, ny, nz, nt);
#endif
  fclose(compressed_file);
}

// This is the main-entry point for all decompression algorithms
// Currently using a naive switch based differentiation of algo used
void do_decompression_load(float *array, int nx, int ny, int nz, int nt, double tolerance,
                   unsigned int codecType, const char *filename,
                   bool zfp_is_relative) {
  FILE *compressed_file = fopen(filename, "rb");
  if (compressed_file == NULL) {
    fprintf(stderr, "Opening compressed file<%s> to read from failed...\n", filename);
    exit(0);
  }
#ifdef ZFP_COMPRESSION
  switch (codecType) {
      case 1:
          applyZFPOperation(array, nx, ny, nz, nt, compressed_file, 1, zfp_is_relative, tolerance);
          break;
      case 2:
          decompressZFP_Parallel(array, nx, ny, nz, nt, tolerance, compressed_file, zfp_is_relative);
          break;
      default:
          fprintf(stderr, "***Invalid codec Type, Terminating\n");
          exit(0);
  }
#else
  no_compression_load(compressed_file, array, nx, ny, nz, nt);
#endif
  fclose(compressed_file);
}
