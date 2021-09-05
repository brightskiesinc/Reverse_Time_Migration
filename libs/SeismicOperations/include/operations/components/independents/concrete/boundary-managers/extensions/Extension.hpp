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

#ifndef OPERATIONS_LIB_COMPONENTS_EXTENSIONS_EXTENSION_HPP
#define OPERATIONS_LIB_COMPONENTS_EXTENSIONS_EXTENSION_HPP

#include <operations/data-units/concrete/holders/GridBox.hpp>

namespace operations {
    namespace components {
        namespace addons {

            /**
             * @brief Class used for the extensions of velocities for full or window model.
             * Provides standard procedures such as velocity backup and restoring,
             * and logic controlling the extensions however leaves the actual extensions
             * function to subclasses to allow different implementations.
             */
            class Extension {
            public:
                /**
                 * @brief Constructor
                 */
                Extension();

                /**
                 * @brief Destructor
                 */
                ~Extension();

                /**
                 * @brief  Extend the velocities at boundaries.
                 */
                void ExtendProperty();

                /**
                 * @brief In case of window mode extend the velocities at boundaries, re-extend the
                 * top layer velocities.
                 */
                void ReExtendProperty();

                /**
                 * @brief Zeros out the top layer in preparation of the backward propagation.
                 */
                void AdjustPropertyForBackward();

                /**
                 * @brief Sets the half length padding used in the extensions operations.
                 */
                void SetHalfLength(uint aHalfLength);

                /**
                 * @brief Sets the boundary length used in the extensions operations.
                 */
                void SetBoundaryLength(uint aBoundaryLength);

                /**
                 * @brief Sets the grid box that the operations are ran on.
                 */
                void SetGridBox(dataunits::GridBox *apGridBox);

                /**
                 * @brief Sets the property that will be extended by this extensions object.
                 */
                void SetProperty(float *property, float *window_property);

            private:
                /**
                 * @brief The original_array extensions actual function, will be called in the extend
                 * original_array and ReExtend original_array functions.
                 * This function will be overridden by each subclass of extensions providing
                 * different ways of extending the original_array like extending it by zeros,
                 * randomly or otherwise.
                 */
                virtual void VelocityExtensionHelper(float *original_array,
                                                     int start_x, int start_y, int start_z,
                                                     int end_x, int end_y, int end_z,
                                                     int nx, int ny, int nz,
                                                     uint boundary_length) = 0;

                /**
                 * @brief A helper function responsible of the extensions of the velocities for only
                 * the top layer. Called in the ReExtend original_array function. This
                 * function will be overridden by each subclass of extensions providing
                 * different ways of extending the original_array like extending it by zeros,
                 * randomly or otherwise.
                 */
                virtual void TopLayerExtensionHelper(float *original_array,
                                                     int start_x, int start_y, int start_z,
                                                     int end_x, int end_y, int end_z,
                                                     int nx, int ny, int nz,
                                                     uint boundary_length) = 0;

                /**
                 * @brief A helper function responsible of the removal of the velocities for only the
                 * top layer. Called in the Adjust original_array for backward function. This
                 * function will be overridden by each subclass of extensions providing
                 * different ways of the removal of the top layer boundary.
                 */
                virtual void TopLayerRemoverHelper(float *original_array,
                                                   int start_x, int start_y, int start_z,
                                                   int end_x, int end_y, int end_z,
                                                   int nx, int ny, int nz,
                                                   uint boundary_length) = 0;


            private:
                /// Grid to extend its velocities/properties.
                dataunits::GridBox *mpGridBox = nullptr;
                /// Property to be extended by an extensions object.
                float *mProperties = nullptr;
                /// Window property array to be used if provided for moving window.
                float *mpWindowProperties = nullptr;
                /// Boundary length.
                uint mBoundaryLength;
                /// Half length of the used kernel.
                uint mHalfLength;
            };
        }//namespace addons
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_EXTENSIONS_EXTENSION_HPP
