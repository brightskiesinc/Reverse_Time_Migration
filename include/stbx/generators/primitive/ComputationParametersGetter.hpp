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

#ifndef SEISMIC_TOOLBOX_COMPUTATIONPARAMETERSGETTER_HPP
#define SEISMIC_TOOLBOX_COMPUTATIONPARAMETERSGETTER_HPP

#include <stbx/generators/common/Keys.hpp>
#include <operations/common/DataTypes.h>
#include <prerequisites/libraries/nlohmann/json.hpp>


namespace stbx {
    namespace generators {
        struct Window {
            int use_window = 0;
            int left_win = DEF_VAL;
            int right_win = DEF_VAL;
            int front_win = DEF_VAL;
            int back_win = DEF_VAL;
            int depth_win = DEF_VAL;
        };

        struct StencilOrder {
            int order = DEF_VAL;
            HALF_LENGTH half_length = O_8;
        };


        class ComputationParametersGetter {

        public:
            explicit ComputationParametersGetter(const nlohmann::json &aMap);

            ~ComputationParametersGetter() = default;

            Window GetWindow();

            StencilOrder GetStencilOrder();

            int GetBoundaryLength();

            float GetSourceFrequency();

            float GetDTRelaxed();

            int GetBlock(const std::string &direction);

            int GetIsotropicCircle();



        private:
            nlohmann::json mMap;
        };

    };// namespace generators
}//namesapce stbx


#endif //SEISMIC_TOOLBOX_COMPUTATIONPARAMETERSGETTER_HPP