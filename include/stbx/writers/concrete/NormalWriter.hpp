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

#ifndef PIPELINE_WRITERS_NORMAL_WRITER_HPP
#define PIPELINE_WRITERS_NORMAL_WRITER_HPP

#include <stbx/writers/interface/Writer.hpp>

namespace stbx {
    namespace writers {

        class NormalWriter : public Writer {
        public:
            NormalWriter() = default;

            ~NormalWriter() override = default;

        private:
            void SpecifyRawMigration() override;

            void Initialize() override {};

            void PostProcess() override;
        };
    }//namespace writers
}//namespace stbx

#endif //PIPELINE_WRITERS_NORMAL_WRITER_HPP
