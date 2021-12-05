/**
 * Copyright (C) 2021 by Brightskies inc
 *
 * This file is part of BS I/O.
 *
 * BS I/O is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BS I/O is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEDLIB. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BS_IO_INDEXERS_FILE_INDEXER_HPP
#define BS_IO_INDEXERS_FILE_INDEXER_HPP

#include <string>

#include <bs/io/streams/helpers/InStreamHelper.hpp>
#include <bs/io/streams/helpers/OutStreamHelper.hpp>
#include <bs/io/indexers/IndexMap.hpp>

namespace bs {
    namespace io {
        namespace indexers {

            class FileIndexer {
            public:
                /**
                 * @brief Explicit constructor.
                 * @param aFilePath
                 */
                explicit FileIndexer(std::string &aFilePath, std::string &aKey);

                /**
                 * @brief Destructor.
                 */
                ~FileIndexer();

                /**
                 * @brief
                 * Initializes the indexer with the appropriate settings to be applied.
                 *
                 * @return File initial size (i.e. Zero value).
                 */
                size_t
                Initialize();

                /**
                 * @brief
                 * Release all resources and close everything.
                 * Should be initialized again afterwards to be able to reuse it again.
                 *
                 * @return Status flag.
                 */
                int
                Finalize();

                /**
                 * @brief Do the actual indexing upon the given file. Takes trace header keys vector to index upon it
                 * @param[in] aTraceHeaderKeys
                 * @return Index Map.
                 */
                indexers::IndexMap
                Index(const std::vector<dataunits::TraceHeaderKey> &aTraceHeaderKeys);

                /**
                 * @brief Index Map getter.
                 * @return IndexMap object.
                 */
                inline indexers::IndexMap
                GetIndexMap() { return this->mIndexMap; }

                /**
                 * @brief Indexed file path getter.
                 */
                inline std::string
                GetIndexedFilePath() { return this->mOutFilePath; }

            private:
                static std::string
                GenerateOutFilePathName(std::string &aInFileName, std::string &aKey);

                void
                StoreMapToFile(const std::vector<dataunits::TraceHeaderKey> &aTraceHeaderKeys);

                void
                LoadMapFromFile(streams::helpers::InStreamHelper *aInStreamHelper,
                                const std::vector<dataunits::TraceHeaderKey> &aTraceHeaderKeys);

            private:
                /// Input ile path.
                std::string mInFilePath;
                /// Output file path.
                std::string mOutFilePath;
                /// File input stream helper.
                streams::helpers::InStreamHelper *mInStreamHelper;
                /// Index map, having trace header key as key and vector of byte positions
                /// in file corresponding to this trace header key.
                indexers::IndexMap mIndexMap;
            };

        } //namespace indexers
    } //namespace io
} //namespace bs

#endif //BS_IO_INDEXERS_FILE_INDEXER_HPP
