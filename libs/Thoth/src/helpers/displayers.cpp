//
// Created by zeyad-osama on 09/03/2021.
//

#include <thoth/helpers/displayers.h>

#include <thoth/lookups/tables/TextHeaderLookup.hpp>

#include <iostream>


void thoth::helpers::displayers::print_text_header(unsigned char *apTextHeader) {
    if (apTextHeader == nullptr) {
        std::cerr << "Error: Null pointer received. Nothing to be printed." << std::endl;
        return;
    }

    for (size_t i = 0; i < IO_SIZE_TEXT_HEADER; i++) {
        if ((i % 80) == 0)
            std::cout << std::endl;
        std::cout << apTextHeader[i];
    }
    std::cout << std::endl;
}
