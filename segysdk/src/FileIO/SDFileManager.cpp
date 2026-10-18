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

#include "../FileIO/SDFileManager.h"

#include "../FileIO/CachedFileReader.h"
#include "../FileIO/FileIOException.h"
#include "../FileIO/SDFileReader.h"
#include "../Logger/Logger.h"
#include "../Utilities/Settings.h"
#pragma warning(push)
#pragma warning(disable : 4251)

#include <SDAPI/SDManager.h>
#include <SDAPI/SDUtils.h>

#pragma warning(pop)

using seismicdrive::SDManager;
using seismicdrive::SDUtils;

namespace segysdk::fileio
{

static std::string
sdAuthCallback(const void* data)
{
    const auto tokenSource = static_cast<segysdk::utilities::auth::TokenSource const*>(data);
    return tokenSource->token();
}

std::shared_ptr<SDManager>
SDFileManager::createSDManager()
{
    auto sd_manager = std::make_shared<SDManager>(sdCredentials_->url(), sdCredentials_->apiKey(), logging_);

    const auto httpHeaders = sdCredentials_->headers();
    if (!httpHeaders.empty()) {
        sd_manager->setHttpHeaders(httpHeaders);
    }
    sd_manager->setAuthProviderCallback(&sdAuthCallback, sdCredentials_->tokenSource().get());

    return sd_manager;
}

SDFileManager::SDFileManager(const SDCredentials& sdCredentials, const CacheSizeOption& cacheSize,
                             const std::optional<std::string> sdLegalTag)
  : sdCredentials_(std::make_shared<SDCredentials>(sdCredentials)),
    cacheSize_(cacheSize),
    sdLegalTag_(std::move(sdLegalTag)),
    logging_(sdCredentials.logging()),
    manager_(createSDManager()),
    readers_(),
    rawReaders_()
{
}

SDFileManager::~SDFileManager()
{
    readers_.clear();
    rawReaders_.clear();
}

bool
SDFileManager::exists(const std::string& fileName)
{
    std::lock_guard lock(mutex_);

    auto utils = SDUtils(manager_.get());
    return utils.datasetExist(fileName);
}

size_t
SDFileManager::getSize(const std::string& fileName)
{
    std::lock_guard lock(mutex_);

    auto utils = SDUtils(manager_.get());
    return utils.datasetSize(fileName);
}

TextFileReadResult
SDFileManager::readTextFile(const FileAccessParams& params)
{
    std::lock_guard lock(mutex_);
    try {
        const auto reader = createReader(params);
        const size_t filesize = reader->getFileSize();
        if (filesize == 0)
            return {true, {}};

        return {true, stringFromBuffer(reader->read(FileReadParams{request::Params{params}}.withOffset(0).withSize(filesize)))};
    }
    catch (const std::exception& ex) {
        throw FileIOException(std::string("SDFileManager::readTextFile: Error reading from file: ") + ex.what());
    }
}

bool
SDFileManager::writeTextFile(const std::string& fileName, const std::string& text, bool allowOverwrite)
{
    std::lock_guard lock(mutex_);

    if (text.empty())
        throw FileIOException(std::string("SDFileManager::writeTextFile: Attempt to write empty string: ") + fileName);

    // Can be removed when the writer handles WriteMode
    if (!allowOverwrite && exists(fileName))
        throw FileIOException("SDFileManager::writeTextFile: Create failed, file already exists - " + fileName);

    try {
        const auto writer = createSDWriter(fileName, allowOverwrite ? WriteMode::Truncate : WriteMode::Create, text.size());
        const auto success = writer->write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
        writer->close();

        return success;
    }
    catch (const std::exception& ex) {
        throw FileIOException(std::string("SDFileManager::writeTextFile: Error writing to file: ") + ex.what());
    }
}

std::shared_ptr<FileReader>
SDFileManager::createReader(const FileAccessParams& params)
{
    SEGYLOG_DEBUG("Creating reader for " + params.fileName_);
    std::lock_guard lock(mutex_);
    const auto key = params.fileName_ + sdCredentials_->token();
    if (readers_.find(key) == readers_.end()) {
        readers_.try_emplace(key,
                             std::make_shared<CachedFileReader>(
                                 params.fileName_,
                                 [this, &params] {
                                     return std::make_shared<SDFileReader>(SDFileReaderParams{params}.withConnection({manager_, sdCredentials_}));
                                 },
                                 std::make_shared<ReadCache>(cacheSize_)));
    }

    return readers_[key];
}

std::shared_ptr<SDFileWriter>
SDFileManager::createSDWriter(const std::string& fileName, WriteMode mode, size_t blockSize)
{
    SEGYLOG_DEBUG("Creating writer for " + fileName);
    std::lock_guard lock(mutex_);

    // If the file was opened ReadOnly previously - remove/close to reopen properly for write.
    if (readers_.find(fileName) != readers_.end()) {
        readers_.erase(fileName);
    }
    if (rawReaders_.find(fileName) != rawReaders_.end()) {
        rawReaders_.erase(fileName);
    }

    return std::make_shared<SDFileWriter>(manager_, fileName, mode, blockSize, sdLegalTag_);
}

std::shared_ptr<FileWriter>
SDFileManager::createWriter(const std::string& fileName, WriteMode mode)
{
    std::lock_guard lock(mutex_);

    return createSDWriter(fileName, mode, defaultBlockSize_);
}

std::shared_ptr<FileReader>
SDFileManager::createRawReader(const FileAccessParams& params)
{
    SEGYLOG_DEBUG("Creating raw reader for " + params.fileName_);
    std::lock_guard lock(mutex_);
    if (rawReaders_.find(params.fileName_) == rawReaders_.end()) {
        rawReaders_.try_emplace(params.fileName_,
                                std::make_shared<SDFileReader>(SDFileReaderParams{params}.withConnection({manager_, sdCredentials_})));
    }

    return rawReaders_[params.fileName_];
}

bool
SDFileManager::remove(const std::string& fileName)
{
    std::lock_guard lock(mutex_);

    auto utils = SDUtils(manager_.get());
    if (utils.datasetExist(fileName)) {
        utils.deleteDataset(fileName);
        return true;
    }

    return false;
}

std::shared_ptr<seismicdrive::SDManager>
SDFileManager::getSDManager()
{
    return manager_;
}

} // namespace segysdk::fileio
