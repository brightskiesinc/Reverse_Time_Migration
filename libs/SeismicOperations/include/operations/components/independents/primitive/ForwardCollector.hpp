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

#ifndef OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTOR_HPP
#define OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTOR_HPP

#include "operations/components/independents/interface/Component.hpp"

#include "operations/common/DataTypes.h"

namespace operations {
    namespace components {

        /**
         * @example
         * 3 Way Propagation:
         * <ol>
         *      <li>
         *      Reset Grid:
         *      <br>
         *      Before forward when it is called it just resetting the array (set it by zeros)
         *      </li>
         *      <li>
         *      Save Forward:
         *      <br>
         *      Called at each time step do no thing as it is 3 propagation.
         *      </li>
         *      <li>
         *      Reset Grid:
         *      <br>
         *      Consider 2 GridBox one for backward and the other for reverse.
         *      </li>
         *      <li>
         *      Fetch Forward:
         *      <br>
         *      Called at each time step to do internal computation kernel calculations to
         *      the corresponding time step (reverse propagation)
         *      </li>
         *      <li>
         *      Get Forward Grid:
         *      <br>
         *      Called at each time step  to return the internal GridBox.
         *      </li>
         * </ol>
         *
         * @note
         * Correlation work on <b>current pressure</b>.
         */

        /**
         * @brief Forward Collector Interface. All concrete techniques for saving the
         * forward propagation or restoring the results from it should be implemented
         * using this interface.
         */
        class ForwardCollector : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~ForwardCollector() {};

            /**
             * @brief This function is called on each time step in the backward propagation
             * and should restore (2 propagation) or set (3 propagation -reverse) the
             * ForwardGrid variable to the suitable time step. The first call is expected
             * to make ForwardGrid current frame point to the frame corresponding to time
             * nt - 2.
             */
            virtual void FetchForward() = 0;

            /**
             * @brief This function is called on each time step in the forward propagation
             * and should store the current time step or save it in such a way that it can be
             * later obtained internally and stored in the ForwardGrid option. The first
             * call to this function happens when current frame in given grid is at
             * time 1. update pointers to save it in the right place in case of 2
             * propagation it will save in memory
             */
            virtual void SaveForward() = 0;

            /**
             * @brief Resets the grid pressures or allocates new frames for the backward
             * propagation. It should either free or keep track of the old frames.
             * It is called one time per shot before forward or backward propagation
             *
             * @param[in] aIsForwardRun
             * Indicates whether we are resetting the grid frame for the start of the
             * run (Forward) or between the propagations (Before Backward).
             * <br>
             * <br>
             * <b>True</b> indicates it is before the forward.
             * <br>
             * <b>False</b> indicates before backward.
             */
            virtual void ResetGrid(bool aIsForwardRun) = 0;

            /**
             * @brief Getter to the internal GridBox to contain the current time step of
             * the forward propagation. This will be accessed to be able to correlate the
             * results in the backward propagation.
             *
             * @return[out]
             * GridBox containing the appropriate values for the time that is implicitly
             * calculated using fetch forward. this function is called on each time step.
             */
            virtual dataunits::GridBox *GetForwardGrid() = 0;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_FORWARD_COLLECTOR_HPP
