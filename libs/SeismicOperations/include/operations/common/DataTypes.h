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

#ifndef OPERATIONS_LIB_BASE_DATA_TYPES_H
#define OPERATIONS_LIB_BASE_DATA_TYPES_H

/**
 * @brief Axis definitions
 */
#define Y_AXIS 0
#define Z_AXIS 1
#define X_AXIS 2

#define OP_DIREC_FRONT      0   /* used to add boundary or padding at the axis front */
#define OP_DIREC_REAR       1   /* used to add boundary or padding at the axis rear */
#define OP_DIREC_BOTH       2   /* used to add boundary or padding at the axis front and rear */


/// Unsigned integer type to be used for strictly positive numbers.
typedef unsigned int uint;

/**
 * @brief Enums for the supported order lengths.
 *
 * @note Here we put values for each enum variable
 * if we make instance of HALF_LENGTH and assign it to O_2 for example
 * it will return 1;
 */
enum HALF_LENGTH {
    O_2 = 1, O_4 = 2, O_8 = 4, O_12 = 6, O_16 = 8
};

enum PHYSICS {
    ACOUSTIC, ELASTIC
};
enum APPROXIMATION {
    ISOTROPIC
};
enum EQUATION_ORDER {
    SECOND, FIRST
};
enum GRID_SAMPLING {
    UNIFORM, VARIABLE
};
enum INTERPOLATION {
    NONE, SPLINE, TRILINEAR
};
enum ALGORITHM {
    RTM, FWI, PSDM, PSTM
};

/**
 * @brief Struct describing the number of points in our grid.
 */
struct GridSize {
public:
    GridSize() = default;

    GridSize(uint _nx, uint _ny, uint _nz)
            : nx(_nx), ny(_ny), nz(_nz) {}

public:
    uint nx;
    uint nz;
    uint ny;
};

/**
 * @brief The step size in each direction.
 */
struct CellDimensions {
public:
    CellDimensions() = default;

    CellDimensions(float _dx, float _dy, float _dz)
            : dx(_dx), dy(_dy), dz(_dz) {}

public:
    float dx;
    float dz;
    float dy;
};

/**
 * @brief Point co-ordinates in 3D.
 */
struct Point3D {
public:
    Point3D() = default;

    Point3D(uint _x, uint _y, uint _z)
            : x(_x), y(_y), z(_z) {}

    // Copy constructor
    Point3D(const Point3D &P) {
        x = P.x;
        y = P.y;
        z = P.z;
    }

    void operator=(const Point3D &P) {
        x = P.x;
        y = P.y;
        z = P.z;
    }

    bool operator==(const Point3D &P) {
        bool value = false;
        value = ((x == P.x) && (y == P.y) && (z == P.z));

        return value;
    }

public:
    uint x;
    uint z;
    uint y;
};

/**
 * @brief Floating Point co-ordinates in 3D.
 */
struct FPoint3D {
public:
    FPoint3D() = default;

    FPoint3D(float _x, float _y, float _z)
            : x(_x), y(_y), z(_z) {}

    // Copy constructor
    FPoint3D(const FPoint3D &P) {
        x = P.x;
        y = P.y;
        z = P.z;
    }

    FPoint3D &operator=(const FPoint3D &P) = default;

    bool operator==(const FPoint3D &P) const {
        bool value = false;
        value = ((x == P.x) && (y == P.y) && (z == P.z));

        return value;
    }

public:
    float x;
    float z;
    float y;
};

/**
 * @brief Integer point in 3D.
 */
struct IPoint3D {
public:
    IPoint3D() = default;

    IPoint3D(int _x, int _y, int _z)
            : x(_x), y(_y), z(_z) {}

public:
    int x;
    int z;
    int y;
};

/**
 * @brief An object representing our full window size,
 * will not match grid size if a window model is applied.
 */
struct WindowProperties {
public:
    WindowProperties() = default;

    WindowProperties(uint _wnx, uint _wnz, uint _wny, Point3D _window_start)
            : wnx(_wnx), wny(_wny), wnz(_wnz), window_start(_window_start) {}

public:
    /**
     * @brief Will be set by traces to the appropriate
     * start of the velocity (including boundary).
     *
     * As the trace manager knows the shot location so it will change
     * the start of the window according to the shot location in each shot
     */
    Point3D window_start;

    /// Will be set by ModelHandler to match all parameters properties.
    uint wnx;
    uint wnz;
    uint wny;
};

#endif // OPERATIONS_LIB_BASE_DATA_TYPES_H
