//
// Created by zeyad-osama on 05/01/2021.
//

#ifndef SEISMIC_TOOLBOX_UTILS_CNV_CSV_TO_PNG_HPP
#define SEISMIC_TOOLBOX_UTILS_CNV_CSV_TO_PNG_HPP

#if (USE_OpenCV)

#include <string>

namespace stbx {
    namespace utils {
        namespace cnv {

            int csv_to_png(const std::string &dir, float percentile);

        }//namespace cnv
    }//namespace utils
}//namespace stbx

#endif

#endif //SEISMIC_TOOLBOX_UTILS_CNV_CSV_TO_PNG_HPP
