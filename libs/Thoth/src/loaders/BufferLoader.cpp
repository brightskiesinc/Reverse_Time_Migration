//
// Created by pancee on 12/15/20.
//

#include <thoth/loaders/BufferLoader.hpp>

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

using namespace thoth::streams;

BufferLoader::BufferLoader(int aGapSize) : mGapSize(aGapSize) {
    InitBuffer(this->mGapSize);
};

BufferLoader::BufferLoader(FILE *aFile, int aGapSize) : mGapSize(aGapSize) {

    // determine the size of the file then create
    // a buffer of size + GAP_SIZE
    struct stat buf;

    fstat(fileno(aFile), &buf);
    long file_size = buf.st_size;
    InitBuffer(file_size + GAP_SIZE);
    MoveGapToPoint();
    ExpandGap((int) file_size);
    unsigned int amount = fread(this->mpGapStart, 1, file_size, aFile);

    this->mpGapStart += amount;
}


