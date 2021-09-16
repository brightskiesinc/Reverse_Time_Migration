/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
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

#include <iostream>

#include <bs/base/exceptions/Exceptions.hpp>

using namespace bs::base::exceptions;


class ExceptionTest {
public:
    ExceptionTest() = default;

    void
    Foo() { throw NOT_IMPLEMENTED_EXCEPTION_WITH_MESSAGE("test message"); }

    void
    Bar() { throw UNSUPPORTED_FEATURE_EXCEPTION(); }
};

int main() {
    ExceptionTest test;
    /* Right catching. No termination. */
    try {
        test.Foo();
    } catch (NotImplementedException &e) {
        std::cout << e.what() << std::endl;
    }

    /* Right catching. No termination. */
    try {
        test.Bar();
    } catch (UnsupportedFeatureException &e) {
        std::cout << e.what() << std::endl;
    }

    /* Wrong catching. Termination encountered. */
    try {
        test.Bar();
    } catch (UndefinedException &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}