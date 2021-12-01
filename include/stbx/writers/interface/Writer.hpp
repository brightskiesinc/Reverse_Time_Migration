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

#ifndef PIPELINE_WRITERS_WRITER_HPP
#define PIPELINE_WRITERS_WRITER_HPP

#include <fstream>
#include <sys/stat.h>

#include <operations/engines/concrete/RTMEngine.hpp>
#include <operations/common/DataTypes.h>
#include <operations/utils/io/write_utils.h>
#include <operations/utils/filters/noise_filtering.h>

#include <bs/base/configurations/concrete/JSONConfigurationMap.hpp>
#include <bs/timer/api/cpp/BSTimer.hpp>
#include <bs/io/api/cpp/BSIO.hpp>

using namespace bs::timer;
using namespace bs::timer::configurations;

namespace stbx {
    namespace writers {
        /**
         * @brief Writer class is used to write various types
         * of formats for a given migration data
         *
         * @note Should be inherited whenever a new approach is used
         */
        class Writer {
        public:
            /**
             * @brief Constructor could be overridden to
             * initialize needed  member variables.
             */
            Writer() : mFilteredMigration(nullptr), mRawMigration(nullptr), mpMigrationData(nullptr) {
                this->mOutputTypes = {"binary", "segy"};
            }

            /**
             * @brief Destructor should be overridden to
             * ensure correct memory management.
             */
            virtual ~Writer() {
                delete[] mRawMigration;
            };

            /**
             * @brief Assign initialized engine to agent to use in
             * all functions.
             * @param aEngine : RTMEngine
             */
            inline void AssignMigrationData(operations::dataunits::MigrationData *apMigrationData) {
                this->mpMigrationData = apMigrationData;
            }

            virtual void Write(const std::string &aWritePath, bool is_traces = false) {
                mkdir(aWritePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                this->SpecifyRawMigration();
                this->PostProcess();
                this->Filter();
                this->WriteFrame(mRawMigration, aWritePath + "/raw_migration");
                this->WriteFrame(mFilteredMigration, aWritePath + "/filtered_migration");
                this->WriteTimeResults(aWritePath);
            }

            /**
            * @brief  Writes time results performed by Timer.
            * @param aWritePath : string     File name to be written.
            */
            void WriteTimeResults(const std::string &aWritePath) {
                std::vector<std::ostream *> streams;
                streams.emplace_back(&std::cout);
                std::ofstream ofs;
                ofs.open(aWritePath + "/timing_results.txt");
                streams.emplace_back(&ofs);
                TimerManager::GetInstance()->Report(streams);
                ofs.close();
            }

        protected:
            /**
             * @brief Initialize member variables assigned to
             * each derived class.
             */
            virtual void Initialize() = 0;

            /**
             * @brief Preprocess migration data before starting
             * to write.
             */
            virtual void PostProcess() = 0;

            /**
             * @brief Extracts migration results from provided
             * Migration Data.
             * @return Migration results
             */
            virtual void SpecifyRawMigration() {};

            /**
             * @brief  Filter migration data into
             * @note *mpMigrationData will be internally used
             * @param frame : float *       Frame to be filtered
             */
            virtual void Filter() {
                uint nx = this->mpMigrationData->GetGridSize(X_AXIS);
                uint ny = this->mpMigrationData->GetGridSize(Y_AXIS);
                uint nz = this->mpMigrationData->GetGridSize(Z_AXIS);

                mFilteredMigration = new float[nx * ny * nz];

                {
                    ScopeTimer t("Writer::FilterMigration");
                    operations::utils::filters::filter_stacked_correlation(
                            this->mRawMigration,
                            this->mFilteredMigration,
                            nx, ny, nz,
                            this->mpMigrationData->GetCellDimensions(X_AXIS),
                            this->mpMigrationData->GetCellDimensions(Y_AXIS),
                            this->mpMigrationData->GetCellDimensions(Z_AXIS));
                }
            };

            /**
             * @brief  Writes migration data into the specified format
             * @note *mpMigrationData will be internally used
             * @param file_name : string     File name to be written.
             * @param is_traces : bool       Check
             */
            virtual void WriteFrame(float *frame, const std::string &file_name, uint shots = 1) {
                uint ns;
                float ds;
                float sample_rate;
                ns = this->mpMigrationData->GetGridSize(Z_AXIS);
                ds = this->mpMigrationData->GetCellDimensions(Z_AXIS);
                sample_rate = 1e3;
                auto gathers = operations::utils::io::TransformToGather(
                        frame,
                        this->mpMigrationData->GetGridSize(X_AXIS),
                        this->mpMigrationData->GetGridSize(Y_AXIS),
                        ns,
                        ds,
                        this->mpMigrationData->GetMetadataGather(),
                        shots,
                        sample_rate
                );
                for (auto &output_type : this->mOutputTypes) {
                    std::string path = file_name;
                    bs::base::configurations::JSONConfigurationMap map({});
                    bs::io::streams::SeismicWriter writer(
                            bs::io::streams::SeismicWriter::ToWriterType(output_type), &map);
                    writer.AcquireConfiguration();
                    writer.Initialize(path);
                    writer.Write(gathers);
                    writer.Finalize();
                }
                for (auto g : gathers) {
                    delete g;
                }
            }

        protected:
            /// Engine instance needed by agent to preform task upon
            operations::dataunits::MigrationData *mpMigrationData;
            float *mFilteredMigration;
            float *mRawMigration;
            std::vector<std::string> mOutputTypes;
        };

    }//namespace writers
}//namespace stbx

#endif //PIPELINE_WRITERS_WRITER_HPP
