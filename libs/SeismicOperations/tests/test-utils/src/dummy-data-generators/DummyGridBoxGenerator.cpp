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


#include <operations/test-utils/dummy-data-generators/DummyGridBoxGenerator.hpp>

using namespace operations::dataunits;


GridBox *generate_grid_box_no_wind_2d();

GridBox *generate_grid_box_inc_wind_2d();

GridBox *generate_grid_box_no_wind_3d();

GridBox *generate_grid_box_inc_wind_3d();

GridBox *operations::testutils::generate_grid_box(
        OP_TU_DIMS aDims, OP_TU_WIND aWindow) {
    GridBox *gb = nullptr;
    if (aDims == OP_TU_2D && aWindow == OP_TU_NO_WIND) {
        gb = generate_grid_box_no_wind_2d();
    } else if (aDims == OP_TU_2D && aWindow == OP_TU_INC_WIND) {
        gb = generate_grid_box_inc_wind_2d();
    } else if (aDims == OP_TU_3D && aWindow == OP_TU_NO_WIND) {
        gb = generate_grid_box_no_wind_3d();
    } else if (aDims == OP_TU_3D && aWindow == OP_TU_INC_WIND) {
        gb = generate_grid_box_inc_wind_3d();
    }
    return gb;
}

GridBox *generate_grid_box_no_wind_2d() {
    /*
     * Variables initialization for grid box.
     */

    int hl = 4;

    int nx = 23;
    int ny = 1;
    int nz = 23;

    int wnx = nx;
    int wny = ny;
    int wnz = nz;

    int sx = 0;
    int sy = 0;
    int sz = 0;

    float dx = 6.25f;
    float dy = 0.0f;
    float dz = 6.25f;

    float dt = 0.00207987f;

    auto *grid_box = new GridBox();

    /*
     * Setting variables inside grid box.
     */

    grid_box->SetAfterSamplingAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetInitialAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetWindowAxis(new Axis3D<unsigned int>(wnx, wny, wnz));
    grid_box->GetAfterSamplingAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetCellDimension(dz);
    grid_box->GetInitialAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetInitialAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetInitialAxis()->GetZAxis().SetCellDimension(dz);

    grid_box->GetInitialAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetInitialAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetInitialAxis()->GetZAxis().SetReferencePoint(sz);

    grid_box->GetAfterSamplingAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetReferencePoint(sz);


    grid_box->SetDT(dt);

    return grid_box;
}

GridBox *generate_grid_box_inc_wind_2d() {
    /*
    * Variables initialization for grid box.
    */

    int hl = 4;

    int nx = 23;
    int ny = 1;
    int nz = 23;

    int wnx = 21;
    int wny = 1;
    int wnz = 21;

    int sx = 0;
    int sy = 0;
    int sz = 0;

    float dx = 6.25f;
    float dy = 0.0f;
    float dz = 6.25f;

    float dt = 0.00207987f;

    auto *grid_box = new GridBox();

    /*
     * Setting variables inside grid box.
     */

    grid_box->SetAfterSamplingAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetInitialAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetWindowAxis(new Axis3D<unsigned int>(wnx, wny, wnz));
    grid_box->GetAfterSamplingAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetCellDimension(dz);
    grid_box->GetInitialAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetInitialAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetInitialAxis()->GetZAxis().SetCellDimension(dz);

    grid_box->GetInitialAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetInitialAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetInitialAxis()->GetZAxis().SetReferencePoint(sz);

    grid_box->GetAfterSamplingAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetReferencePoint(sz);


    grid_box->SetDT(dt);

    return grid_box;
}

GridBox *generate_grid_box_no_wind_3d() {
    /*
    * Variables initialization for grid box.
    */

    int hl = 4;

    int nx = 23;
    int ny = 23;
    int nz = 23;

    int wnx = nx;
    int wny = ny;
    int wnz = nz;

    int sx = 0;
    int sy = 0;
    int sz = 0;

    float dx = 6.25f;
    float dy = 6.25f;
    float dz = 6.25f;

    float dt = 0.00169821f;

    auto *grid_box = new GridBox();

    /*
     * Setting variables inside grid box.
     */
    grid_box->SetAfterSamplingAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetInitialAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetWindowAxis(new Axis3D<unsigned int>(wnx, wny, wnz));
    grid_box->GetAfterSamplingAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetCellDimension(dz);
    grid_box->GetInitialAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetInitialAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetInitialAxis()->GetZAxis().SetCellDimension(dz);

    grid_box->GetInitialAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetInitialAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetInitialAxis()->GetZAxis().SetReferencePoint(sz);

    grid_box->GetAfterSamplingAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetReferencePoint(sz);


    grid_box->SetDT(dt);

    return grid_box;
}

GridBox *generate_grid_box_inc_wind_3d() {
    /*
    * Variables initialization for grid box.
    */

    int hl = 4;

    int nx = 23;
    int ny = 23;
    int nz = 23;

    int wnx = 21;
    int wny = 21;
    int wnz = 21;

    int sx = 0;
    int sy = 0;
    int sz = 0;

    float dx = 6.25f;
    float dy = 6.25f;
    float dz = 6.25f;

    float dt = 0.00169821;

    auto *grid_box = new GridBox();

    /*
     * Setting variables inside grid box.
     */

    grid_box->SetAfterSamplingAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetInitialAxis(new Axis3D<unsigned int>(nx, ny, nz));
    grid_box->SetWindowAxis(new Axis3D<unsigned int>(wnx, wny, wnz));
    grid_box->GetAfterSamplingAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetCellDimension(dz);
    grid_box->GetInitialAxis()->GetXAxis().SetCellDimension(dx);
    grid_box->GetInitialAxis()->GetYAxis().SetCellDimension(dy);
    grid_box->GetInitialAxis()->GetZAxis().SetCellDimension(dz);

    grid_box->GetInitialAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetInitialAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetInitialAxis()->GetZAxis().SetReferencePoint(sz);

    grid_box->GetAfterSamplingAxis()->GetXAxis().SetReferencePoint(sx);
    grid_box->GetAfterSamplingAxis()->GetYAxis().SetReferencePoint(sy);
    grid_box->GetAfterSamplingAxis()->GetZAxis().SetReferencePoint(sz);

    grid_box->SetDT(dt);

    return grid_box;
}
