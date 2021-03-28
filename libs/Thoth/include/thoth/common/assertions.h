//
// Created by zeyad-osama on 21/01/2021.
//

#ifndef THOTH_COMMON_ASSERTIONS_TPP
#define THOTH_COMMON_ASSERTIONS_TPP

#include <type_traits>

#define ASSERT_IS_POD(ARG)             std::is_pod<ARG>::value
#define ASSERT_IS_STR(ARG)             std::is_base_of<std::string, ARG>::value

#define ASSERT_T_TEMPLATE(ARG) \
static_assert(ASSERT_IS_POD(ARG) || ASSERT_IS_STR(ARG), "T type is not compatible")

#endif //THOTH_COMMON_ASSERTIONS_TPP
