//
// Created by zeyad-osama on 02/11/2020.
//

#ifndef THOTH_STREAMS_SEGY_READER_HPP
#define THOTH_STREAMS_SEGY_READER_HPP

#include <thoth/streams/primitive/Reader.hpp>
#include <thoth/streams/helpers/InStreamHelper.hpp>
#include <thoth/indexers/FileIndexer.hpp>

namespace thoth {
    namespace streams {
        /**
         * @brief SEG-Y file format reader.
         */
        class SegyReader : public Reader {
        public:
            /**
             * @brief Constructor
             */
            explicit SegyReader(thoth::configuration::ConfigurationMap *apConfigurationMap);

            /**
             * @brief Destructor.
             */
            ~SegyReader() override;

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
             * Initializes the reader with the appropriate settings applied
             * to the reader, and any preparations needed are done.
             * Should be called once at the start.
             *
             * @param[in] aGatherKeys
             * A vector containing the strings of the headers to utilize as the gathers in reading.
             * If provided an empty vector, will not gather on any key and will return all data.
             *
             * @param[in] aSortingKeys
             * A vector of pairs with each pair containing a string representing the header to sort on, and the sorting direction.
             * The vector is treated with the first element being the most major decider, and the last the most minor one.
             * If empty, no sorting is applied.
             *
             * @param[in] aPaths
             * List of paths to be processed by the the reader. 
             */
            int
            Initialize(std::vector<std::string> &aGatherKeys,
                       std::vector<std::pair<std::string, dataunits::Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) override;

            /**
             * @brief
             * Initializes the reader with the appropriate settings applied
             * to the reader, and any preparations needed are done.
             * Should be called once at the start.
             *
             * @param[in] aGatherKeys
             * A vector containing the trace header keys of the headers to utilize as the gathers in reading.
             * If provided an empty vector, will not gather on any key and will return all data.
             *
             * @param[in] aSortingKeys
             * A vector of pairs with each pair containing a string representing the header to sort on, and the sorting direction.
             * The vector is treated with the first element being the most major decider, and the last the most minor one.
             * If empty, no sorting is applied.
             *
             * @param[in] aPaths
             * List of paths to be processed by the the reader. 
             */
            int
            Initialize(std::vector<dataunits::TraceHeaderKey> &aGatherKeys,
                       std::vector<std::pair<dataunits::TraceHeaderKey, dataunits::Gather::SortDirection>> &aSortingKeys,
                       std::vector<std::string> &aPaths) override;

            /**
             * @brief
             * Release all resources and close everything.
             * Should be initialized again afterwards to be able to reuse it again.
             */
            int
            Finalize() override;

            /**
             * @brief
             * Sets the reader to only read and set the headers in the gather read functionalities.
             * This will reduce the time needed, however won't populate the data in the traces at all.
             * You should be able to enable and disable as you want during the runs.
             * By default, this mode is considered false unless explicitly called and set.
             * 
             * @param[in] aEnableHeaderOnly
             * If true, make all gather read functions read and set the headers only.
             * If false, will make all gather read functions read headers and data.
             */
            void
            SetHeaderOnlyMode(bool aEnableHeaderOnly) override;

            /**
             * @brief
             * Return the total number of gathers available for reading.
             * 
             * @return
             * An unsigned int representing how many unique gathers are available for the reader.
             */
            unsigned int
            GetNumberOfGathers() override;

            /**
             * @brief
             * Get the unique values of gathers available to be read.
             * 
             * @return
             * A list of sub-lists.
             * Each sublist represent one gather unique value combination.
             * The sublist length is equal to the number of keys that the reader is set to gather on.
             * The list length is equal to the value returned by GetNumberOfGathers().
             * The order of values in the sublist matches the same order given in the initialize.
             */
            std::vector<std::vector<std::string>>
            GetIdentifiers() override;

            /**
             * @brief
             * Get all the gathers that can be read by the reader.
             *
             * @return
             * A vector of all possible gathers.
             * The gather pointers returned should be deleted by the user to avoid memory leaks.
             */
            std::vector<dataunits::Gather *>
            ReadAll() override;

            /**
             * @brief
             * Get a group of gathers with the requested unique values.
             *
             * @param[in] aHeaderValues
             * Each sublist represent one gather unique value combination.
             * The sublist length is equal to the number of keys that the reader is set to gather on.
             * The order of values in the sublist matches the same order given in the initialize.
             * 
             * @return
             * A vector of the gathers matching the given request.
             * The gather pointers returned should be deleted by the user to avoid memory leaks.
             */
            std::vector<dataunits::Gather *>
            Read(std::vector<std::vector<std::string>> aHeaderValues) override;

            /**
             * @brief
             * Get a group of gathers with the requested unique values.
             *
             * @param[in] aHeaderValues
             * Each list represent one gather unique value combination.
             * The list length is equal to the number of keys that the reader is set to gather on.
             * The order of values in the list matches the same order given in the initialize.
             * 
             * @return
             * The gather matching the given request.
             * The gather pointer returned should be deleted by the user to avoid memory leaks.
             */
            thoth::dataunits::Gather *
            Read(std::vector<std::string> aHeaderValues) override;

            /**
             * @brief
             * Get a gather by its index.
             * 
             * @param[in] aIndex
             * The index of the gather to be read. Must be between 0 and 
             * the number returned by GetNumberOfGathers().
             * It should return the gather with the unique value equivalent to
             * the entry of GetUniqueGatherValues() at index aIndex.
             *
             * @return
             * The requested gather if possible, nullptr otherwise.
             * The gather pointer returned should be deleted by the user to avoid memory leaks.
             */
            thoth::dataunits::Gather *
            Read(unsigned int aIndex) override;

            /**
             * @brief Check if the given SEG-Y file has extended text header or not.
             *
             * @note This function should not be used before any of the read functions,
             * since it gets set internally when parsing the given file.
             */
            bool
            HasExtendedTextHeader() const;

            /**
             * @brief Gets the text header extracted from the given SEG-Y file.
             *
             * @note This function should not be used before any of the read functions,
             * since it gets set internally when parsing the given file.
             */
            unsigned char *
            GetTextHeader();

            /**
             * @brief Gets the extended text header extracted (i.e. If any) from the given SEG-Y file.
             *
             * @note This function should not be used before any of the read functions,
             * since it gets set internally when parsing the given file.
             */
            unsigned char *
            GetExtendedTextHeader();

        private:
            int
            Index();

        private:
            /// File paths.
            std::vector<std::string> mPaths;
            /// File stream helper.
            std::vector<streams::helpers::InStreamHelper *> mInStreamHelpers;
            /// Text header.
            /// Should be initialized in Initialize function and freed in Finalize function.
            unsigned char *mTextHeader;
            /// Extended Text header.
            /// Should be initialized in Initialize function and freed in Finalize function.
            unsigned char *mExtendedHeader;
            /// Binary Header Lookup.
            lookups::BinaryHeaderLookup mBinaryHeaderLookup{};
            /// Files indexers vector.
            std::vector<indexers::FileIndexer> mFileIndexers;
            /// Index maps vector.
            std::vector<indexers::IndexMap> mIndexMaps;
            /// Extended Text header check variable.
            /// Should be set in any of the Read functions.
            bool mHasExtendedTextHeader;
            /// Boolean to check whether text headers should be stored or not.
            bool mStoreTextHeaders;
            /// mGatherKeys: vector of keys to gather on
            std::vector<dataunits::TraceHeaderKey> mGatherKeys;
            /// mSortingKeys: Keys to sort gathers on, and sorting direction of each key
            /// Optional to have file(s) gathers sorted
            std::vector<std::pair<dataunits::TraceHeaderKey, dataunits::Gather::SortDirection>> mSortingKeys;
            /// Enable header only mode boolean.
            bool mEnableHeaderOnly;
        };
    } //streams
} //thoth

#endif //THOTH_STREAMS_SEGY_READER_HPP
