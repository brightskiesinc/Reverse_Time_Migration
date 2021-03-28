//
// Created by amr-nasr on 18/11/2019.
//

#ifndef OPERATIONS_LIB_COMPONENTS_EXTENSIONS_ZERO_EXTENSION_HPP
#define OPERATIONS_LIB_COMPONENTS_EXTENSIONS_ZERO_EXTENSION_HPP

#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>

namespace operations {
    namespace components {
        namespace addons {

            class ZeroExtension : public Extension {
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
            };
        }//namespace addons
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_EXTENSIONS_ZERO_EXTENSION_HPP
