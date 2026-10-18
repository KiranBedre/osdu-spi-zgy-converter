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

#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

#include "../SegyIO/SampleFormat.h"

#include <openzgy/api.h>

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractSeismicPrecisionStruct final : public OsduMetadataStructBase
{
    explicit OsduAbstractSeismicPrecisionStruct(std::string osduPartition);
    OsduAbstractSeismicPrecisionStruct(const OsduAbstractSeismicPrecisionStruct& other);
    OsduAbstractSeismicPrecisionStruct(OsduAbstractSeismicPrecisionStruct&& other) noexcept;
    OsduAbstractSeismicPrecisionStruct& operator=(const OsduAbstractSeismicPrecisionStruct& rhs);
    OsduAbstractSeismicPrecisionStruct& operator=(OsduAbstractSeismicPrecisionStruct&& rhs) noexcept;
    ~OsduAbstractSeismicPrecisionStruct() override = default;

    std::optional<int> wordWidth;
    std::optional<std::string> wordFormat;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

    void setPrecision(const segysdk::segyio::SampleFormat sampleFormat);
    void setPrecision(const OpenZGY::SampleDataType zgySampleType);

private:
    std::string getPrecisionWord(const OpenZGY::SampleDataType sampleSize) const;
    std::string getPrecisionWord(const segysdk::segyio::SampleFormat sampleFormat) const;
    int getSampleSize(const OpenZGY::SampleDataType sampleSize) const;
    int getSampleSize(const segysdk::segyio::SampleFormat sampleFormat) const;

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"SeismicPrecision"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    OsduMetadataHelper helper_;

public:
    inline static constexpr const char* const WordFormatTypeReference{"reference-data--WordFormatType"};
    inline static constexpr const char* const WordFormatINT{"INT"};
    inline static constexpr const char* const WordFormatIBMFLOAT{"IBMFLOAT"};
    inline static constexpr const char* const WordFormatIEEEFLOAT{"IEEEFLOAT"};

    
};

} // namespace segysdk::metadata::osdu
