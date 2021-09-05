/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of Thoth (I/O Library).
 *
 * Thoth (I/O Library) is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Thoth (I/O Library) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bs/io/streams/concrete/writers/ImageWriter.hpp>

#include <bs/io/configurations/MapKeys.h>
#include <bs/base/common/ExitCodes.hpp>

#include <cmath>
#include <vector>

#ifdef USING_OPENCV

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#endif

using namespace bs::io::streams;
using namespace std;


ImageWriter::ImageWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap) {
    this->mpConfigurationMap = apConfigurationMap;
    this->mPercentile = 100;
}

void
ImageWriter::AcquireConfiguration() {
    this->mPercentile = this->mpConfigurationMap->GetValue(IO_K_PROPERTIES,
                                                           IO_K_PERCENTILE,
                                                           this->mPercentile);
}

float *
ImageWriter::NormalizeArrayByPercentile(const float *apInputArray,
                                        int aWidth,
                                        int aHeight,
                                        float aPercentile) {
    float max_val = -std::numeric_limits<float>::max();
    vector<float> values;
    auto normalized_image = new float[aWidth * aHeight];
    values.push_back(0);
    for (int i = 0; i < aWidth * aHeight; i++) {
        if (apInputArray[i] != 0) {
            values.push_back(fabs(apInputArray[i]));
        }
    }
    std::sort(values.begin(), values.end());
    int index = ((aPercentile / 100.0) * (values.size() - 1));
    if (index < 0) {
        index = 0;
    }
    max_val = values[index];
    for (int i = 0; i < aWidth * aHeight; i++) {
        if (fabs(apInputArray[i]) > max_val) {
            if (apInputArray[i] > 0) {
                normalized_image[i] = max_val;
            } else {
                normalized_image[i] = -max_val;
            }
        } else {
            normalized_image[i] = apInputArray[i];
        }
    }
    return normalized_image;
}

int
ImageWriter::WriteArrayToPNG(const float *apArray,
                             int aWidth,
                             int aHeight,
                             float aPercentile,
                             const char *apFilename) {
    auto rc = BS_BASE_RC_FAILURE;
#ifdef USING_OPENCV
    float *ptr_new = ImageWriter::NormalizeArrayByPercentile(apArray,
                                                             aWidth,
                                                             aHeight,
                                                             aPercentile);
    const cv::Mat data(aHeight, aWidth,
                       CV_32FC1, const_cast<float *>(ptr_new));
    cv::Mat data_displayed(aHeight, aWidth, CV_8UC1);
    float minV = *min_element(data.begin<float>(), data.end<float>());
    float maxV = *max_element(data.begin<float>(), data.end<float>());
    cv::Mat data_scaled = (data - minV) / (maxV - minV);
    data_scaled.convertTo(data_displayed, CV_8UC1, 255.0, 0);
    try {
        cv::imwrite(apFilename, data_displayed);
        rc = BS_BASE_RC_SUCCESS;
    } catch (cv::Exception &ex) {
        rc = BS_BASE_RC_FAILURE;
    }
    delete[] ptr_new;
#endif
    return rc;
}

std::string
ImageWriter::GetExtension() {
    return IO_K_EXT_IMG;
}

int
ImageWriter::Initialize(std::string &aFilePath) {
    this->mFilePath = aFilePath;
    return BS_BASE_RC_SUCCESS;
}

int
ImageWriter::Finalize() {
    return BS_BASE_RC_SUCCESS;
}

int
ImageWriter::Write(std::vector<dataunits::Gather *> aGathers) {
    int index = 0;
    auto rc = BS_BASE_RC_SUCCESS;
    for (auto gather : aGathers) {
        rc = this->WriteGather(gather, "_gather_id_" + to_string(index) + this->GetExtension());
        if (rc != BS_BASE_RC_SUCCESS) {
            break;
        }
        index++;
    }
    return rc;
}

int
ImageWriter::Write(io::dataunits::Gather *aGather) {
    return this->WriteGather(aGather, "");
}

int
ImageWriter::WriteGather(io::dataunits::Gather *apGather,
                         const std::string &aPostfix) {
    auto width = apGather->GetNumberTraces();
    auto height = apGather->GetTrace(0)->GetNumberOfSamples();
    auto array = new float[width * height];
    for (int ix = 0; ix < width; ix++) {
        for (int iz = 0; iz < height; iz++) {
            array[iz * width + ix] = apGather->GetTrace(ix)->GetTraceData()[iz];
        }
    }
    auto rc = ImageWriter::WriteArrayToPNG(array, width, height, this->mPercentile,
                                           (this->mFilePath + aPostfix).c_str());
    delete[] array;
    return rc;
}
