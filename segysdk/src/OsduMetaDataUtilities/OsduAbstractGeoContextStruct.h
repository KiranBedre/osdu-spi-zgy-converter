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

#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <optional>

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractGeoContextStruct final : public OsduMetadataStructBase
{
    explicit OsduAbstractGeoContextStruct(std::string osduPartition);
    OsduAbstractGeoContextStruct(const OsduAbstractGeoContextStruct& other);
    OsduAbstractGeoContextStruct(OsduAbstractGeoContextStruct&& other) noexcept;
    OsduAbstractGeoContextStruct& operator=(const OsduAbstractGeoContextStruct& rhs);
    OsduAbstractGeoContextStruct& operator=(OsduAbstractGeoContextStruct&& rhs) noexcept;
    ~OsduAbstractGeoContextStruct() override = default;

    std::optional<std::string> geoTypeID;
    std::optional<std::string> geoPoliticalEntityID;
    std::optional<std::string> basinID;
    std::optional<std::string> fieldID;
    std::optional<std::string> playID;
    std::optional<std::string> prospectID;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;

private:
    void performValidityScan(std::vector<std::string>& results, const std::optional<std::string>& parameter, const std::string& tag) const;
    void performValidityScan(std::vector<std::string>& results, const std::optional<std::vector<std::string>>& parameters, const std::string& tag) const;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"GeoContext"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
