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

#include "../FileIO/GenericFileManager.h"

#include "../FileIO/DiskFileManager.h"
#include "../FileIO/SDFileManager.h"
#include "../FileIO/SDFileReader.h"

#include <SDAPI/SDManager.h>

namespace segysdk::fileio
{

std::map<std::string, std::shared_ptr<FileManager>> GenericFileManager::Managers;
std::recursive_mutex GenericFileManager::Mutex;

std::shared_ptr<FileManager>
GenericFileManager::getManager(const FileReference& fileReference)
{
    return getManager(FileManagerParams{fileReference});
}

std::shared_ptr<FileManager>
GenericFileManager::getManager(const FileManagerParams& params)
{
    auto lock = std::unique_lock{GenericFileManager::Mutex};
    const auto& options = params.readerOptions_;
    auto id = params.fileName_ + options.sdCredentials.token();
    if (GenericFileManager::Managers.find(id) == GenericFileManager::Managers.end()) {
        if (isSDDataset(params.fileName_)) {
            GenericFileManager::Managers.try_emplace(id,
                                                     std::make_shared<SDFileManager>(
                                                         options.sdCredentials,
                                                         options.cacheSize,
                                                         options.sdLegalTag));
        }
        else {
            GenericFileManager::Managers.try_emplace(id, std::make_shared<DiskFileManager>());
        }
    }

    return GenericFileManager::Managers[id];
}

GenericFileManager::GenericFileManager()
{
}

void
GenericFileManager::reset()
{
    std::unique_lock<std::recursive_mutex> lock(GenericFileManager::Mutex);
    GenericFileManager::Managers.clear();
}

void
GenericFileManager::eraseCache(const FileReference& fileReference)
{
    std::unique_lock<std::recursive_mutex> lock(GenericFileManager::Mutex);

    auto id = fileReference.fileName() + fileReference.readerOptions().sdCredentials.token();
    GenericFileManager::Managers.erase(id);
}

bool
GenericFileManager::exists(const FileReference& fileReference)
{
    return getManager(fileReference)->exists(fileReference.fileName());
}

size_t
GenericFileManager::getSize(const FileReference& fileReference)
{
    return getManager(fileReference)->getSize(fileReference.fileName());
}

TextFileReadResult
GenericFileManager::readTextFile(const FileManagerParams& params)
{
    return getManager(params)->readTextFile(params);
}

bool
GenericFileManager::writeTextFile(const FileReference& fileReference, const std::string& text, bool allowOverwrite)
{
    return getManager(fileReference)->writeTextFile(fileReference.fileName(), text, allowOverwrite);
}

std::shared_ptr<FileReader>
GenericFileManager::createReader(const FileManagerParams& params)
{
    return getManager(params)->createReader(params);
}

std::shared_ptr<FileWriter>
GenericFileManager::createWriter(const FileReference& fileReference, WriteMode mode)
{
    return getManager(fileReference)->createWriter(fileReference.fileName(), mode);
}

std::shared_ptr<FileReader>
GenericFileManager::createRawReader(const FileManagerParams& params)
{
    return getManager(params)->createRawReader(params);
}

bool
GenericFileManager::remove(const FileReference& fileReference)
{
    return getManager(fileReference)->remove(fileReference.fileName());
}

bool
GenericFileManager::isSDDataset(const std::string& fileName)
{
    return FileManager::isSDDataset(fileName);
}

std::string
GenericFileManager::getSDLegalTag(const FileManagerParams& params)
{
    if (!isSDDataset(params.fileName_)) {
        return {};
    }
    const auto sd_manager = std::make_shared<SDFileManager>(params.readerOptions_.sdCredentials, params.readerOptions_.cacheSize);
    const auto reader = sd_manager->createRawReader(params);
    return std::dynamic_pointer_cast<fileio::SDFileReader>(reader)->getSDLegalTag();
}

FileManagerParams::FileManagerParams(const FileReference& reference)
  : FileManagerParams{request::Params{}, reference}
{
}

FileManagerParams::FileManagerParams(const request::Params& params, const FileReference& reference)
  : FileAccessParams{FileAccessParams{params}.withFileName(reference.fileName())}, readerOptions_{reference.readerOptions()}
{
}

FileManagerParams&
FileManagerParams::withReaderOptions(const FileReaderOptions& value)
{
    readerOptions_ = value;
    return *this;
}

FileManagerParams::FileManagerParams(const FileAccessParams& params)
  : FileAccessParams{params}
{
}

} // namespace segysdk::fileio
