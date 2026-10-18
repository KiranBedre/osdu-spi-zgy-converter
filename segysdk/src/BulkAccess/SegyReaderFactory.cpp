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

#include "../BulkAccess/SegyReaderFactory.h"

#include "../BulkAccess/BulkReader.h"
#include "../BulkAccess/IndexedBulkIndex.h"
#include "../BulkAccess/MultiBulkReader.h"
#include "../BulkAccess/SectionBulkReader.h"
#include "../BulkAccess/Segy2DReaderImpl.h"
#include "../BulkAccess/SimpleBulkIndex.h"
#include "../FileIO/FileManager.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../SegyIO/BinaryHeader.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/SegyGleaner.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIndex/SegyIndexFactory.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

namespace segysdk::bulkaccess
{
using segyio::BinaryHeader;
using segyio::SegyGleaner;

//****************************************************************************************************
// class SegyReaderFactory

void
check3DReaderCreation(const std::shared_ptr<segyindex::SegyIndex>& index)
{
    // Index should be loaded
    if (not index->isSegy3D()) {
        throw segyio::SegyRuntimeException("3D Reader can't be created for 2D Index");
    }
}

void
check2DReaderCreation(const std::shared_ptr<segyindex::SegyIndex>& index)
{
    // Index should be loaded
    if (index->isSegy3D()) {
        throw segyio::SegyRuntimeException("2D Reader can't be created for 3D Index");
    }
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createSimpleReader(const fileio::FileReference& fileReference)
{
    std::string reason;
    SegyGleaner gleaner(fileReference);
    if (!gleaner.glean(reason))
        throw segyio::SegyRuntimeException(std::string("Geometry gleaning failed for: " + fileReference.fileName() + ", - " + reason));

    const auto results = gleaner.getResults();
    const auto index = std::make_shared<SimpleBulkIndex>(results.getInlineExtent().getCount(), results.getCrosslineExtent().getCount(), results.getSampleExtent().getCount(), results.getSampleFormat());
    const auto params = fileio::FileManagerParams{fileReference};
    const auto file = fileio::GenericFileManager::createReader(params);
    const auto indexFile = fileio::GenericFileManager::createReader(params);

    auto result = std::make_unique<BulkReader>(file, indexFile, index);
    dynamic_cast<SegyBulkReader*>(result.get())->setBufferLoadChunk(fileReference.readerOptions().cacheSize.chunkSize());
    return result;
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createSimpleMultiBulkReader(const fileio::FileReference& fileReference, size_t numberOfTraces, size_t bulksCount)
{
    std::string reason;
    SegyGleaner gleaner(fileReference);
    if (!gleaner.glean(reason)) {
        throw segyio::SegyRuntimeException(std::string("Geometry gleaning failed for: " + fileReference.fileName() + ", - " + reason));
    }
    const auto results = gleaner.getResults();
    const auto bulkIndex = std::make_shared<SimpleBulkIndex>(results.getInlineExtent().getCount(), results.getCrosslineExtent().getCount(), results.getSampleExtent().getCount(), results.getSampleFormat());
    const auto params = fileio::FileManagerParams{fileReference};
    std::vector<decltype(fileio::GenericFileManager::createReader(params))> readers(bulksCount, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&params]() -> auto {
            return fileio::GenericFileManager::createReader(params);
        });

    auto result = std::make_unique<MultiBulkReader>(std::move(readers), numberOfTraces, bulkIndex);
    dynamic_cast<SegyBulkReader*>(result.get())->setBufferLoadChunk(fileReference.readerOptions().cacheSize.chunkSize());
    return result;
}

template<typename indexCheckFunction, typename createFunction>
std::result_of_t<createFunction(const std::shared_ptr<fileio::FileReader>& reader,
                                const std::shared_ptr<fileio::FileReader>& indexReader,
                                const std::shared_ptr<segyindex::SegyIndex>& index,
                                const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                const ConversionDetails& details,
                                const CacheSizeOption cacheSizeOption)>
createReaderF(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
              fileio::FileReference const& fileReference,
              indexCheckFunction check,
              createFunction create)
{
    const std::string& fileName{fileReference.fileName()};
    fileio::FileReaderOptions const fileOptions{fileReference.readerOptions()};

    const auto filesNames = std::vector{fileName};
    const auto index = segysdk::api::SegyIndexFactory().CreateSegyIndex(segyFilesVector, filesNames, fileOptions);

    if (!index->hasIndexFile()) {
        throw segyio::SegyRuntimeException("Unable to locate SEG-Y index.");
    }

    index->Load();
    check(index);

    // Note: This loading is using GenericFileManager
    const auto headerDescription = FileHeaderDescription::loadFromParameterFile(fileName, fileOptions);
    if (headerDescription == nullptr)
        throw segyio::SegyRuntimeException("Unable to load parameter file.");
    if (headerDescription->getTraceHeaders()->empty())
        throw segyio::SegyRuntimeException("Unable to locate header mapping.");

    const auto params = fileio::FileManagerParams{fileReference};
    const auto reader = fileio::GenericFileManager::createReader(params);
    const auto indexReader = fileio::GenericFileManager::createReader(params);
    const auto binaryHeader = BinaryHeader(reader, *headerDescription->getBinaryHeaders(), *headerDescription->getBinaryHeaderOverrides());
    const auto details = ConversionDetails(binaryHeader.isByteSwapped(), binaryHeader.getSampleFormat());

    return create(reader, indexReader, index, headerDescription, details, fileReference.readerOptions().cacheSize);
}

std::unique_ptr<SegyBulkReader>
createBulkReader(const std::shared_ptr<fileio::FileReader>& reader,
                 const std::shared_ptr<fileio::FileReader>& indexReader,
                 const std::shared_ptr<segyindex::SegyIndex>& index,
                 const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                 const ConversionDetails& details,
                 const CacheSizeOption cacheSizeOption)
{
    return SegyReaderFactory::createReader(reader, indexReader, index, headerDescription, details, cacheSizeOption);
}

std::unique_ptr<Segy2DReader>
createSegy2DReader(const std::shared_ptr<fileio::FileReader>& reader,
                   const std::shared_ptr<fileio::FileReader>& indexReader,
                   const std::shared_ptr<segyindex::SegyIndex>& index,
                   const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                   const ConversionDetails& details,
                   const CacheSizeOption cacheSizeOption)
{
    return SegyReaderFactory::create2DReader(reader, indexReader, index, headerDescription, details, cacheSizeOption);
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                fileio::FileReference const& fileReference)
{
    return createReaderF(segyFilesVector, fileReference, check3DReaderCreation, createBulkReader);
}

std::unique_ptr<Segy2DReader>
SegyReaderFactory::create2DReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector, const fileio::FileReference& fileReference)
{
    return createReaderF(segyFilesVector, fileReference, check2DReaderCreation, createSegy2DReader);
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createMultiBulkReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                         fileio::FileReference const& fileReference,
                                         size_t numberOfTraces,
                                         size_t bulksCount)
{
    const std::string& fileName{fileReference.fileName()};
    fileio::FileReaderOptions const fileOptions{fileReference.readerOptions()};

    const auto filesNames = std::vector{fileName};
    const auto index = segysdk::api::SegyIndexFactory().CreateSegyIndex(segyFilesVector, filesNames, fileOptions);

    if (!index->hasIndexFile())
        throw segyio::SegyRuntimeException("Unable to locate SEG-Y index.");

    index->Load();
    check3DReaderCreation(index);

    // Note: This loading is using GenericFileManager
    const auto headerDescription = FileHeaderDescription::loadFromParameterFile(fileReference);
    if (headerDescription == nullptr)
        throw segyio::SegyRuntimeException("Unable to load parameter file.");
    if (headerDescription->getTraceHeaders()->empty())
        throw segyio::SegyRuntimeException("Unable to locate header mapping.");

    const auto params = fileio::FileManagerParams{fileReference};
    const auto reader = fileio::GenericFileManager::createReader(params);

    const auto binaryHeader = BinaryHeader(reader, *headerDescription->getBinaryHeaders(), *headerDescription->getBinaryHeaderOverrides());
    const auto details = ConversionDetails(binaryHeader.isByteSwapped(), binaryHeader.getSampleFormat());

    std::vector<decltype(fileio::GenericFileManager::createReader(params))> readers(bulksCount, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&params]() -> auto {
            return fileio::GenericFileManager::createReader(params);
        });

    return createMultiBulkReader(std::move(readers), numberOfTraces, index, headerDescription, details, fileReference.readerOptions().cacheSize);
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createSectionBulkReader(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                           fileio::FileReference const& fileReference,
                                           size_t readersCount)
{
    const std::string& fileName{fileReference.fileName()};
    fileio::FileReaderOptions const fileOptions{fileReference.readerOptions()};

    const auto filesNames = std::vector{fileName};
    const auto index = segysdk::api::SegyIndexFactory().CreateSegyIndex(segyFilesVector, filesNames, fileOptions);

    if (!index->hasIndexFile())
        throw segyio::SegyRuntimeException("Unable to locate SEG-Y index.");

    index->Load();
    check3DReaderCreation(index);

    // Note: This loading is using GenericFileManager
    const auto headerDescription = FileHeaderDescription::loadFromParameterFile(fileReference);
    if (headerDescription == nullptr)
        throw segyio::SegyRuntimeException("Unable to load parameter file.");
    if (headerDescription->getTraceHeaders()->empty())
        throw segyio::SegyRuntimeException("Unable to locate header mapping.");

    const auto params = fileio::FileManagerParams{fileReference};
    const auto reader = fileio::GenericFileManager::createReader(params);
    const auto indexReader = fileio::GenericFileManager::createReader(params);

    const auto binaryHeader = BinaryHeader(reader, *headerDescription->getBinaryHeaders(), *headerDescription->getBinaryHeaderOverrides());
    const auto details = ConversionDetails(binaryHeader.isByteSwapped(), binaryHeader.getSampleFormat());

    std::vector<decltype(fileio::GenericFileManager::createReader(params))> readers(readersCount, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&params]() -> auto {
            return fileio::GenericFileManager::createReader(params);
        });

    return createSectionBulkReader(std::move(readers), indexReader, index, headerDescription, details, fileReference.readerOptions().cacheSize);
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createReader(const std::shared_ptr<fileio::FileReader>& reader,
                                const std::shared_ptr<fileio::FileReader>& indexReader,
                                const std::shared_ptr<segyindex::SegyIndex>& index,
                                const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                const ConversionDetails& details,
                                const CacheSizeOption cacheSizeOption)
{
    // At this point index is loaded
    check3DReaderCreation(index);
    const auto bulkIndex = std::make_shared<IndexedBulkIndex>(index, headerDescription, details);
    auto result = std::make_unique<BulkReader>(reader, indexReader, bulkIndex);
    dynamic_cast<SegyBulkReader*>(result.get())->setBufferLoadChunk(cacheSizeOption.chunkSize());
    return result;
}

std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createMultiBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                         size_t numberOfTraces,
                                         const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                         const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                         const ConversionDetails& details,
                                         const CacheSizeOption cacheSizeOption)
{
    // At this point index is loaded
    check3DReaderCreation(index);
    const auto bulkIndex = std::make_shared<IndexedBulkIndex>(index, headerDescription, details);
    auto result = std::make_unique<MultiBulkReader>(std::move(readers), numberOfTraces, bulkIndex);
    dynamic_cast<SegyBulkReader*>(result.get())->setBufferLoadChunk(cacheSizeOption.chunkSize());
    return result;
}

// The (current) generic section reader interface
std::unique_ptr<SegyBulkReader>
SegyReaderFactory::createSectionBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                           const std::shared_ptr<fileio::FileReader>& indexReader,
                                           const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                           const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                           const ConversionDetails& details,
                                           const CacheSizeOption cacheSizeOption)
{
    // At this point index is loaded
    check3DReaderCreation(index);
    const auto bulkIndex = std::make_shared<IndexedBulkIndex>(index, headerDescription, details);
    auto result = std::make_unique<SectionBulkReader>(std::move(readers), indexReader, bulkIndex);
    dynamic_cast<SegyBulkReader*>(result.get())->setBufferLoadChunk(cacheSizeOption.chunkSize());
    return result;
}

std::unique_ptr<Segy2DReader>
SegyReaderFactory::create2DReader(const std::shared_ptr<fileio::FileReader>& reader,
                                  const std::shared_ptr<fileio::FileReader>& indexReader,
                                  const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                  const std::shared_ptr<const FileHeaderDescription>& headerDescription,
                                  const ConversionDetails& details, const CacheSizeOption cacheSizeOption)
{
    check2DReaderCreation(index);
    const auto bulkIndex = std::make_shared<IndexedBulkIndex>(index, headerDescription, details);
    auto result = std::make_unique<Segy2DReaderImpl>(reader, indexReader, bulkIndex);
    result->setBufferLoadChunk(cacheSizeOption.chunkSize());
    return result;
}

} // namespace segysdk::bulkaccess
