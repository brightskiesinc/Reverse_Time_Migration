/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of SeismicToolbox.
 *
 * SeismicToolbox is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SeismicToolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <operations/utils/compressor/Compressor.hpp>

#include <bs/base/logger/concrete/LoggerSystem.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>

#include <cstdio>
#include <cstdlib>

#ifdef ZFP_COMPRESSION

#include <zfp.h>

#endif

#define ZFP_MAX_PAR_BLOCKS 512
#define MIN_BLOCK_SIZE 4

using namespace bs::timer;
using namespace operations::utils::compressors;
using namespace bs::base::logger;

void apply_zfp(float *array, int nx, int ny, int nz, int nt,
               FILE *file, int decompress, bool zfp_is_relative, double tolerance);

/**
 * @brief Compresses multidimensional array using ZFP algorithm.
 */
void compress_zfp(float *array, int nx, int ny, int nz, int nt,
                  double tolerance, FILE *file, bool zfp_is_relative);

/**
 * @brief Decompresses multidimensional array using ZFP algorithm.
 */
void decompress_zfp(float *array, int nx, int ny, int nz, int nt,
                    double tolerance, FILE *file, bool zfp_is_relative);

/**
 * @brief Compresses multidimensional array.
 */
void compress_normal(FILE *file, const float *data, int nx, int ny, int nz, int nt);

/**
 * @brief Decompresses multidimensional array.
 */
void decompress_normal(FILE *file, float *data, int nx, int ny, int nz, int nt);


void Compressor::Compress(float *array, int nx, int ny, int nz, int nt, double tolerance,
                          unsigned int codecType, const char *filename,
                          bool zfp_is_relative) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    FILE *compressed_file = fopen(filename, "wb");
    if (compressed_file == nullptr) {
        Logger->Error() << "Opening compressed file" << filename << "to write to failed..." << '\n';
        exit(EXIT_FAILURE);
    }
#ifdef ZFP_COMPRESSION
    switch (codecType) {
        case 1:
            apply_zfp(array, nx, ny, nz, nt, compressed_file, 0, zfp_is_relative, tolerance);
            break;
        case 2:
            compress_zfp(array, nx, ny, nz, nt, tolerance, compressed_file, zfp_is_relative);
            break;
        default:
            Logger->Error() << "***  Invalid codec Type, Terminating" << '\n';
            exit(EXIT_FAILURE);
    }
#else
    compress_normal(compressed_file, array, nx, ny, nz, nt);
#endif
    fclose(compressed_file);
}

void Compressor::Decompress(float *array, int nx, int ny, int nz, int nt, double tolerance,
                            unsigned int codecType, const char *filename,
                            bool zfp_is_relative) {
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    FILE *compressed_file = fopen(filename, "rb");
    if (compressed_file == nullptr) {
        Logger->Error() << "Opening compressed file " << filename << " to read from failed..." << '\n';
        exit(EXIT_FAILURE);
    }
#ifdef ZFP_COMPRESSION
    switch (codecType) {
        case 1:
            apply_zfp(array, nx, ny, nz, nt, compressed_file, 1, zfp_is_relative, tolerance);
            break;
        case 2:
            decompress_zfp(array, nx, ny, nz, nt, tolerance, compressed_file, zfp_is_relative);
            break;
        default:
            Logger->Error() << "***Invalid codec Type, Terminating" << '\n';
            exit(EXIT_FAILURE);
    }
#else
    decompress_normal(compressed_file, array, nx, ny, nz, nt);
#endif
    fclose(compressed_file);
}

void apply_zfp(float *array, int nx, int ny, int nz, int nt,
               FILE *file, int decompress, bool zfp_is_relative, double tolerance) {
#ifdef ZFP_COMPRESSION
    auto logger = LoggerSystem::GetInstance();
    /// Compressed stream
    zfp_stream *zfp;
    /// Storage for compressed stream
    void *buffer;
    /// Byte size of compressed buffer
    size_t buffer_size;
    /// Bit stream to write to or read from
    bitstream *stream;
    /// Array meta data
    zfp_field *size_field;
    /// Array scalar type
    zfp_type type;

    type = zfp_type_float;

    // Allocate meta data for a compressed stream
    zfp = zfp_stream_open(nullptr);

    // Set compression mode and parameters via one of three functions

    /*
     * zfp_stream_set_rate(zfp, rate, type, 3, 0);
     * zfp_stream_set_precision(zfp, precision, type);
     */

    if (zfp_is_relative) {
        // Concerned with relative error (precision)
        zfp_stream_set_precision(zfp, tolerance);
    } else {
        // Concerned with absolute error (accuracy)
        /*
         * /// Old ZFP version
         * zfp_stream_set_accuracy(zfp[block], tolerance, type);
         */
        zfp_stream_set_accuracy(zfp, tolerance);
    }
    // Chooses between 1D, 2D or 3D representation
    if ((nx > 1) && (ny > 1) && (nz > 1)) {
        // Handle 3D
        size_field = zfp_field_3d(array, type, nx, ny, nz);
    } else if ((nx > 1) && (nz > 1)) {
        // Handle 2D
        size_field = zfp_field_2d(array, type, nx, nz);
    } else {
        // Handle 1D
        size_field = zfp_field_1d(array, type, nz);
    }
    // Allocate buffer for compressed data
    buffer_size = zfp_stream_maximum_size(zfp, size_field);
    zfp_field_free(size_field);
    buffer = malloc(buffer_size);

    // Associate bit stream with allocated buffer
    stream = stream_open(buffer, buffer_size);
    zfp_stream_set_bit_stream(zfp, stream);
    int pressure_size = nx * ny * nz;
    float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &array[i * pressure_size];

        // Array meta data
        zfp_field *field;
        // Byte size of compressed stream
        size_t zfp_size;
        // Allocate meta-data for the 3D array a[nz][ny][nx]
        ElasticTimer compressor_timer("Compressor::ZFP::SetupProperties");
        compressor_timer.Start();
        // To choose between 1d,2d or 3d representation
        if ((nx > 1) && (ny > 1) && (nz > 1)) {// handle 3d
            field = zfp_field_3d(off_arr, type, nx, ny, nz);
        } else if ((nx > 1) && (nz > 1)) {// handle 2d
            field = zfp_field_2d(off_arr, type, nx, nz);
        } else {// handle 1d
            field = zfp_field_1d(off_arr, type, nz);
        }
        zfp_stream_rewind(zfp);
        compressor_timer.Stop();
        /* Compress or decompress entire array. */
        if (decompress) {
            /* Read compressed stream and decompress array. */
            {
                ScopeTimer t("Compressor::ZFP::IO::Decompression");
                fread(&zfp_size, 1, sizeof(zfp_size), file);
                fread(buffer, 1, zfp_size, file);
            }
            {
                ScopeTimer t("Compressor::ZFP::Decompress");
                if (!zfp_decompress(zfp, field)) {
                    logger->Error() << "decompression failed" << '\n';
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            /* Compress array and output compressed stream. */
            ElasticTimer compress_timer("Compressor::ZFP::Compress");
            compressor_timer.Start();
            zfp_size = zfp_compress(zfp, field);
            compressor_timer.Stop();
            if (!zfp_size) {
                logger->Error() << "compression failed" << '\n';
                exit(EXIT_FAILURE);
            } else {
                {
                    ScopeTimer t("Compressor::ZFP::IO::Compression");
                    fwrite(&zfp_size, 1, sizeof(zfp_size), file);
                    fwrite(buffer, 1, zfp_size, file);
                }
            }
        }
        zfp_field_free(field);
    }

    /*
     * Clean up
     */

    zfp_stream_close(zfp);
    stream_close(stream);
    free(buffer);
#endif
}

void compress_zfp(float *array, int nx, int ny, int nz, int nt,
                  double tolerance, FILE *file,
                  bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION
    LoggerSystem *Logger = LoggerSystem::GetInstance();
    unsigned int total_blocks = 0;

    // Calculating the number of blocks to be written, a block
    // will consist of a number of rows in nx dimension
    total_blocks = (nz + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE;

    int pressure_size = nx * ny * nz;

    /// Array scalar type
    zfp_type type;
    /// Array meta data
    auto **field = (zfp_field **) malloc(sizeof(zfp_field *) * total_blocks);
    /// Compressed stream
    auto **zfp = (zfp_stream **) malloc(sizeof(zfp_stream *) * total_blocks);
    /// Storage for compressed stream
    void **buffer = (void **) malloc(sizeof(void *) * total_blocks);
    /// Byte size of compressed buffer
    auto *buffer_size = (size_t *) malloc(sizeof(size_t) * total_blocks);
    /// Bit stream to write to or read from
    auto **stream = (bitstream **) malloc(sizeof(bitstream *) * total_blocks);
    /// Byte size of compressed stream
    auto *zfp_size = (size_t *) malloc(sizeof(size_t) * total_blocks);
    int *block_dim = (int *) malloc(sizeof(int) * total_blocks);
    type = zfp_type_float;

#pragma omp parallel for schedule(static)
    for (unsigned int block = 0; block < total_blocks; block++) {
        block_dim[block] = ((block < (total_blocks - 1)) ? MIN_BLOCK_SIZE : nz % MIN_BLOCK_SIZE);
        if (block_dim[block] == 0) {
            block_dim[block] = MIN_BLOCK_SIZE;
        }
        // Determine the size of a block before compression
        if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
            field[block] = zfp_field_3d(&array[block * MIN_BLOCK_SIZE * ny * nx], type, nx, ny, block_dim[block]);
        } else { // handle 2d
            field[block] = zfp_field_2d(&array[block * MIN_BLOCK_SIZE * nx], type, nx, block_dim[block]);
        }
        // Allocate meta data for a compressed stream.
        zfp[block] = zfp_stream_open(nullptr);

        if (zfp_is_relative) { // we are concerned with relative error (precision)
            zfp_stream_set_precision(zfp[block], tolerance);
        } else {
            // We are concerned with absolute error (accuracy)
            // zfp_stream_set_accuracy(zfp[block], tolerance, type);
            // Old ZFP version
            zfp_stream_set_accuracy(zfp[block], tolerance); // New ZFP version
        }

        // Allocate buffer for compressed data
        buffer_size[block] = zfp_stream_maximum_size(zfp[block], field[block]);
        buffer[block] = malloc(buffer_size[block]);

        // Associate bit stream with allocated buffer
        stream[block] = stream_open(buffer[block], buffer_size[block]);
        zfp_stream_set_bit_stream(zfp[block], stream[block]);
    }

    /*
     * Clean up
     */

    for (int block = 0; block < total_blocks; block++) {
        zfp_field_free(field[block]);
    }
    float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &array[i * pressure_size];
        ElasticTimer compress_timer("Compressor::ZFP::Compress");
        compress_timer.Start();

        /*
         * Loop on blocks, each is compressed and then written
         */

#pragma omp parallel for schedule(static)
        for (unsigned int block = 0; block < total_blocks; block++) {
            // Determine the size of a block before compression
            if ((nx > 1) && (ny > 1) && (nz > 1)) { // handle 3d
                field[block] = zfp_field_3d(&off_arr[block * MIN_BLOCK_SIZE * ny * nx], type, nx, ny, block_dim[block]);
            } else { // handle 2d
                field[block] = zfp_field_2d(&off_arr[block * MIN_BLOCK_SIZE * nx], type, nx, block_dim[block]);
            }
            zfp_stream_rewind(zfp[block]);

            // Compress entire arrayCompressor::ZFP::Compress
            zfp_size[block] = zfp_compress(zfp[block], field[block]);
            if (!zfp_size[block]) {
                Logger->Error() << "compression failed" << '\n';
                exit(EXIT_FAILURE);
            }
        }
        compress_timer.Stop();

        ElasticTimer io_compressor_timer("Compressor::ZFP::IO::Compression");
        io_compressor_timer.Start();

        /*
         * Write Block Sizes serially
         */

        for (int block = 0; block < total_blocks; block++) {
            fwrite(&zfp_size[block], sizeof(zfp_size[block]), 1, file); // write the size of the block after compression
            fwrite(buffer[block], 1, zfp_size[block], file); // write the actual compressed data
        }
        io_compressor_timer.Stop();

        /*
         * Clean up
         */

        for (int block = 0; block < total_blocks; block++) {
            zfp_field_free(field[block]);
        }
    }
    for (int block = 0; block < total_blocks; block++) {
        zfp_stream_close(zfp[block]);
        stream_close(stream[block]);
        free(buffer[block]);
    }
    free(field);
    free(zfp);
    free(buffer);
    free(buffer_size);
    free(stream);
    free(zfp_size);
    free(block_dim);
#endif
}

void decompress_zfp(float *array, int nx, int ny, int nz, int nt,
                    double tolerance, FILE *file,
                    bool zfp_is_relative) {
#ifdef ZFP_COMPRESSION

    LoggerSystem *Logger = LoggerSystem::GetInstance();
    unsigned int totalBlocks = 0;

    // Calculating the number of blocks to be written, a block
    // will consist of a number of rows in nx dimension
    totalBlocks = (nz + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE;

    int pressure_size = nx * ny * nz;
    /// Array scalar type
    zfp_type type;
    /// Array meta data
    auto **field = (zfp_field **) malloc(sizeof(zfp_field *) * totalBlocks);
    /// Compressed stream
    auto **zfp = (zfp_stream **) malloc(sizeof(zfp_stream *) * totalBlocks);
    /// Storage for compressed stream
    void **buffer = (void **) malloc(sizeof(void *) * totalBlocks);
    /// Byte size of compressed buffer
    auto *buffer_size = (size_t *) malloc(sizeof(size_t) * totalBlocks);
    /// Bit stream to write to or read from
    auto **stream = (bitstream **) malloc(sizeof(bitstream *) * totalBlocks);
    /// Byte size of compressed stream
    auto *zfp_size = (size_t *) malloc(sizeof(size_t) * totalBlocks);
    int *block_dim = (int *) malloc(sizeof(int) * totalBlocks);
    type = zfp_type_float;

#pragma omp parallel for schedule(static)
    for (unsigned int block = 0; block < totalBlocks; block++) {
        block_dim[block] = ((block < (totalBlocks - 1)) ? MIN_BLOCK_SIZE : nz % MIN_BLOCK_SIZE);
        if (block_dim[block] == 0) {
            block_dim[block] = MIN_BLOCK_SIZE;
        }
        // Determine the size of a block before compression
        if ((nx > 1) && (ny > 1) && (nz > 1)) {
            // Handle 3D
            field[block] = zfp_field_3d(&array[block * MIN_BLOCK_SIZE * ny * nx], type, nx, ny, block_dim[block]);
        } else {
            // Handle 2D
            field[block] = zfp_field_2d(&array[block * MIN_BLOCK_SIZE * nx], type, nx, block_dim[block]);
        }
        // Allocate meta data for a compressed stream
        zfp[block] = zfp_stream_open(nullptr);

        if (zfp_is_relative) {
            // We are concerned with relative error (precision)
            zfp_stream_set_precision(zfp[block], tolerance);
        } else {
            // We are concerned with absolute error (accuracy)
            // zfp_stream_set_accuracy(zfp[block], tolerance, type); // Old ZFP
            // version
            zfp_stream_set_accuracy(zfp[block], tolerance); // New ZFP version
        }

        //  Allocate buffer for compressed data
        buffer_size[block] = zfp_stream_maximum_size(zfp[block], field[block]);
        buffer[block] = malloc(buffer_size[block]);

        // Associate bit stream with allocated buffer
        stream[block] = stream_open(buffer[block], buffer_size[block]);
        zfp_stream_set_bit_stream(zfp[block], stream[block]);
    }

    /*
     * Clean up
     */

    for (int block = 0; block < totalBlocks; block++) {
        zfp_field_free(field[block]);
    }
    float *off_arr;
    for (int i = 0; i < nt; i++) {
        off_arr = &array[i * pressure_size];
        ElasticTimer io_decompress_timer("Compressor::ZFP::IO::Decompression");
        io_decompress_timer.Start();
        /*
         * Read Block Sizes in serial
         */

        for (int block = 0; block < totalBlocks; block++) {
            // Write the size of the block after compression
            fread(&zfp_size[block], sizeof(zfp_size[block]), 1, file);
            // Write the actual compressed data
            fread(buffer[block], 1, zfp_size[block], file);
        }
        io_decompress_timer.Stop();

        ElasticTimer decompress_timer("Compressor::ZFP::Decompress");
        decompress_timer.Start();

        /*
         * Loop on blocks, each is compressed and then written
         */

#pragma omp parallel for schedule(static)
        for (unsigned int block = 0; block < totalBlocks; block++) {
            // Determine the size of a block before compression
            if ((nx > 1) && (ny > 1) && (nz > 1)) {
                // Handle 3D
                field[block] = zfp_field_3d(&off_arr[block * MIN_BLOCK_SIZE * ny * nx], type, nx, ny, block_dim[block]);
            } else {
                // Handle 2D
                field[block] = zfp_field_2d(&off_arr[block * MIN_BLOCK_SIZE * nx], type, nx, block_dim[block]);
            }
            zfp_stream_rewind(zfp[block]);

            // Decompress entire array
            if (!zfp_decompress(zfp[block], field[block])) {
                Logger->Error() << "Decompression failed" << '\n';
                exit(EXIT_FAILURE);
            }
        }
        decompress_timer.Stop();

        /*
         * Clean up
         */

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
    free(buffer_size);
    free(stream);
    free(zfp_size);
    free(block_dim);
#endif
}

void compress_normal(FILE *file, const float *data, int nx, int ny, int nz, int nt) {
    int size = nx * ny * nz;
    const float *offset_arr;
    {
        ScopeTimer t("Compressor::Normal::Compress");
        for (int i = 0; i < nt; i++) {
            offset_arr = &data[i * size];
            fwrite(offset_arr, 1, size * sizeof(float), file);
        }
    }
}

void decompress_normal(FILE *file, float *data, int nx, int ny, int nz, int nt) {
    int size = nx * ny * nz;
    float *offset_arr;
    {
        ScopeTimer t("Compressor::Normal::Decompress");
        for (int i = 0; i < nt; i++) {
            offset_arr = &data[i * size];
            fread(offset_arr, 1, size * sizeof(float), file);
        }
    }
}
