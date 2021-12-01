/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_IO_STREAMS_SU_READER_HPP
#define BS_IO_STREAMS_SU_READER_HPP

#include <bs/io/streams/primitive/Reader.hpp>

namespace bs {
    namespace io {
        namespace streams {
            /**
             * @brief
             */
            class SUReader : public Reader {
            public:
                /**
                 * @brief Constructor
                 */
                explicit SUReader(bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Destructor
                 */
                ~SUReader() override;
            };

        } //streams
    } //thoth
} //namespace bs

#endif //BS_IO_STREAMS_SU_READER_HPP
