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

#ifndef OPERATIONS_LIB_DATA_UNITS_MIGRATION_MIGRATION_DATA_HPP
#define OPERATIONS_LIB_DATA_UNITS_MIGRATION_MIGRATION_DATA_HPP

#include <operations/data-units/interface/DataUnit.hpp>
#include <operations/data-units/concrete/migration/Result.hpp>
#include <operations/common/DataTypes.h>

#include <bs/io/data-units/concrete/Gather.hpp>

#include <bs/base/exceptions/Exceptions.hpp>

#include <utility>
#include <vector>

namespace operations {
    namespace dataunits {

        class MigrationData : public DataUnit {
        public:
            /**
             * @brief Constructor
             * @param[in] mig_type
             * @param[in] nx
             * @param[in] nz
             * @param[in] ny
             * @param[in] nt
             * @param[in] dx
             * @param[in] dz
             * @param[in] dy
             * @param[in] dt
             * @param[in] avResults
             */
            MigrationData(uint nx, uint ny, uint nz,
                          float dx, float dy, float dz,
                          bs::io::dataunits::Gather *apMetadataGather,
                          std::vector<Result *> avResults) :
                    MigrationData(nx, ny, nz, 1, dx, dy, dz,
                                  apMetadataGather, std::move(avResults)) {
            }

            MigrationData(uint nx, uint ny, uint nz,
                          uint gather_dimension,
                          float dx, float dy, float dz,
                          bs::io::dataunits::Gather *apMetadataGather,
                          std::vector<Result *> avResults) {
                this->mNX = nx;
                this->mNY = ny;
                this->mNZ = nz;

                this->mDX = dx;
                this->mDY = dy;
                this->mDZ = dz;

                this->mpMetadataGather = apMetadataGather;
                this->mvResults = std::move(avResults);

                this->mGatherDimension = gather_dimension;
            }

            /**
             * @brief Destructor.
             */
            ~MigrationData() override {
                for (auto const &result : this->mvResults) {
                    delete result;
                }
            }

            /**
             * @brief Grid size per axe getter.
             * @param[in] axis      Axe direction
             * @return[out] value   Value
             */
            uint GetGridSize(uint axis) const {
                if (bs::base::exceptions::is_out_of_range(axis)) {
                    throw bs::base::exceptions::AXIS_EXCEPTION();
                }

                uint val;
                if (axis == Y_AXIS) {
                    val = this->mNY;
                } else if (axis == Z_AXIS) {
                    val = this->mNZ;
                } else if (axis == X_AXIS) {
                    val = this->mNX;
                }
                return val;
            }

            /**
             * @brief Cell dimensions per axe getter.
             * @param[in] axis      Axe direction
             * @return[out] value   Value
             */
            float GetCellDimensions(uint axis) const {
                if (bs::base::exceptions::is_out_of_range(axis)) {
                    throw bs::base::exceptions::AXIS_EXCEPTION();
                }

                float val;
                if (axis == Y_AXIS) {
                    val = this->mDY;
                } else if (axis == Z_AXIS) {
                    val = this->mDZ;
                } else if (axis == X_AXIS) {
                    val = this->mDX;
                }
                return val;
            }

            /**
             * @brief Grid size per axe setter.
             * @param[in] axis      Axe direction
             * @param[in] value   Value
             */
            void SetGridSize(uint axis, uint value) {
                if (bs::base::exceptions::is_out_of_range(axis)) {
                    throw bs::base::exceptions::AXIS_EXCEPTION();
                }
                if (axis == Y_AXIS) {
                    this->mNY = value;
                } else if (axis == Z_AXIS) {
                    this->mNZ = value;
                } else if (axis == X_AXIS) {
                    this->mNX = value;
                }
            }

            /**
             * @brief Cell dimensions per axe getter.
             * @param[in] axis      Axe direction
             * @param[in] value     Value
             */
            void SetCellDimensions(uint axis, float value) {
                if (bs::base::exceptions::is_out_of_range(axis)) {
                    throw bs::base::exceptions::AXIS_EXCEPTION();
                }
                if (axis == Y_AXIS) {
                    this->mDY = value;
                } else if (axis == Z_AXIS) {
                    this->mDZ = value;
                } else if (axis == X_AXIS) {
                    this->mDX = value;
                }
            }

            void SetResults(uint index, Result *apResult) {
                this->mvResults[index] = apResult;
            }

            std::vector<Result *> GetResults() const {
                return this->mvResults;
            }

            Result *GetResultAt(uint index) const {
                return this->mvResults[index];
            }

            uint GetGatherDimension() const {
                return this->mGatherDimension;
            }

            bs::io::dataunits::Gather *GetMetadataGather() const {
                return this->mpMetadataGather;
            }

        private:
            uint mNX;
            uint mNY;
            uint mNZ;

            float mDX;
            float mDY;
            float mDZ;

            bs::io::dataunits::Gather *mpMetadataGather;

            std::vector<Result *> mvResults;

            /**
            * @brief The extra dimension is supposed to carry the number of angles in angle
            * domain common image gathers or number of offsets in offset domain common
            * image gathers, etc
            */
            uint mGatherDimension;
        };
    } //namespace dataunits
} //namespace operations

#endif // OPERATIONS_LIB_DATA_UNITS_MIGRATION_MIGRATION_DATA_HPP
