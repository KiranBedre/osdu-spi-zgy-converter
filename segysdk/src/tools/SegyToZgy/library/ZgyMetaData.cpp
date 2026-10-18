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

#include "ZgyMetaData.h"

#include "../Utilities/Adapters.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/SafeTime.h"
#include "../Utilities/StringUtils.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <SDAPI/SDGenericDataset.h>
#include <SDAPI/SDManager.h>
#pragma warning(pop)

#include <json/json.h>

#include <ctime>
#include <iomanip>
#include <iostream>

using seismicdrive::SDDatasetDisposition;
using seismicdrive::SDGenericDataset;
using seismicdrive::SDManager;

std::string
ZgyMetaData::getZgyVersionGuid(SDGenericDataset& dataset)
{
    // In lack of a public API, read and convert the raw data
    uint8_t guid[16] = {};
    dataset.readBlock(0, reinterpret_cast<char*>(guid), 46, 16);

    // String conversion from OpenZgy
    // Use local conversion to avoid cross-platform intricacies
    // Note that the algorithm handles the zgy case, but is not a generic solution
    const char hex[17]{"0123456789abcdef"};
    char result[37]{0};
    char* cp = &result[0];
    char* end = &result[sizeof(result) - 1];
    for (std::size_t ii = 0; ii < sizeof(guid) && cp < end; ++ii) {
        *cp++ = hex[(guid[ii] >> 4) & 0x0f];
        *cp++ = hex[(guid[ii] >> 0) & 0x0f];
        if (ii == 3 || ii == 5 || ii == 7 || ii == 9)
            *cp++ = '-';
    }
    *cp++ = '\0';
    return std::string(result);
}

std::string
ZgyMetaData::getTimeAsString()
{
    // Get current time
    // Would be better to getCreatedDate() from the dataset, but that would require parsing and reformatting
    std::time_t time = std::time(nullptr);
    std::tm tm{};
    segysdk::utilities::safe_gmtime(time, tm);
    std::stringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    return stream.str();
}

bool
ZgyMetaData::clone(SDManager* manager, const std::string& sourceMetaData, const std::string& targetFile)
{
    if (!segysdk::utilities::isSDDataset(targetFile))
        return false;

    if (sourceMetaData.empty()) {
        SEGYLOG_GSM_ERROR("ZgyMetaData::clone Source metadata is empty");
        return false;
    }

    Json::Value root = segysdk::utilities::JsonUtilities::getJsonFromString(sourceMetaData);

    // Update pertinent parts of the metadata
   
    SDGenericDataset targetDataset = SDGenericDataset(manager, targetFile);
    targetDataset.open(SDDatasetDisposition::READ_ONLY);

    // Write metadata to target dataset
    std::string targetMetaData = root.toStyledString();
    targetDataset.setSeismicMeta(targetMetaData);
    targetDataset.close();

    return true;
}

bool
ZgyMetaData::setOnGeneratedZgyFile(seismicdrive::SDManager* manager, const std::string& stdMetaData, const std::string& targetFile)
{
    if (!segysdk::utilities::isSDDataset(targetFile))
        return false;

    if (stdMetaData.empty()) {
        SEGYLOG_GSM_ERROR("ZgyMetaData::SeismicTraceData metadata is empty");
        return false;
    }

    SDGenericDataset targetDataset = SDGenericDataset(manager, targetFile);
    if (!targetDataset.exists()) {
        return false;
    }
    targetDataset.open(SDDatasetDisposition::READ_ONLY);

    // Write metadata to target dataset

    targetDataset.setSeismicMeta(stdMetaData);
    targetDataset.close();

    return true;

}
