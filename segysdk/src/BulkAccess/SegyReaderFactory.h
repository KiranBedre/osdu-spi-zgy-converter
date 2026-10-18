/*
Copyright 2021-2022 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../BulkAccess/SegyBulkReader.h"
#include "../BulkAccess/Segy2DReader.h"
#include "../FileIO/CacheSizeOption.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/SegyIndex.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk::bulkaccess
{
using segyio::ConversionDetails;
using segyio::FileHeaderDescription;

class SEGYSDK_DLL_PUBLIC SegyReaderFactory
{
public:
    SegyReaderFactory() = delete;

    // Creates a simple gleaner-based reader
    static std::unique_ptr<SegyBulkReader> createSimpleReader(const fileio::FileReference& fileReference);

    // Creates a simple gleaner-based multi reader
    static std::unique_ptr<SegyBulkReader> createSimpleMultiBulkReader(fileio::FileReference const& fileReference,
                                                                       size_t numberOfTraces,
                                                                       size_t bulksCount);

    // Uses index and parameter files adjacent to the segy file
    static std::unique_ptr<SegyBulkReader> createReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                                        const fileio::FileReference& fileReference);

    // Uses index and parameter files adjacent to the segy file
    static std::unique_ptr<Segy2DReader> create2DReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                                        const fileio::FileReference& fileReference);

    // Uses index and parameter files adjacent to the segy file
    static std::unique_ptr<SegyBulkReader> createMultiBulkReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                                                 fileio::FileReference const& fileReference,
                                                                 size_t numberOfTraces,
                                                                 size_t bulksCount);

    // Uses index and parameter files adjacent to the segy file
    static std::unique_ptr<SegyBulkReader> createSectionBulkReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                                                   fileio::FileReference const& fileReference,
                                                                   size_t readersCount);

    // The (current) generic interface
    static std::unique_ptr<SegyBulkReader> createReader(const std::shared_ptr<fileio::FileReader>& reader,
                                                        const std::shared_ptr<fileio::FileReader>& indexReader,
                                                        const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                        const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                                        const ConversionDetails& details,
                                                        const CacheSizeOption cacheSizeOption);

    // The (current) generic multi reader interface
    static std::unique_ptr<SegyBulkReader> createMultiBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                                                 size_t numberOfTraces,
                                                                 const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                                 const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                                                 const ConversionDetails& details,
                                                                 const CacheSizeOption cacheSizeOption);

    // The (current) generic section reader interface
    static std::unique_ptr<SegyBulkReader> createSectionBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                                                   const std::shared_ptr<fileio::FileReader>& indexReader,
                                                                   const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                                   const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                                                   const ConversionDetails& details,
                                                                   const CacheSizeOption cacheSizeOption);

    // The (current) generic interface
    static std::unique_ptr<Segy2DReader> create2DReader(const std::shared_ptr<fileio::FileReader>& reader,
                                                        const std::shared_ptr<fileio::FileReader>& indexReader,
                                                        const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                        const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                                        const ConversionDetails& details,
                                                        const CacheSizeOption cacheSizeOption);
};

} // namespace segysdk::bulkaccess
