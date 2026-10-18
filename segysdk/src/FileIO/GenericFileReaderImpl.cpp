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

#include "../FileIO/GenericFileReaderImpl.h"

#include "../FileIO/GenericFileManager.h"
#include "../FileIO/SDCredentials.h"
#include "../Utilities/Environment.h"
#include "../Utilities/Settings.h"
#include "../Utilities/TokenSourceFactory.h"

#include "../segysdk_details/SegyException.h"

#include <iostream>

namespace segysdk::fileio
{

GenericFileReaderImpl::GenericFileReaderImpl(const std::string& fileName)
  : _fileName(fileName),
    _sdms_url{},
    _api_key{},
    _tokenSource{utilities::TokenSourceFactory::getTokenSource()},
    _headers{utilities::Settings::instance().getHttpHeaders()}
{
    if (_fileName.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl: parameter <fileName> cannot be empty.");

} // Intended for local files

GenericFileReaderImpl::GenericFileReaderImpl(const std::string& fileName,
                                             const std::string& sdms_url,
                                             const std::string& api_key,
                                             std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                                             std::map<std::string, std::string> headers) // Intended for cloud based files
  : _fileName(fileName),
    _sdms_url(sdms_url),
    _api_key(api_key),
    _tokenSource(std::move(tokenSource)),
    _headers(std::move(headers))
{
    if (_fileName.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl: parameter <fileName> cannot be empty.");

    if (_sdms_url.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl: parameter <sdms_url> cannot be empty.");

    if (_api_key.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl: parameter <api_key> cannot be empty.");

    if (_tokenSource->token().empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl: parameter <token> cannot be empty.");
}

std::string
GenericFileReaderImpl::readText()
{
    const auto& fileReference = getFileReference();

    if (GenericFileManager::exists(fileReference)) {
        return GenericFileManager::readTextFile(FileManagerParams{fileReference}).text_;
    }

    return {};
}

bool
GenericFileReaderImpl::exists()
{
    const auto file_reader_options = GenericFileManager::isSDDataset(_fileName)
                                         ? getFileReaderOptions(_sdms_url, _api_key, utilities::TokenSourceFactory::getTokenSource(), _headers)
                                         : getFileReaderOptions();

    const segysdk::fileio::FileReference file_reference(_fileName, file_reader_options);

    return fileio::GenericFileManager::exists(file_reference);
}

size_t
GenericFileReaderImpl::getSize()
{
    const auto file_reader_options = getFileReaderOptions();

    const segysdk::fileio::FileReference file_reference(_fileName, file_reader_options);

    return fileio::GenericFileManager::getSize(file_reference);
}

segysdk::fileio::FileReference
GenericFileReaderImpl::getFileReference()
{
    const segysdk::fileio::FileReaderOptions readerOptions = segysdk::fileio::GenericFileManager::isSDDataset(_fileName)
                                                                 ? getFileReaderOptions(_sdms_url, _api_key, utilities::TokenSourceFactory::getTokenSource(), _headers)
                                                                 : getFileReaderOptions();

    segysdk::fileio::FileReference fileReference(_fileName, readerOptions);

    return fileReference;
}

segysdk::fileio::FileReaderOptions
GenericFileReaderImpl::getFileReaderOptions()
{
    // All is read in one block,  so no need to set up a cached buffer ...
    segysdk::fileio::FileReaderOptions readerOptions = segysdk::fileio::GenericFileManager::isSDDataset(_fileName)
                                                           ? getFileReaderOptions(_sdms_url, _api_key, utilities::TokenSourceFactory::getTokenSource(), _headers)
                                                           : FileReaderOptions::localOnly();

    return readerOptions;
}

segysdk::fileio::FileReaderOptions
GenericFileReaderImpl::getFileReaderOptions(const std::string& sdms_url,
                                            const std::string& api_key,
                                            std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                                            std::map<std::string, std::string> headers)
{
    std::string my_sdms_url = sdms_url;
    std::string my_api_key = api_key;

    if (my_sdms_url.empty()) {
        my_sdms_url = segysdk::utilities::Environment::instance().sdUrl();
    }
    if (my_api_key.empty()) {
        my_api_key = segysdk::utilities::Environment::instance().sdApiKey();
    }

    if (my_sdms_url.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl called with empty sdms_url.");

    if (my_api_key.empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl called with empty api_key.");

    if (tokenSource->token().empty())
        throw SegyInvalidArgumentException("GenericFileReaderImpl called with empty token.");

    std::cout << "Seismic Data Management Services (SDMS) configured to access " << sdms_url << std::endl;
    std::cout << "SDMS configured with app key " << api_key << std::endl;

    const segysdk::SDCredentials credentials(sdms_url, api_key, std::move(tokenSource), headers);

    // All is read in one block,  but looks like we need a cache anyway now ...
    return {credentials, segysdk::CacheSizeOption(4 * 1048576, 1024, 1, 1024U * 1024U)};
}

} // namespace segysdk::fileio
