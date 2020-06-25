//
// Created by amr on 05/11/19.
//

#ifndef RTM_FRAMEWORK_DOUT_H
#define RTM_FRAMEWORK_DOUT_H

#include <iostream>

#ifndef NDEBUG
#define dout std::cout << __FILE__ << "(" << __LINE__ << ") DEBUG: "
#else
#define dout 0 && std::cout
#endif

#endif // RTM_FRAMEWORK_DOUT_H
