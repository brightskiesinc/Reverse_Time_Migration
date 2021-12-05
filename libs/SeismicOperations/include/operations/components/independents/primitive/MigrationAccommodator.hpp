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

#ifndef OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATOR_HPP
#define OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATOR_HPP

#include <operations/components/independents/interface/Component.hpp>

#include <operations/common/DataTypes.h>
#include <operations/data-units/concrete/migration/MigrationData.hpp>

namespace operations {
    namespace components {

        /**
         * @brief Enum to hold illumination compensation categories.
         * - No compensation will provide normal cross correlation
         * - Source compensation will compensate the cross correlation stack for the source illumination effect
         * - Receiver compensation will compensate the cross correlation stack for the receiver illumination effect
         * - Combined compensation will compensate the cross correlation stack for both the source and the receiver illumination effect
         */
        enum COMPENSATION_TYPE {
            NO_COMPENSATION,
            SOURCE_COMPENSATION,
            RECEIVER_COMPENSATION,
            COMBINED_COMPENSATION
        };

        /**
         * @brief Migration Accommodator Interface. All concrete techniques for the
         * imaging conditions should be implemented using this interface.
         */
        class MigrationAccommodator : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~MigrationAccommodator() {};

            /**
             * @brief Resets the single shot correlation results.
             */
            virtual void ResetShotCorrelation() = 0;

            /**
             * @brief Stacks the single shot correlation current result
             * into the stacked shot correlation.
             */
            virtual void Stack() = 0;

            /**
             * @brief Correlates two frames using the dimensions provided in the
             * meta data using an imaging condition and stacks/writes them in the
             * single shot correlation result.
             *
             * @param apDataUnit[in]
             * The pointer to the GridBox containing the first frame to be correlated.
             * This should be the simulation or forward propagation result. Our frame of
             * interest is the current one.
             */
            virtual void Correlate(dataunits::DataUnit *apDataUnit) = 0;

            /**
             * @return
             * The pointer to the array that should contain the results of the correlation
             * of the frames of a single shot.
             */
            virtual dataunits::FrameBuffer<float> *GetShotCorrelation() = 0;

            /**
             * @return
             * The pointer to the array that should contain the results of the stacked
             * correlation of the frames of all shots.
             */
            virtual dataunits::FrameBuffer<float> *GetStackedShotCorrelation() = 0;

            /**
             * @return
             * The pointer to the array that should contain the final results and
             * details of the stacked correlation of the frames of all shots.
             */
            virtual dataunits::MigrationData *GetMigrationData() = 0;

            /**
             * @brief Sets the compensation category according to CompensationType Enum
             *
             * @param in_1
             * the compensation type that the correlation kernel would output at
             * the end of migration
             */
            virtual void SetCompensation(COMPENSATION_TYPE aCOMPENSATION_TYPE) = 0;

            /**
             * @brief Assigned the current processed trace's source point to this
             * Migration Accommodator for further usage.
             *
             * @param aSourcePoint
             */
            virtual void SetSourcePoint(Point3D *apSourcePoint) = 0;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_MIGRATION_ACCOMMODATOR_HPP
