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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractMasterStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractProjectActivityStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractProjectStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryRefStruct.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismic2DInterpretationSetDataStruct final : public OsduMetadataStructBase
{
    explicit OsduSeismic2DInterpretationSetDataStruct(std::string osduPartition);
    OsduSeismic2DInterpretationSetDataStruct(const OsduSeismic2DInterpretationSetDataStruct& other) = default;
    OsduSeismic2DInterpretationSetDataStruct(OsduSeismic2DInterpretationSetDataStruct&& other) noexcept = default;
    OsduSeismic2DInterpretationSetDataStruct& operator=(const OsduSeismic2DInterpretationSetDataStruct& rhs) = default;
    OsduSeismic2DInterpretationSetDataStruct& operator=(OsduSeismic2DInterpretationSetDataStruct&& rhs) noexcept = default;
    ~OsduSeismic2DInterpretationSetDataStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResourcesStruct;
    std::optional<OsduAbstractMasterStruct> masterStruct;
    std::optional<OsduAbstractProjectStruct> projectStruct;
    std::optional<OsduAbstractProjectActivityStruct> activityStruct;
    std::optional<std::vector<OsduSeismicLineGeometryRefStruct>> seismicLineGeometriesStruct;

    bool isValid() const override;

    std::vector<std::string> performAttributeValidityScan() const override;

    void propagatePartition(const std::string& partition) override;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"AbstractProject"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
