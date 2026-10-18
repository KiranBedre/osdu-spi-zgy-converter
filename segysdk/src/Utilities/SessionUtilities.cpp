/*
Copyright 2023 Schlumberger

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

#include "../Utilities/SessionUtilities.h"

#include "../FileIO/GenericFileManager.h"

#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/JsonException.h"
#include "../Utilities/TokenSourceFactory.h"


#include <iostream>
#include <string>
#include "Settings.h"

namespace segysdk::utilities
{

bool
SessionUtilities::TestIfCloudSession(const std::string& file_name)
{
    const bool is_cloud_session = fileio::GenericFileManager::isSDDataset(file_name);

    return is_cloud_session;
}

CacheSizeOption
SessionUtilities::getSDCacheSize()
{
    constexpr size_t const default_pageSize = static_cast<size_t>(2U * 1024U * 1024U); // 2 megabytes
    constexpr size_t const default_Page_count = 32U;
    constexpr size_t const default_blocksPerPage = 1U;
    constexpr size_t const default_chunkSize = static_cast<size_t>(1024U * 1024U);

    const auto sd_cache_page_size = Environment::instance().sdReadCachePageSize();
    const auto sd_ChunkSize = sd_cache_page_size;
    const auto sd_max_pages = Environment::instance().sdReadCacheMaxPages();
    const auto sd_BlocksPerPage = Environment::instance().sdReadCacheBlocksPerPage();


    auto pageSize = default_pageSize;
    if (sd_cache_page_size) {
        pageSize = sd_cache_page_size;
    }

    auto maxPages = default_Page_count;
    if (sd_max_pages) {
        maxPages = sd_max_pages;
    }

    auto blocksPerPage = default_blocksPerPage;
    if (sd_BlocksPerPage) {
        blocksPerPage = sd_BlocksPerPage;
    }

    auto chunkSize = default_chunkSize;
    if (sd_ChunkSize) {
        chunkSize = sd_ChunkSize;
    }

    return CacheSizeOption{pageSize, maxPages, blocksPerPage, chunkSize};
}

std::unique_ptr<api::SegySession>
SessionUtilities::initSession(const bool& isCloudSession, const api::SegySessionFactory& sessionFactory)
{
    if (isCloudSession) {

        // Pick up required environment variables
        const segysdk::SDCredentials sdCredentials(
            segysdk::utilities::Environment::instance().sdUrl(),
            segysdk::utilities::Environment::instance().sdApiKey(),
            segysdk::utilities::TokenSourceFactory::getTokenSource(),
            segysdk::utilities::Settings::instance().getHttpHeaders());

        const CacheSizeOption cacheSize = isCloudSession ? getSDCacheSize() : CacheSizeOption::none();

        return sessionFactory.CreateSegySession(sdCredentials, cacheSize);
    }

    return sessionFactory.CreateSegySession();
}

bool
SessionUtilities::isInputSEGYFileInCloud(const std::string& jsonParams)
{
    Json::Value params = JsonUtilities::getJsonFromString(jsonParams);

    if (params.isMember("data") && params["data"].isMember("DatasetProperties") && params["data"]["DatasetProperties"].isMember("FileSourceInfos")) {
        auto node = params["data"]["DatasetProperties"]["FileSourceInfos"];
        auto infos = node[0];

        if (infos.isMember("FileSource")) {
            const auto input_file = infos["FileSource"].asString();
            return TestIfCloudSession(input_file);
        }
    }
    std::cout << std::endl
              << " isInputSEGYFileInCloud:  Input OSDU FileCollection parameter file could not be parsed successfully! " << std::endl
              << std::endl;
    throw JsonParseException("Input OSDU FileCollection parameter file could not be parsed successfully!");
}

std::unique_ptr<api::SegySession>
SessionUtilities::GetSegySessionFromFileCollectionJson(const std::string& osduFileCollectionJson)
{
    if (osduFileCollectionJson.empty()) {
        std::cout << std::endl
                  << " *** Input OSDU FileCollection parameter is empty. Exit program! *** " << std::endl
                  << std::endl;

        return nullptr;
    }

    try {
        const std::string jsonParams = osduFileCollectionJson.find(fileCollectionClass_) == std::string::npos
                                           ? JsonUtilities::GetJsonStringFromFile(osduFileCollectionJson)
                                           : JsonUtilities::GetJsonFromId(osduFileCollectionJson);

        const api::SegySessionFactory sessionFactory(jsonParams);

        // Check if referenced SEG-Y file is local or cloud hosted
        const bool isCloudSession = isInputSEGYFileInCloud(jsonParams);
        auto session = initSession(isCloudSession, sessionFactory);

        return session;
    }
    catch (std::exception& ex) {
        std::cout << "Unable to create SEGY Session!" << std::endl;
        std::cout << "Exception: " << ex.what() << std::endl;
    }
    return nullptr;
}

std::shared_ptr<segyindex::SegyIndex>
SessionUtilities::GetSegyIndex(const std::unique_ptr<api::SegySession>& segySession)
{

    // Get the index file. The index contains useful information on how to navigate the SEG-Y file.
    auto index = segySession->getIndex();

    return index;
}

std::shared_ptr<segyindex::SegyIndex>
SessionUtilities::GetSegyIndex(const std::shared_ptr<api::SegySession>& segySession)
{
    // Get the index file. The index contains useful information on how to navigate the SEG-Y file.
    auto index = segySession->getIndex();

    return index;
}

} // namespace segysdk::utilities

