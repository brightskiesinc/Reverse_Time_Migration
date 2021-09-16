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

#ifndef PIPELINE_WRITERS_ADCIG_WRITER_HPP
#define PIPELINE_WRITERS_ADCIG_WRITER_HPP

#include <stbx/writers/interface/Writer.hpp>

namespace stbx {
    namespace writers {

        class ADCIGWriter : public Writer {
        public:
            ADCIGWriter();

            ~ADCIGWriter() override;

            void Write(const std::string &write_path, bool is_traces) override;

        private:
            void Initialize() override;

            void PostProcess() override {};

            void Filter() override;

            void SpecifyRawMigration() override {};

            void PrepareResults();

            void WriteSegyIntervals(float *frame, const std::string &file_name);

            void WriteCIG(float *frame, const std::string &file_name);

        private:
            float *mRawMigrationIntervals;

            float *mFilteredMigrationIntervals;

            float *mRawMigrationStacked;

            float *mFilteredMigrationStacked;

            int mIntervalLength = 25;
        };
    }//namespace writers
}//namespace stbx

#endif //PIPELINE_WRITERS_ADCIG_WRITER_HPP
