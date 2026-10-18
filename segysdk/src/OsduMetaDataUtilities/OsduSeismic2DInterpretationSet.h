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

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduSeismic2DInterpretationSet final : public OsduMetadataBase
{
public:
    OsduSeismic2DInterpretationSet(const std::string& osduPartition);
    OsduSeismic2DInterpretationSet(const OsduSeismic2DInterpretationSet& other) = default;
    OsduSeismic2DInterpretationSet(OsduSeismic2DInterpretationSet&& other) noexcept = default;
    OsduSeismic2DInterpretationSet& operator=(const OsduSeismic2DInterpretationSet& rhs) = default;
    OsduSeismic2DInterpretationSet& operator=(OsduSeismic2DInterpretationSet&& rhs) noexcept = default;
    ~OsduSeismic2DInterpretationSet() override = default;

    explicit OsduSeismic2DInterpretationSet(OsduSeismic2DInterpretationSetStruct seismic2DInterpretationSet);
    explicit OsduSeismic2DInterpretationSet(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduSeismic2DInterpretationSetStruct seismic2DInterpretationSet_;
};
} // namespace segysdk::metadata::osdu
