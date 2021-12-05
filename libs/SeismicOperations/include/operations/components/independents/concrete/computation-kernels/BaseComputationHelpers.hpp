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

#ifndef OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_BASE_COMPUTATION_HELPERS_HPP
#define OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_BASE_COMPUTATION_HELPERS_HPP

#ifndef fma
#define fma(a, b, c) ((a) * (b) + (c))
#endif
/**
 * Computation kernels forward declarations utilities.
 */

#define FORWARD_DECLARE_COMPUTE_TEMP_3(CLASS, FUNCTION, KERNEL_TYPE, IS_2D) \
    template void CLASS::FUNCTION<KERNEL_TYPE, IS_2D, O_2>(); \
    template void CLASS::FUNCTION<KERNEL_TYPE, IS_2D, O_4>(); \
    template void CLASS::FUNCTION<KERNEL_TYPE, IS_2D, O_8>(); \
    template void CLASS::FUNCTION<KERNEL_TYPE, IS_2D, O_12>(); \
    template void CLASS::FUNCTION<KERNEL_TYPE, IS_2D, O_16>();

#define FORWARD_DECLARE_COMPUTE_TEMP_2(CLASS, FUNCTION, KERNEL_TYPE) \
    FORWARD_DECLARE_COMPUTE_TEMP_3(CLASS, FUNCTION, KERNEL_TYPE, true) \
    FORWARD_DECLARE_COMPUTE_TEMP_3(CLASS, FUNCTION, KERNEL_TYPE, false)

#define FORWARD_DECLARE_COMPUTE_TEMPLATE(CLASS, FUNCTION) \
    FORWARD_DECLARE_COMPUTE_TEMP_2(CLASS, FUNCTION, KERNEL_MODE::FORWARD) \
    FORWARD_DECLARE_COMPUTE_TEMP_2(CLASS, FUNCTION, KERNEL_MODE::INVERSE) \
    FORWARD_DECLARE_COMPUTE_TEMP_2(CLASS, FUNCTION, KERNEL_MODE::ADJOINT)

/**
 * Boundary managers forward declaration utilities.
 */
#define FORWARD_DECLARE_BOUND_TEMP_3(FUNCTION, KERNEL_TYPE, AXIS, OPPOSITE) \
    template void FUNCTION<KERNEL_TYPE, AXIS, OPPOSITE, O_2>(); \
    template void FUNCTION<KERNEL_TYPE, AXIS, OPPOSITE, O_4>(); \
    template void FUNCTION<KERNEL_TYPE, AXIS, OPPOSITE, O_8>(); \
    template void FUNCTION<KERNEL_TYPE, AXIS, OPPOSITE, O_12>(); \
    template void FUNCTION<KERNEL_TYPE, AXIS, OPPOSITE, O_16>();

#define FORWARD_DECLARE_BOUND_TEMP_2(FUNCTION, KERNEL_TYPE, AXIS) \
    FORWARD_DECLARE_BOUND_TEMP_3(FUNCTION, KERNEL_TYPE, AXIS, false) \
    FORWARD_DECLARE_BOUND_TEMP_3(FUNCTION, KERNEL_TYPE, AXIS, true)

#define FORWARD_DECLARE_BOUND_TEMP_1(FUNCTION, KERNEL_TYPE) \
    FORWARD_DECLARE_BOUND_TEMP_2(FUNCTION, KERNEL_TYPE, X_AXIS) \
    FORWARD_DECLARE_BOUND_TEMP_2(FUNCTION, KERNEL_TYPE, Z_AXIS) \
    FORWARD_DECLARE_BOUND_TEMP_2(FUNCTION, KERNEL_TYPE, Y_AXIS)

#define FORWARD_DECLARE_BOUND_TEMPLATE(FUNCTION) \
    FORWARD_DECLARE_BOUND_TEMP_1(FUNCTION, true) \
    FORWARD_DECLARE_BOUND_TEMP_1(FUNCTION, false)

#define FORWARD_DECLARE_SINGLE_BOUND_TEMP_3(FUNCTION, AXIS, OPPOSITE) \
    template void FUNCTION<AXIS, OPPOSITE, O_2>(); \
    template void FUNCTION<AXIS, OPPOSITE, O_4>(); \
    template void FUNCTION<AXIS, OPPOSITE, O_8>(); \
    template void FUNCTION<AXIS, OPPOSITE, O_12>(); \
    template void FUNCTION<AXIS, OPPOSITE, O_16>();

#define FORWARD_DECLARE_SINGLE_BOUND_TEMP_2(FUNCTION, AXIS) \
    FORWARD_DECLARE_SINGLE_BOUND_TEMP_3(FUNCTION, AXIS, false) \
    FORWARD_DECLARE_SINGLE_BOUND_TEMP_3(FUNCTION, AXIS, true)

#define FORWARD_DECLARE_SINGLE_BOUND_TEMPLATE(FUNCTION) \
    FORWARD_DECLARE_SINGLE_BOUND_TEMP_2(FUNCTION, X_AXIS) \
    FORWARD_DECLARE_SINGLE_BOUND_TEMP_2(FUNCTION, Z_AXIS) \
    FORWARD_DECLARE_SINGLE_BOUND_TEMP_2(FUNCTION, Y_AXIS)

/**
 * Unrolled derivation operators utilities.
 */

#define DERIVE_STATEMENT_2(ix, offset_1, offset_2, coeff_offset, sign, x, coeff, output, y) \
    output = fma(x[ix + (offset_1)] * y[ix + (offset_1)] sign x[ix - (offset_2)] * y[ix - (offset_2)], coeff[coeff_offset], output);
#define DERIVE_STATEMENT_1(ix, offset_1, offset_2, coeff_offset, sign, x, coeff, output) \
    output = fma(x[ix + (offset_1)] sign x[ix - (offset_2)], coeff[(coeff_offset)], output);

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME
#define DERIVE_STATEMENT(...) GET_MACRO(__VA_ARGS__, DERIVE_STATEMENT_2, DERIVE_STATEMENT_1)(__VA_ARGS__)

#define DERIVE_GENERIC_AXIS(ix, calculate_offset, offset_1, offset_2, sign, ...) \
    DERIVE_STATEMENT(ix, calculate_offset(offset_1, 0), calculate_offset(offset_2, 0), 0, sign, __VA_ARGS__) \
    if constexpr (HALF_LENGTH_ > 1) { \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 1), calculate_offset(offset_2, 1), 1, sign, __VA_ARGS__) \
    }   \
    if constexpr (HALF_LENGTH_ > 2) {   \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 2), calculate_offset(offset_2, 2), 2, sign, __VA_ARGS__) \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 3), calculate_offset(offset_2, 3), 3, sign, __VA_ARGS__) \
    }   \
    if constexpr (HALF_LENGTH_ > 4) {   \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 4), calculate_offset(offset_2, 4), 4, sign, __VA_ARGS__) \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 5), calculate_offset(offset_2, 5), 5, sign, __VA_ARGS__) \
    }   \
    if constexpr (HALF_LENGTH_ > 6) {   \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 6), calculate_offset(offset_2, 6), 6, sign, __VA_ARGS__) \
        DERIVE_STATEMENT(ix, calculate_offset(offset_1, 7), calculate_offset(offset_2, 7), 7, sign, __VA_ARGS__) \
    }

#define SEQ_OFFSET_CAL(offset, index)   (index + offset)
#define ARRAY_OFFSET_CAL(offset, index)   (offset[index])
#define JUMP_OFFSET_CALCULATION(factor)  factor * SEQ_OFFSET_CAL

#define DERIVE_SEQ_AXIS_EQ_OFF(index, offset, sign, ...) \
    DERIVE_GENERIC_AXIS(index, SEQ_OFFSET_CAL, offset, offset, sign, __VA_ARGS__)

#define DERIVE_ARRAY_AXIS_EQ_OFF(index, offset, sign, ...) \
    DERIVE_GENERIC_AXIS(index, ARRAY_OFFSET_CAL, offset, offset, sign, __VA_ARGS__)

#define DERIVE_JUMP_AXIS_EQ_OFF(index, jump, offset, sign, ...) \
    DERIVE_GENERIC_AXIS(index, JUMP_OFFSET_CALCULATION(jump), offset, offset, sign, __VA_ARGS__)

#define DERIVE_SEQ_AXIS(index, offset_1, offset_2, sign, ...) \
    DERIVE_GENERIC_AXIS(index, SEQ_OFFSET_CAL, offset_1, offset_2, sign, __VA_ARGS__)

#define DERIVE_ARRAY_AXIS(index, offset_1, offset_2, sign, ...) \
    DERIVE_GENERIC_AXIS(index, ARRAY_OFFSET_CAL, offset_1, offset_2, sign, __VA_ARGS__)

#define DERIVE_JUMP_AXIS(index, jump, offset_1, offset_2, sign, ...) \
    DERIVE_GENERIC_AXIS(index, JUMP_OFFSET_CALCULATION(jump), offset_1, offset_2, sign, __VA_ARGS__)


#endif //OPERATIONS_LIB_COMPONENTS_COMPUTATION_KERNELS_BASE_COMPUTATION_HELPERS_HPP
