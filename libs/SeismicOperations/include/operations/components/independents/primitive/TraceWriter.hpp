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
#ifndef OPERATIONS_LIB_COMPONENTS_TRACE_WRITER_HPP
#define OPERATIONS_LIB_COMPONENTS_TRACE_WRITER_HPP

#include "operations/data-units/concrete/holders/TracesHolder.hpp"
#include "operations/components/independents/interface/Component.hpp"

namespace operations {
    namespace components {

        /**
         * @brief This class is used to Record at each time step the
         * pressure at the surface in the places we want to have receivers on.
         * Can be considered as a hydrophone.
         */
        class TraceWriter : public Component {
        public:
            /**
             * @brief Destructors should be overridden to ensure correct memory management.
             */
            virtual ~TraceWriter() {};

            /**
             * @brief
             * Signals the start of a recording sequence
             */
            virtual void StartRecordingInstance(
                    operations::dataunits::TracesHolder &aTracesHolder) = 0;

            /**
             * @brief Records the traces from the domain according to the
             * configuration given in the initialize function.
             */
            virtual void RecordTrace(uint time_step) = 0;

            /**
             * @brief
             * Signals the end of a recording sequence
             *
             * @param[in] shot_id
             * The shot id this recording is assigned.
             */
            virtual void FinishRecordingInstance(uint shot_id) = 0;

            /**
             * @brief
             * Finalize the trace writer.
             */
            virtual void Finalize() = 0;
        };
    }//namespace components
}//namespace operations

#endif // OPERATIONS_LIB_COMPONENTS_TRACE_WRITER_HPP
