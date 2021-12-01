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

#ifndef OPERATIONS_LIB_COMPONENTS_EXTENSIONS_HOMOGENOUS_EXTENSION_HPP
#define OPERATIONS_LIB_COMPONENTS_EXTENSIONS_HOMOGENOUS_EXTENSION_HPP

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>

namespace operations {
    namespace components {
        namespace addons {

            class HomogenousExtension : public Extension {
            public:
                HomogenousExtension(bool use_top_layer = true);

            private:
                void VelocityExtensionHelper(float *property_array,
                                             int start_x, int start_y, int start_z,
                                             int end_x, int end_y, int end_z,
                                             int nx, int ny, int nz,
                                             uint boundary_length) override;

                void TopLayerExtensionHelper(float *property_array,
                                             int start_x, int start_y, int start_z,
                                             int end_x, int end_y, int end_z,
                                             int nx, int ny, int nz,
                                             uint boundary_length) override;

                void TopLayerRemoverHelper(float *property_array,
                                           int start_x, int start_y, int start_z,
                                           int end_x, int end_y, int end_z,
                                           int nx, int ny, int nz,
                                           uint boundary_length) override;

            private:
                bool mUseTop;
            };
        }//namespace addons
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_EXTENSIONS_HOMOGENOUS_EXTENSION_HPP
