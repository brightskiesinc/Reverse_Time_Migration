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

#ifndef OPERATIONS_LIB_COMPONENTS_RAY_TRACER_HPP
#define OPERATIONS_LIB_COMPONENTS_RAY_TRACER_HPP

#include <operations/components/independents/interface/Component.hpp>
#include <operations/components/dependents/primitive/MemoryHandler.hpp>
#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/holders/TravelTimeTable.hpp>

namespace operations {
    namespace components {

        /**
         * @brief
         */
        class RayTracer : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~RayTracer() {};

            /**
             * @brief
             */
            virtual void Map(std::vector<Point3D> vPoints3D) = 0;


            /**
             * @return[out]
             * The memory handler used.
             */
            MemoryHandler *GetMemoryHandler() {
                return this->mpMemoryHandler;
            }

            /**
             * @brief
             */
            virtual TravelTimeTable GetTravelTimeTable() = 0;

        protected:
            /// Memory Handler instance to be used for all memory
            /// handling (i.e. First touch)
            MemoryHandler *mpMemoryHandler;
        };
    }//namespace components
}//namespace operations

#endif //OPERATIONS_LIB_COMPONENTS_RAY_TRACER_HPP
