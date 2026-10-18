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

#include "../OsduMetaDataUtilities/OsduCommonMetadataStruct.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificPropertiesStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismicTraceDataStruct final : public OsduMetadataStructBase
{
    OsduSeismicTraceDataStruct(std::string osduPartition);
    OsduSeismicTraceDataStruct(const OsduSeismicTraceDataStruct& other) = default;
    OsduSeismicTraceDataStruct(OsduSeismicTraceDataStruct&& other) noexcept = default;
    OsduSeismicTraceDataStruct& operator=(const OsduSeismicTraceDataStruct& rhs) = default;
    OsduSeismicTraceDataStruct& operator=(OsduSeismicTraceDataStruct&& rhs) noexcept = default;
    ~OsduSeismicTraceDataStruct() override = default;

    OsduCommonMetadataStruct commonMetadata;
    std::optional<OsduSeismicTraceDataSpecificPropertiesStruct> seismicTraceDataSpecificProperties;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;
    void propagatePartition(const std::string& partition) override;
    void addArtefact(const std::string& newId, const std::string& newObjectKind);
    
    OsduMetadataHelper getMetadataHelper() const { return helper_; }

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    std::string getArtefactRoleIdFromKind(const std::string& artefactKind) const;

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"work-product-component--SeismicTraceData"};
    inline static constexpr const char* const classVersion_{"1.4.0"};

    OsduMetadataHelper helper_;

public:
    inline static constexpr const char* const ReferenceArtefactRole{"reference-data--ArtefactRole"};
    inline static constexpr const char* const ConvertedContent{"ConvertedContent"};
    inline static constexpr const char* const ExtractedContent{"ExtractedContent"};
    inline static constexpr const char* const AdaptedContent{"AdaptedContent"};

    inline static constexpr const char* const AmplitudeSeismic{"reference-data--SeismicAttributeType:Amplitude"};
};

} // namespace segysdk::metadata::osdu
