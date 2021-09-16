/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS Base Package.
 *
 * BS Base Package is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS Base Package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_BASE_EXCEPTIONS_EXCEPTIONS_HPP
#define BS_BASE_EXCEPTIONS_EXCEPTIONS_HPP

/*
 * Exceptions interface.
 */

#include <bs/base/exceptions/interface/BaseException.hpp>

/*
 * Concrete exceptions.
 */

#include <bs/base/exceptions/concrete/AxisException.hpp>
#include <bs/base/exceptions/concrete/DeviceNoSpaceException.hpp>
#include <bs/base/exceptions/concrete/DeviceNotFoundException.hpp>
#include <bs/base/exceptions/concrete/DirectionException.hpp>
#include <bs/base/exceptions/concrete/FileNotFoundException.hpp>
#include <bs/base/exceptions/concrete/IllogicalException.hpp>
#include <bs/base/exceptions/concrete/IndexOutOfBoundsException.hpp>
#include <bs/base/exceptions/concrete/NoKeyFoundException.hpp>
#include <bs/base/exceptions/concrete/InvalidKeyValueException.hpp>
#include <bs/base/exceptions/concrete/NotImplementedException.hpp>
#include <bs/base/exceptions/concrete/NullPointerException.hpp>
#include <bs/base/exceptions/concrete/UndefinedException.hpp>
#include <bs/base/exceptions/concrete/UnsupportedFeatureException.hpp>

#endif //BS_BASE_EXCEPTIONS_EXCEPTIONS_HPP
