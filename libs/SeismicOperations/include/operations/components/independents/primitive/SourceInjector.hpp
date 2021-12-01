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

#ifndef OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTOR_HPP
#define OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTOR_HPP

#include <operations/components/independents/interface/Component.hpp>

#include <operations/common/DataTypes.h>

namespace operations {
    namespace components {

        /**
         * @brief Source Injector Interface. All concrete techniques for source
         * injection should be implemented using this interface.
         */
        class SourceInjector : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~SourceInjector() {};

            /**
             * @brief Sets the source point to apply the injection to it.
             *
             * @param[in] source_point
             * The designated source point.
             */
            virtual void SetSourcePoint(Point3D *source_point) = 0;

            /**
             * @brief Apply source injection to the wave field(s). It should inject
             * the current frame in our grid with the appropriate value. It should be
             * responsible of controlling when to stop the injection.
             *
             * @param[in] time_step
             * The time step corresponding to the current frame.
             */
            virtual void ApplySource(int time_step) = 0;

            /**
             * @brief Applies Isotropic Field upon all parameters' models
             *
             * @see ComputationParameters->GetIsotropicRadius()
             */
            virtual void ApplyIsotropicField() = 0;

            /**
             * @brief Reverts Isotropic Field upon all parameters' models
             *
             * @see ComputationParameters->GetIsotropicRadius()
             */
            virtual void RevertIsotropicField() = 0;

            /**
             * @brief Gets the time step that the source injection would stop after.
             *
             * @return[out]
             * An integer indicating the time step at which the source injection
             * would stop.
             */
            virtual int GetCutOffTimeStep() = 0;

            /**
             * @brief
             * Get the number of negative timesteps to be done before
             * the propagation.
             *
             * @return
             * The number of timesteps that are done before the 0 time.
             */
            virtual int GetPrePropagationNT() = 0;

            /**
             * @brief Gets the maximum frequency of the injector wavelet.
             *
             * @return[out]
             * A floating point number indicating the maximum frequency contained in the injector
             * wavelet. Notice that this is the maximum frequency, which is not necessarily
             * the peak frequency(often used and defined as source frequency).
             */
            virtual float GetMaxFrequency() = 0;

        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_SOURCE_INJECTOR_HPP
