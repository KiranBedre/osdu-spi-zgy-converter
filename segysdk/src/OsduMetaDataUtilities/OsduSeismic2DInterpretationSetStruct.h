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

#include "../OsduMetaDataUtilities/OsduCommonMetadataStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetDataStruct.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismic2DInterpretationSetStruct final : public OsduMetadataStructBase
{
    explicit OsduSeismic2DInterpretationSetStruct(std::string osduPartition);
    OsduSeismic2DInterpretationSetStruct(const OsduSeismic2DInterpretationSetStruct& other) = default;
    OsduSeismic2DInterpretationSetStruct(OsduSeismic2DInterpretationSetStruct&& other) noexcept = default;
    OsduSeismic2DInterpretationSetStruct& operator=(const OsduSeismic2DInterpretationSetStruct& rhs) = default;
    OsduSeismic2DInterpretationSetStruct& operator=(OsduSeismic2DInterpretationSetStruct&& rhs) noexcept = default;
    ~OsduSeismic2DInterpretationSetStruct() override = default;

    OsduCommonMetadataStruct commonMetadataStruct;
    std::optional<OsduSeismic2DInterpretationSetDataStruct> dataStruct;

    bool isValid() const override;

    std::vector<std::string> performAttributeValidityScan() const override;

    void propagatePartition(const std::string& partition) override;

    OsduMetadataHelper getMetadataHelper() const { return helper_; }

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"master-data--Seismic2DInterpretationSet"};
    inline static constexpr const char* const classVersion_{"1.2.0"};

    OsduMetadataHelper helper_;
};

} // namespace segysdk::metadata::osdu
