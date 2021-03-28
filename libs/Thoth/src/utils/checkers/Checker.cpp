//
// Created by zeyad-osama on 08/03/2021.
//

#include <thoth/utils/checkers/Checker.hpp>
#include <cstdint>

using namespace thoth::utils::checkers;

bool Checker::IsLittleEndianMachine() {
    volatile uint32_t i = 0x01234567;
    return (*((uint8_t *) (&i))) == 0x67;
}