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

#ifndef OPERATIONS_LIB_COMPONENTS_EXTENSIONS_RANDOM_EXTENSION_HPP
#define OPERATIONS_LIB_COMPONENTS_EXTENSIONS_RANDOM_EXTENSION_HPP

#include <stdlib.h>
#include <random>
#include <operations/components/independents/concrete/boundary-managers/extensions/Extension.hpp>

namespace operations {
    namespace components {
        namespace addons {

            /**
             * @brief generates a random value based on the values of bound_l and x between 0 and 1
             * 
             */
#define GET_RANDOM_VALUE(bound_l, x) ((float) rand() / RAND_MAX) * ((float) (bound_l - (x)) / bound_l)
            /**
             * @brief generates a random float number between 0 and 1
             * 
             */
#define RANDOM_VALUE (float) rand() / RAND_MAX

            class RandomExtension : public Extension {

            public:
                RandomExtension(int aGrainSideLength) :
                        mGrainSideLength(aGrainSideLength) {}

            private:
                void VelocityExtensionHelper(float *apPropertyArray,
                                             int aStartX, int aStartY, int aStartZ,
                                             int aEndX, int aEndY, int aEndZ,
                                             int aNx, int aNy, int aNz,
                                             uint aBoundaryLength) override;

                void TopLayerExtensionHelper(float *apPropertyArray,
                                             int aStartX, int aStartY, int aStartZ,
                                             int aEndX, int aEndY, int aEndZ,
                                             int aNx, int aNy, int aNz,
                                             uint aBoundaryLength) override;

                void TopLayerRemoverHelper(float *apPropertyArray,
                                           int aStartX, int aStartY, int aStartZ,
                                           int aEndX, int aEndY, int aEndZ,
                                           int aNx, int aNy, int aNz,
                                           uint aBoundaryLength) override;

                /// Grain side length.
                int mGrainSideLength;
            };

        }//namespace addons
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_EXTENSIONS_RANDOM_EXTENSION_HPP
