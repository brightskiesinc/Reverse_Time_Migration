//
// Created by zeyad-osama on 13/03/2021.
//

#include <thoth/utils/convertors/StringsConvertor.hpp>

/// @todo To be removed
/// {
#include <thoth/exceptions/Exceptions.hpp>

using namespace thoth::exceptions;
/// }

using namespace thoth::utils::convertors;


short StringsConvertor::ToShort(const std::string &aStr) {
    /// @todo To be removed
    /// {
    throw NotImplementedException();
    /// }
}

unsigned short StringsConvertor::ToUShort(const std::string &aStr) {
    return std::stoul(aStr);
}

int StringsConvertor::ToInt(const std::string &aStr) {
    return std::stoi(aStr);
}

unsigned int StringsConvertor::ToUInt(const std::string &aStr) {
    /// @todo To be removed
    /// {
    throw NotImplementedException();
    /// }
}

long StringsConvertor::ToLong(const std::string &aStr) {
    return std::stol(aStr);
}

unsigned long StringsConvertor::ToULong(const std::string &aStr) {
    return std::stoul(aStr);
}

unsigned char *StringsConvertor::E2A(unsigned char *apSrc, size_t aSize) {
    for (size_t i = 0; i < aSize; i++) {
        apSrc[i] = E2A(apSrc[i]);
    }
    return apSrc;
}

unsigned char StringsConvertor::E2A(unsigned char aSrc) {
    return StringsConvertor::mE2ATable[(int) (aSrc)];
}

unsigned char StringsConvertor::mE2ATable[256] = {
        0, 1, 2, 3, 156, 9, 134, 127, 151, 141, 142, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 157, 133, 8, 135, 24, 25, 146, 143, 28, 29, 30, 31,
        128, 129, 130, 131, 132, 10, 23, 27, 136, 137, 138, 139, 140, 5, 6, 7,
        144, 145, 22, 147, 148, 149, 150, 4, 152, 153, 154, 155, 20, 21, 158, 26,
        32, 160, 161, 162, 163, 164, 165, 166, 167, 168, 91, 46, 60, 40, 43, 33,
        38, 169, 170, 171, 172, 173, 174, 175, 176, 177, 93, 36, 42, 41, 59, 94,
        45, 47, 178, 179, 180, 181, 182, 183, 184, 185, 124, 44, 37, 95, 62, 63,
        186, 187, 188, 189, 190, 191, 192, 193, 194, 96, 58, 35, 64, 39, 61, 34,
        195, 97, 98, 99, 100, 101, 102, 103, 104, 105, 196, 197, 198, 199, 200, 201,
        202, 106, 107, 108, 109, 110, 111, 112, 113, 114, 203, 204, 205, 206, 207, 208,
        209, 126, 115, 116, 117, 118, 119, 120, 121, 122, 210, 211, 212, 213, 214, 215,
        216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231,
        123, 65, 66, 67, 68, 69, 70, 71, 72, 73, 232, 233, 234, 235, 236, 237,
        125, 74, 75, 76, 77, 78, 79, 80, 81, 82, 238, 239, 240, 241, 242, 243,
        92, 159, 83, 84, 85, 86, 87, 88, 89, 90, 244, 245, 246, 247, 248, 249,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 250, 251, 252, 253, 254, 255
};
