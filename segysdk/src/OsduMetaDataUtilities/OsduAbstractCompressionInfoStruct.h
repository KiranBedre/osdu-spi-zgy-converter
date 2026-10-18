/*
Copyright 2024 Schlumberger

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
#include "../Utilities/ZgyOutputParams.h"

namespace segysdk::metadata::osdu
{

    struct SEGYSDK_DLL_PUBLIC OsduAbstractCompressionInfoStruct final : public OsduMetadataStructBase
{
    OsduAbstractCompressionInfoStruct(std::string osduPartition);
    OsduAbstractCompressionInfoStruct(const OsduAbstractCompressionInfoStruct& other) = default;
    OsduAbstractCompressionInfoStruct(OsduAbstractCompressionInfoStruct&& other) noexcept = default;
    OsduAbstractCompressionInfoStruct& operator=(const OsduAbstractCompressionInfoStruct& rhs) = default;
    OsduAbstractCompressionInfoStruct& operator=(OsduAbstractCompressionInfoStruct&& rhs) noexcept = default;
    ~OsduAbstractCompressionInfoStruct() override = default;

    std::optional<bool> lossyCompressionIndicator;
    std::optional<std::string> compressionMethodTypeID;
    std::optional<double> compressionLevel;
    std::optional<std::string> compressedByteCount;
    std::optional<std::string> uncompressedByteCount;

    bool compressionMethodTypeIdValid() const;
    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;
    void setCompressionInfo(segysdk::utilities::ZgyOutputParams zgyOutputParameters, size_t fileSize);

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"AbstractCompressionInfo"};
    inline static constexpr const char* const classVersion_{"1.1.0"};

    
};

} // namespace segysdk::metadata::osdu
