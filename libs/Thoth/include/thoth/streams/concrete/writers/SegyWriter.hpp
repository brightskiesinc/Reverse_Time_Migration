//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_SEGY_WRITER_HPP
#define THOTH_STREAMS_SEGY_WRITER_HPP

#include <thoth/streams/primitive/Writer.hpp>
#include <thoth/streams/helpers/OutStreamHelper.hpp>

namespace thoth {
    namespace streams {
        /**
         * @brief SEG-Y file format writer.
         */
        class SegyWriter : public Writer {
        public:
            /**
             * @brief Constructor
             */
            explicit SegyWriter(thoth::configuration::ConfigurationMap *apConfigurationMap);

            /**
             * @brief Destructor
             */
            ~SegyWriter() override;

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
            std::string GetExtension() override;

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
            Write(thoth::dataunits::Gather *aGather) override;

        private:
            /// File path.
            std::string mFilePath;
            /// File stream helper.
            streams::helpers::OutStreamHelper *mOutStreamHelpers;
            /// Boolean to check whether the target file should be written
            /// in little endian or not.
            bool mWriteLittleEndian;
        };

    } //streams
} //thoth

#endif //THOTH_STREAMS_SEGY_WRITER_HPP
