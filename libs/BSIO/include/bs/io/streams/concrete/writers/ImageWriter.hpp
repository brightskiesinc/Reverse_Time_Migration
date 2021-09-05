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

#ifndef BS_IO_STREAMS_IMAGE_WRITER_HPP
#define BS_IO_STREAMS_IMAGE_WRITER_HPP

#include <bs/io/streams/primitive/Writer.hpp>

namespace bs {
    namespace io {
        namespace streams {
            /**
             * @brief
             */
            class ImageWriter : public Writer {
            public:
                /**
                 * @brief Constructor
                 */
                explicit ImageWriter(bs::base::configurations::ConfigurationMap *apConfigurationMap);

                /**
                 * @brief Destructor
                 */
                ~ImageWriter() override = default;

                /**
                 * @brief
                 * Acquires the component configurations from a given configurations map.
                 *
                 * @param[in] apConfigurationMap
                 * The configurations map to be used.
                 */
                void
                AcquireConfiguration() override;

                /**
                 * @brief Returns the file format extension of the current stream.
                 */
                std::string
                GetExtension() override;

                /**
                 * @brief
                 * Initializes the writer with the appropriate settings applied
                 * to the writer, and any preparations needed are done.
                 * Should be called once at the start.
                 *
                 * @param[in] aFilePath
                 * The path to be used, either directly or as a seed, for writing.
                 */
                int
                Initialize(std::string &aFilePath) override;

                /**
                 * @brief
                 * Does any final updates needed for consistency of the writer.
                 * Release all resources and close everything.
                 * Should be initialized again afterwards to be able to reuse it again.
                 */
                int
                Finalize() override;

                /**
                 * @brief
                 * Writes a group of gathers to the output stream of the writer.
                 *
                 * @param[in] aGathers
                 * List of gathers to be written.
                 *
                 * @return
                 * An error flag, if 0 that means operation was successful, otherwise indicate an error.
                 */
                int
                Write(std::vector<dataunits::Gather *> aGathers) override;

                /**
                 * @brief
                 * Writes a gather to the output stream of the writer.
                 *
                 * @param[in] aGather
                 * The gather to be written.
                 *
                 * @return
                 * An error flag, if 0 that means operation was successful, otherwise indicate an error.
                 */
                int
                Write(io::dataunits::Gather *aGather) override;

            private:
                /**
                 * @brief
                 * Normalizes an array using a given percentile.
                 *
                 * @param[in] apInputArray
                 * The array to normalize.
                 *
                 * @param[in] aWidth
                 * The width of the array(number of elements).
                 *
                 * @param[in] aHeight
                 * The height of the array(number of elements).
                 *
                 * @param[in] aPercentile
                 * The percentile to normalize on.
                 *
                 * @return
                 * The normalized array.
                 */
                static float *NormalizeArrayByPercentile(const float *apInputArray,
                                                         int aWidth,
                                                         int aHeight,
                                                         float aPercentile);

                /**
                 * @brief
                 * Writes a 2D array as a png image.
                 *
                 * @param[in] apArray
                 * The array to process.
                 *
                 * @param[in] aWidth
                 * The width of the array(minor axis).
                 *
                 * @param[in] aHeight
                 * The height of the array(major axis).
                 *
                 * @param[in] aPercentile
                 * The percentile to normalize on.
                 *
                 * @param[in] apFilename
                 * The filename to write to.
                 *
                 * @return
                 * Status flag.
                 */
                static int WriteArrayToPNG(const float *apArray,
                                           int aWidth,
                                           int aHeight,
                                           float aPercentile,
                                           const char *apFilename);

                /**
                 * @brief
                 * Writes a gather to the writer filepath with the added postfix.
                 *
                 * @param[in] apGather
                 * The gather to write.
                 *
                 * @param[in] aPostfix
                 * The postfix tag to add to the name.
                 *
                 * @return
                 * Status flag.
                 */
                int WriteGather(io::dataunits::Gather *apGather,
                                const std::string &aPostfix);

            private:
                /// File path.
                std::string mFilePath;
                /// The percentile to use.
                float mPercentile;
            };

        } //streams
    } //thoth
} //namespace bs

#endif //BS_IO_STREAMS_IMAGE_WRITER_HPP
