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

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduPersistableMetadataBase : public OsduMetadataBase
{
public:
    OsduPersistableMetadataBase(std::string osduPartition);
    OsduPersistableMetadataBase(const OsduPersistableMetadataBase& other) = default;
    OsduPersistableMetadataBase(OsduPersistableMetadataBase&& other) noexcept = default;
    OsduPersistableMetadataBase& operator=(const OsduPersistableMetadataBase& rhs) = default;
    OsduPersistableMetadataBase& operator=(OsduPersistableMetadataBase&& rhs) noexcept = default;
    virtual ~OsduPersistableMetadataBase() override = default;

public:
    /// \brief Method to read a valid Json schema structure from disk or cloud storage
    /// \param osdu_record_id       OSDU Id or file path of entity to be imported
    /// \return                     restored Json object
    virtual Json::Value importSchema(const std::string& osdu_record_id) = 0;

    /// \brief Method to save a valid Json structure to disk or the cloud storage
    /// \param osdu_record_id       Id of entity to be saved
    /// \param save_as_styled_json  If true, save as pretty print, otherwise save as compact string
    /// \return saved record id
    virtual std::string saveSchema(const std::string& osdu_record_id, bool save_as_styled_json = false) const = 0;

};

} // namespace segysdk::metadata::osdu
