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

#include <utility>

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSet.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetData.h"

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismic2DInterpretationSet::OsduSeismic2DInterpretationSet(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismic2DInterpretationSet_(osduPartition)
{
    /* Empty */
}
OsduSeismic2DInterpretationSet::OsduSeismic2DInterpretationSet(OsduSeismic2DInterpretationSetStruct seismic2DInterpretationSet)
  : OsduMetadataBase(seismic2DInterpretationSet.getPartition()), seismic2DInterpretationSet_(std::move(seismic2DInterpretationSet))
{
    populateJsonObject();
}

OsduSeismic2DInterpretationSet::OsduSeismic2DInterpretationSet(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismic2DInterpretationSet_(OsduSeismic2DInterpretationSetStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismic2DInterpretationSet::isValidInput() const
{
    return seismic2DInterpretationSet_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismic2DInterpretationSet::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismic2DInterpretationSetStruct>(seismic2DInterpretationSet_);
}

void
OsduSeismic2DInterpretationSet::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismic2DInterpretationSet::populateJsonObject()
{
    initializeJson();

    mergeInJson(OsduCommonMetadata(seismic2DInterpretationSet_.commonMetadataStruct).getAsJson());

    if (seismic2DInterpretationSet_.dataStruct.has_value()) {
        Json::Value additionJson = Json::objectValue;
        additionJson[OsduSeismic2DInterpretationSetLiterals::Data] = OsduSeismic2DInterpretationSetData(seismic2DInterpretationSet_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduSeismic2DInterpretationSet::initializeMetadataStruct()
{
    seismic2DInterpretationSet_ = OsduSeismic2DInterpretationSetStruct(getOsduPartition());
}

void
OsduSeismic2DInterpretationSet::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismic2DInterpretationSet_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    seismic2DInterpretationSet_.dataStruct = getOptionalStructParameter<OsduSeismic2DInterpretationSetData, OsduSeismic2DInterpretationSetDataStruct>(OsduSeismic2DInterpretationSetLiterals::Data);
}
} // namespace segysdk::metadata::osdu
