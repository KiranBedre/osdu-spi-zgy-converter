/*
Copyright 2021 Schlumberger

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

#include "../FileIO/CachedFileReader.h"
#include "../Logger/Logger.h"

namespace segysdk::fileio
{

CachedFileReader::CachedFileReader(const std::shared_ptr<FileReader>& fileReader, const std::shared_ptr<ReadCache>& cache, uint16_t segyFileNumber)
  : CachedFileReader(
        fileReader->getFileName(), [fileReader]() { return fileReader; }, cache, segyFileNumber)
{
    if (cache->isMultiThread())
        throw std::invalid_argument("CachedFileReader: Use factory method overload for multi-thread reading.");
}

CachedFileReader::~CachedFileReader()
{
    fileReaders_.clear();
    cache_ = nullptr;
}

template<class CreateReader>
static auto
createFileReaders(const size_t threadCount, const CreateReader& fileReaderFactory)
{
    std::vector<std::shared_ptr<FileReader>> ret;
    for (size_t i = 0; i < threadCount; ++i) {
        ret.push_back(fileReaderFactory());
    }
    return ret;
}

CachedFileReader::CachedFileReader(const std::string& fileName, const std::function<std::shared_ptr<FileReader>()>& fileReaderFactory, const std::shared_ptr<ReadCache>& cache, uint16_t segyFileNumber)
  : FileReader(fileName),
    fileReaders_(createFileReaders(cache->threadCount(), fileReaderFactory)),
    cache_(cache),
    segyFileNumber_(segyFileNumber)
{
    SEGYLOG_DEBUG("Creating cached file reader for " + fileName);
}

size_t
CachedFileReader::getBufferSize() const
{
    return cache_->pageSize();
}

uint64_t
CachedFileReader::getFileSize()
{
    return fileReaders_[0]->getFileSize();
}

ByteBuffer
CachedFileReader::doRead(const FileReadParams& params)
{
    return cache_->read(ReadCacheParams{params}.withSegyFileNumber(segyFileNumber_), fileReaders_);
}

void
CachedFileReader::close()
{
    for (const auto& fileReader : fileReaders_) {
        fileReader->close();
    }
}

} // namespace segysdk::fileio
