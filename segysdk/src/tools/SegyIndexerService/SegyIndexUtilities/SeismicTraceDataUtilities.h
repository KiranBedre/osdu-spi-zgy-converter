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

#pragma once

#include "../LibraryLoader/PluginConfig.h"
#include "../SegyIndex/SegyIndex.h"
#include "../Utilities/StorageSvcClient.h"
#include "../Utilities/osdu/OsduSeismicTraceData140.h"

#include <string>
#include <optional>

namespace SegyIndexing::Utilities
{

class SeismicTraceDataUtilities
{
public:
    SeismicTraceDataUtilities(std::string recordId, const segysdk::library_loader::PluginConfig& pluginConfig);

    void savePolygonOutlineToSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index);

    void updateSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index);

private:
    std::optional<segysdk::utilities::osdu::OsduSeismicTraceData140> extractSeismicTraceDataFromWorkProductId(const std::string& workProductId);
    std::optional<segysdk::utilities::osdu::OsduSeismicTraceData140> getSeismicTraceDataFromId();
    void StoreOrSaveRecord(const std::string& metadataId, const std::string& metadataRecord);
    segysdk::fileio::FileReaderOptions getFileReaderOptions(const std::string& fileName, const std::string& legalTag = "");
    bool isMetadataID(const std::string& fileNameOrId);
    bool isLocalFile(const std::string& fileNameOrId);

private:
    const std::string pluginPath_;
    std::string workProductId_;
    std::string seismicTraceDataId_;
    std::string legalTag_ = "";
    const segysdk::utilities::StorageSvcClient storageSvcClient_;
};

} // namespace SegyIndexing::Utilities
