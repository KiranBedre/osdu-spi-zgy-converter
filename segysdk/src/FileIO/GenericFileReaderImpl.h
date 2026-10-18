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

// TODO literal names are mentioned several times in the documentation comments, but how does the user know what are the exact string values?

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileReader.h"
#include "../Utilities/auth/TokenSource.h"

#include <string>
#include <map>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC GenericFileReaderImpl : public segysdk::fileio::GenericFileReader
{
private:
    std::string _fileName{};
    std::string _sdms_url{};
    std::string _api_key{};
    std::unique_ptr<segysdk::utilities::auth::TokenSource> _tokenSource;
    std::map<std::string, std::string> _headers;

private:
    segysdk::fileio::FileReaderOptions getFileReaderOptions();
    static segysdk::fileio::FileReaderOptions getFileReaderOptions(const std::string& sdms_url,
                                                                   const std::string& api_key,
                                                                   std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                                                                   std::map<std::string, std::string> headers = std::map<std::string, std::string>());

public:
    
    GenericFileReaderImpl(const std::string& fileName);
    GenericFileReaderImpl(const std::string& fileName,
                          const std::string& sdms_url,
                          const std::string& api_key,
                          std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                          std::map<std::string, std::string> headers = std::map<std::string, std::string>());

public:
    segysdk::fileio::FileReference getFileReference() override;


    std::string readText() override;

    bool exists() override;

    size_t getSize() override;
};

} // namespace segysdk::fileio
