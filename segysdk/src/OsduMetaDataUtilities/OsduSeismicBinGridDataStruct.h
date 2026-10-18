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
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractBinGrid.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismicBinGridDataStruct final : public OsduMetadataStructBase
{
    OsduSeismicBinGridDataStruct(std::string osduPartition);
    OsduSeismicBinGridDataStruct(const OsduSeismicBinGridDataStruct& other) = default;
    OsduSeismicBinGridDataStruct(OsduSeismicBinGridDataStruct&& other) noexcept = default;
    OsduSeismicBinGridDataStruct& operator=(const OsduSeismicBinGridDataStruct& rhs) = default;
    OsduSeismicBinGridDataStruct& operator=(OsduSeismicBinGridDataStruct&& rhs) noexcept = default;
    ~OsduSeismicBinGridDataStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResourcesStruct;
    std::optional<OsduAbstractWPCGroupTypeStruct> wpcGroupTypeStruct;
    std::optional<OsduAbstractWorkProductComponentStruct> workProductComponentStruct;
    std::optional<OsduAbstractBinGridStruct> binGridStruct;

    bool isValid() const override;
    bool isCompleteP6Definition() const;

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
    inline static constexpr const char* const className_{"SeismicBinGridData"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
