/*
Copyright 2022 Schlumberger

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

#include "SessionUtilities.h"
#include "JsonParseException.h"
#include "JsonToolKit.h"
#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../segysdk/SegySession.h"
#include "../segysdk/SegySessionFactory.h"
#include "../segysdk/SegySessionImpl.h"
#include "../Utilities/Environment.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/Settings.h"

#include <iostream>
#include <string>


namespace SampleCode::Common
{

bool
SessionUtilities::TestIfCloudSession(const std::string& file_name)
{
    const bool is_cloud_session = segysdk::fileio::GenericFileManager::isSDDataset(file_name);

    return is_cloud_session;
}

segysdk::CacheSizeOption
SessionUtilities::getSDCacheSize()
{
    constexpr size_t const default_pageSize = static_cast<size_t>(2U * 1024U * 1024U); // 2 megabytes
    constexpr size_t const default_Page_count = 32U;
    constexpr size_t const default_blocksPerPage = 1U;
    constexpr size_t const default_chunkSize = 1024U * 1024U;

    const auto sd_cache_page_size = segysdk::utilities::Environment::instance().sdReadCachePageSize();
    const auto sd_ChunkSize = sd_cache_page_size;
    const auto sd_max_pages = segysdk::utilities::Environment::instance().sdReadCacheMaxPages();
    const auto sd_BlocksPerPage = segysdk::utilities::Environment::instance().sdReadCacheBlocksPerPage();

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

    return segysdk::CacheSizeOption{pageSize, maxPages, blocksPerPage, chunkSize};
}

std::unique_ptr<segysdk::api::SegySession>
SessionUtilities::initSession(const bool& isCloudSession, const segysdk::api::SegySessionFactory& sessionFactory)
{
    if (isCloudSession) {

        // Pick up required environment variables
        const segysdk::SDCredentials sdCredentials(
            segysdk::utilities::Environment::instance().sdUrl(),
            segysdk::utilities::Environment::instance().sdApiKey(),
            segysdk::utilities::TokenSourceFactory::getTokenSource(),
            segysdk::utilities::Settings::instance().getHttpHeaders());

        const segysdk::CacheSizeOption cacheSize = isCloudSession ? getSDCacheSize() : segysdk::CacheSizeOption::none();

        return sessionFactory.CreateSegySession(sdCredentials, cacheSize);
    }

    return sessionFactory.CreateSegySession();
}

bool
SessionUtilities::isInputSEGYFileInCloud(const std::string& jsonParams)
{
    Json::Value params = JsonToolkit::getJsonFromString(jsonParams);

    if (params.isMember("data") && params["data"].isMember("DatasetProperties") && params["data"]["DatasetProperties"].isMember("FileSourceInfos")) {
        auto node = params["data"]["DatasetProperties"]["FileSourceInfos"];
        auto infos = node[0];

        if (infos.isMember("FileSource")) {
            const auto input_file = infos["FileSource"].asString();
            return TestIfCloudSession(input_file);
        }
    }
    const std::string msg = "isInputSEGYFileInCloud:  Input OSDU FileCollection parameter file could not be parsed successfully! \n";
    SEGYLOG_REPORT_ERROR(msg);
              
    throw JsonParseException(msg);
}

std::unique_ptr<segysdk::api::SegySession>
SessionUtilities::GetSegySessionFromFileCollectionJson(const std::string& osduFileCollectionJson)
{
    if (osduFileCollectionJson.empty()) {
        std::cout << std::endl
                  << " *** Input OSDU FileCollection parameter is empty. Exit program! *** " << std::endl
                  << std::endl;

        return nullptr;
    }

    try {
        // GetJsonFromFile will use appropriate reader depending on whether osduFileCollectionJson is stored on local disk or in the cloud.
        const std::string jsonParams = JsonToolkit::GetJsonFromFile(osduFileCollectionJson);

        const segysdk::api::SegySessionFactory sessionFactory(jsonParams);

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

std::shared_ptr<segysdk::segyindex::SegyIndex>
SessionUtilities::GetSegyIndex(const std::unique_ptr<segysdk::api::SegySession>& segySession)
{

    // Get the index file. The index contains useful information on how to navigate the SEG-Y file.
    return segySession->getIndex();
}

} // namespace SampleCode::Common
