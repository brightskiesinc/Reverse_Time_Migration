#include "compress.h"
#include <fstream>
#include <skeleton/helpers/timer/timer.hpp>

namespace zfp {
// this namespace contains all the functions related to zfp compression

void setPath(std::string path) { zfp::write_path = path; }

// Supporting function for creating compressed filename
// concatenating the given model name and timestep
char *getFileName(const char *model, unsigned int curTimestep) {
  // creates the filename of the current time step
  char *filename;
  filename = (char *)malloc(256);
  sprintf(filename, (zfp::write_path + "/%s_%u").c_str(), model, curTimestep);
  return filename;
}

char *getBlockFileName(const char *filename, unsigned int blockId) {
  // creates the block file name, currently not used
  char *blockfilename;
  blockfilename = (char *)malloc(256);
  sprintf(blockfilename, "%s_%u", filename, blockId);
  return blockfilename;
}

size_t compressZFP_Parallel_1d(float *array, size_t nz, double tolerance,
                               char *filename, bool zfp_is_relative) {
  // compress using zfp for a single trace (one dimensional vector)
#ifdef ZFP_COMPRESSION
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

  FILE *compressedFile;
  compressedFile = fopen(filename, "wb");

  unsigned int totalBlocks = ((nz + MINBLOCKSIZE1D - 1) / MINBLOCKSIZE1D);

  size_t bytesWritten = 0;
  bytesWritten =
      fwrite(&totalBlocks, 4, 1,
             compressedFile); // writes the number of block to be written
  resultSize += 4;
  // fprintf(stderr, "\n BytesWritten = %d", bytesWritten);

#pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks;
       block++) { // loop on each block to be written, a trace is divided into
                  // blocks with each compressed independently from others

    type = zfp_type_float;

    blockDim[block] =
        ((block < (totalBlocks - 1))
             ? MINBLOCKSIZE1D
             : nz % MINBLOCKSIZE1D); // the block size is MINBLOCKSIZE unless we
                                     // are at the last block (in this case take
                                     // the remaining values as a smaller block)

    field[block] =
        zfp_field_1d(&array[block * MINBLOCKSIZE1D], type, blockDim[block]);

    /* allocate meta data for a compressed stream */
    zfp[block] = zfp_stream_open(NULL);

    if (zfp_is_relative) { // we are interested in relative error (precision)
      zfp_stream_set_precision(zfp[block], tolerance);
    } else { // we are interested in absolute error (accuracy)
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

  // Write Block Sizes in serial
  for (int block = 0; block < totalBlocks; block++) {
    // fprintf(stderr, "\n ZFPSize : %d", zfpsize[block]);
    bytesWritten =
        fwrite(&zfpsize[block], sizeof(zfpsize[block]), 1,
               compressedFile); // write the size of a compressed block
    resultSize += sizeof(zfpsize[block]); // accumulate the value to resultSize
    // fprintf(stderr, "\n sizes BytesWritten = %d", bytesWritten);
    bytesWritten = fwrite(buffer[block], 1, zfpsize[block],
                          compressedFile); // write the actual compressed data
    // fprintf(stderr, "\n data: BytesWritten = %d", bytesWritten);
    resultSize += zfpsize[block]; // accumulate the value to resultSize
  }
  // fprintf(stderr, "\n resultSize  = %d", resultSize);
  /* clean up */
  for (int block = 0; block < totalBlocks; block++) {
    zfp_field_free(field[block]);
    zfp_stream_close(zfp[block]);
    stream_close(stream[block]);
    free(buffer[block]);
  }
  //  free(array);
  fclose(compressedFile);

  return resultSize;
#endif
}

size_t compressZFP_Parallel(float *array, int nx, int ny, int nz,
                            double tolerance, char *filename,
                            bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION
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
  FILE *compressedFile;
  compressedFile = fopen(filename, "wb");

  totalBlocks =
      (nz + MINBLOCKSIZE - 1) /
      MINBLOCKSIZE; // calculating the number of blocks to be written, a block
                    // will consist of a number of rows in nx dimension
  size_t bytesWritten = 0;
  Timer *timer = Timer::getInstance();
  timer->start_timer("IO::ZFP::Compression::Header");
  bytesWritten =
      fwrite(&totalBlocks, 4, 1,
             compressedFile); // write the number of blocks to be written
  timer->stop_timer("IO::ZFP::Compression::Header");
  resultSize += 4;            // accumulate the size of the written data
  // fprintf(stderr, "\n BytesWritten = %d", bytesWritten);

  //#pragma omp parallel for schedule(static)
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
    // fprintf(stderr, "\n ZFPSize : %d", zfpsize[block]);
    bytesWritten =
        fwrite(&zfpsize[block], sizeof(zfpsize[block]), 1,
               compressedFile); // write the size of the block after compression
    resultSize += sizeof(zfpsize[block]); // accumulate the written data size
    // fprintf(stderr, "\n sizes BytesWritten = %d", bytesWritten);
    bytesWritten = fwrite(buffer[block], 1, zfpsize[block],
                          compressedFile); // write the actual compressed data
    // fprintf(stderr, "\n data: BytesWritten = %d", bytesWritten);
    resultSize += zfpsize[block]; // accumulate the written data size
  }
  timer->stop_timer("IO::ZFP::Compression");
  // fprintf(stderr, "\n resultSize  = %d", resultSize);
  /* clean up */
  for (int block = 0; block < totalBlocks; block++) {
    zfp_field_free(field[block]);
    zfp_stream_close(zfp[block]);
    stream_close(stream[block]);
    free(buffer[block]);
  }
  //  free(array);
  fclose(compressedFile);

  return resultSize;
#endif
}

size_t decompressZFP_Parallel_1d(float *array, size_t nz, double tolerance,
                                 char *filename, bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION
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

  FILE *compressedFile;
  compressedFile = fopen(filename, "rb");
  fread(&totalBlocks, 4, 1,
        compressedFile); // read the number of blocks to loop on
                         // numThreads = 1;
  // fprintf(stderr, "\n Decompression: NumThreads = %d", numThreads);

#pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks; block++) {
    // loop on blocks, in each read the block from IO then decompress and write
    // it to the array

    /* allocate meta data for the 3D array a[nz][ny][nx] */

    type = zfp_type_float;

    // blockDim[block] = ((block < (numThreads - 1)) ? nz/numThreads :
    // (nz/numThreads) + (nz % numThreads));
    blockDim[block] =
        ((block < (totalBlocks - 1))
             ? MINBLOCKSIZE1D
             : nz % MINBLOCKSIZE1D); // the size of a block before compression

    field[block] =
        zfp_field_1d(&array[block * MINBLOCKSIZE1D], type, blockDim[block]);

    /* allocate meta data for a compressed stream */
    zfp[block] = zfp_stream_open(NULL);

    if (zfp_is_relative) {
      // we were concerned with relative error (precision)
      zfp_stream_set_precision(zfp[block], tolerance);
    } else { // we were concerned with absolute error (accuracy)
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

  // read the data serially
  for (unsigned int block = 0; block < totalBlocks; block++) {
    fread(&bufsize[block], sizeof(bufsize[block]), 1, compressedFile);

    size_t readsize = fread(buffer[block], 1, bufsize[block], compressedFile);
  }

  fclose(compressedFile);

#pragma omp parallel for schedule(static)
  for (unsigned int block = 0; block < totalBlocks; block++) {

    /* decompress entire array */
    if (!zfp_decompress(zfp[block], field[block])) {
      fprintf(stderr, "decompression failed\n");
      status = 1;
    } else {
      // resultSize_block[block] = nx * ny * blockDim[block] * sizeof(float);
      resultSize_block[block] = blockDim[block] * sizeof(float);
    }
  }

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
#endif
}

size_t decompressZFP_Parallel(float *array, int nx, int ny, int nz,
                              double tolerance, char *filename,
                              bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION
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

  FILE *compressedFile;
  compressedFile = fopen(filename, "rb");
  Timer *timer = Timer::getInstance();
  timer->start_timer("IO::ZFP::Decompression::Header");
  fread(&totalBlocks, 4, 1, compressedFile);
  timer->stop_timer("IO::ZFP::Decompression::Header");
  // numThreads = 1;
  // fprintf(stderr, "\n Decompression: NumThreads = %d", numThreads);
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
    fread(&bufsize[block], sizeof(bufsize[block]), 1, compressedFile);

    size_t readsize = fread(buffer[block], 1, bufsize[block], compressedFile);
  }
  timer->stop_timer("IO::ZFP::Decompression");
  fclose(compressedFile);
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
#endif
}

size_t applyZFPOperation(float *array, int nx, int ny, int nz, double tolerance,
                         char *filename, int decompress, bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION
  int status = 0;    /* return value: 0 = success */
  zfp_type type;     /* array scalar type */
  zfp_field *field;  /* array meta data */
  zfp_stream *zfp;   /* compressed stream */
  void *buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream *stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */
  size_t resultSize; /*for compression resultSize=zfpsize else nx*ny*nz */

  FILE *compressedFile;

  if (!decompress)
    compressedFile = fopen(filename, "wb");
  else
    compressedFile = fopen(filename, "rb");
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
    zfpsize = fread(buffer, 1, bufsize, compressedFile);
    timer->stop_timer("IO::ZFP::Decompression");
    timer->start_timer("ZFP::Decompress");
    if (!zfp_decompress(zfp, field)) {
      fprintf(stderr, "decompression failed\n");
      status = 1;
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
      status = 1;
    } else
    // fwrite(buffer, 1, zfpsize, stdout);
    {
      timer->start_timer("IO::ZFP::Compression");
      fwrite(buffer, 1, zfpsize, compressedFile);
      timer->stop_timer("IO::ZFP::Compression");
      resultSize = zfpsize;
    }
  }

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
  //  free(array);

  fclose(compressedFile);

  return resultSize;
#endif
}

void no_compression_save(const char *fname, const float *data,
                         const size_t size) {
  std::ofstream myfile(fname, std::ios::out | std::ios::binary);
  if (!myfile.is_open()) {
    exit(EXIT_FAILURE);
  }
  myfile.write(reinterpret_cast<const char *>(data), size * sizeof(float));
  myfile.close();
}

void no_compression_load(const char *fname, float *data, const size_t size) {
  std::ifstream is(fname, std::ios::binary | std::ios::in);
  if (!is.is_open()) {
    exit(EXIT_FAILURE);
  }
  is.read(reinterpret_cast<char *>(data),
          std::streamsize(size * sizeof(float)));
  is.close();
}

// This is the main-entry point for all compression algorithms
// Currently using a naive switch based differentiation of algo used
void compression(float *array, int nx, int ny, int nz, double tolerance,
                 unsigned int codecType, unsigned int curTimestep,
                 size_t *resultSize, const char *inputModel,
                 bool zfp_is_relative) {
  char *filename = getFileName(
      inputModel,
      curTimestep); // returns the file name where this time step is recorded
#ifdef ZFP_COMPRESSION
  switch (codecType) {

  case 1:
    // printf("Compression: Using ZFP\n");
    //*resultSize = compressZFP_Parallel(array, nx, ny, nz, tolerance,
    // filename);
    *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance, filename, 0,
                                    zfp_is_relative);
    break;

  case 2:
    // printf("Compression: Using ZFP\n");
    *resultSize = compressZFP_Parallel(array, nx, ny, nz, tolerance, filename,
                                       zfp_is_relative);
    //*resultSize = compressZFP_Parallel(array, nx * ny, nz, 1, tolerance,
    // filename); *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance,
    // filename, 0);
    break;
  default:
    // printf("No compression\n");
    fprintf(stderr, "***Invalid codec Type, Using ZFP***\n");
    *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance, filename, 0,
                                    zfp_is_relative);
    break;
  }
#else
  no_compression_save(filename, array, nx * nz * ny);
#endif
  free(filename);
  return;
}

// This is the main-entry point for all decompression algorithms
// Currently using a naive switch based differentiation of algo used
void decompression(float *array, int nx, int ny, int nz, double tolerance,
                   unsigned int codecType, unsigned int curTimestep,
                   size_t *resultSize, const char *inputModel,
                   bool zfp_is_relative) {
  char *filename = getFileName(inputModel, curTimestep);
#ifdef ZFP_COMPRESSION
  switch (codecType) {

  case 1:
    // printf("Decompression: Using ZFP\n");
    *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance, filename, 1,
                                    zfp_is_relative);
    break;

  case 2:
    // printf("Decompression: Using ZFP\n");
    *resultSize = decompressZFP_Parallel(array, nx, ny, nz, tolerance, filename,
                                         zfp_is_relative);
    //*resultSize = decompressZFP_Parallel(array, nx *ny, nz, 1, tolerance,
    // filename); *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance,
    // filename, 1);
    break;

  default:
    // printf("No decompression used\n");
    *resultSize = applyZFPOperation(array, nx, ny, nz, tolerance, filename, 1,
                                    zfp_is_relative);
    break;
  }
#else
  no_compression_load(filename, array, nx * ny * nz);
#endif
  free(filename);
  return;
}
} // namespace zfp
