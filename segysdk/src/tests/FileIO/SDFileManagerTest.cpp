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

#include "../FileIO/SDFileManager.h"
#include "../Utilities/Environment.h"
#include "../Utilities/Settings.h"
#include "../Utilities/TokenSourceFactory.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <map>

using namespace segysdk;
using namespace segysdk::fileio;
using namespace segysdk::utilities;

TEST(SegyIOTests, test_http_header_when_nothing_is_set)
{
    auto sdFileManager = SDFileManager(SDCredentials(
                                           "https://sdurl.fake",
                                           "fake-api-key",
                                           auth::TokenSource::fromToken("fake-test-token")),
                                       CacheSizeOption::none());
    auto sdManager = sdFileManager.getSDManager();
    auto header = sdManager->copyHttpHeaders();

    ASSERT_EQ(header.find("correlation-id"), header.end());

    const auto osdu_partition = Environment::instance().osduDataPartitionId();
    if (osdu_partition.empty()) {

        ASSERT_EQ(header.find("data-partition-id"), header.end());
    }
    else {
        const auto hdr_partition = header.find("data-partition-id");
        ASSERT_NE(hdr_partition, header.end());
        ASSERT_STREQ(hdr_partition->second.c_str(), osdu_partition.c_str());
    }
}

TEST(SegyIOTests, test_http_header_when_it_is_explicitly_set)
{
    std::map<std::string, std::string> headers;

    headers.emplace("correlation-id", "test-correlation-id");
    headers.emplace("data-partition-id", "test-partition-id");
    auto sdFileManager = SDFileManager(SDCredentials(
                                           "https://sdurl.fake",
                                           "fake-api-key",
                                           auth::TokenSource::fromToken("fake-test-token"),
                                           headers),
                                       CacheSizeOption::none());
    auto sdManager = sdFileManager.getSDManager();
    auto header = sdManager->copyHttpHeaders();

    auto correlationId_it = header.find("correlation-id");
    auto dataPartitionId_it = header.find("data-partition-id");

    ASSERT_NE(correlationId_it, header.end());
    ASSERT_NE(dataPartitionId_it, header.end());
    ASSERT_STREQ(correlationId_it->second.c_str(), "test-correlation-id");
    ASSERT_STREQ(dataPartitionId_it->second.c_str(), "test-partition-id");
}

TEST(SegyIOTests, test_http_header_overwrite)
{
    std::map<std::string, std::string> headers;

    headers.emplace("correlation-id", "test-correlation-id");
    headers.emplace("data-partition-id", "test-partition-id");
    auto sdFileManager = SDFileManager(SDCredentials(
                                           "https://sdurl.fake",
                                           "fake-api-key",
                                           auth::TokenSource::fromToken("fake-test-token"),
                                           headers),
                                       CacheSizeOption::none());
    auto sdManager = sdFileManager.getSDManager();
    auto header = sdManager->copyHttpHeaders();

    auto correlationId_it = header.find("correlation-id");
    auto dataPartitionId_it = header.find("data-partition-id");

    ASSERT_NE(correlationId_it, header.end());
    ASSERT_NE(dataPartitionId_it, header.end());
    ASSERT_STREQ(correlationId_it->second.c_str(), "test-correlation-id");
    ASSERT_STREQ(dataPartitionId_it->second.c_str(), "test-partition-id");

    std::map<std::string, std::string> headersUpdated;

    headersUpdated.emplace("correlation-id", "test-correlation-id-updated");
    headersUpdated.emplace("data-partition-id", "test-partition-id-updated");

    auto sdFileManagerUpdated = SDFileManager(SDCredentials(
                                                  "https://sdurl.fake",
                                                  "fake-api-key",
                                                  auth::TokenSource::fromToken("fake-test-token"),
                                                  headersUpdated),
                                              CacheSizeOption::none());
    auto sdManagerUpdated = sdFileManagerUpdated.getSDManager();
    auto headerUpdated = sdManagerUpdated->copyHttpHeaders();
    auto correlationIdUpdated_it = headerUpdated.find("correlation-id");
    auto dataPartitionIdUpdated_it = headerUpdated.find("data-partition-id");

    ASSERT_NE(correlationIdUpdated_it, headerUpdated.end());
    ASSERT_NE(dataPartitionIdUpdated_it, headerUpdated.end());
    ASSERT_STREQ(correlationIdUpdated_it->second.c_str(), "test-correlation-id-updated");
    ASSERT_STREQ(dataPartitionIdUpdated_it->second.c_str(), "test-partition-id-updated");
}
