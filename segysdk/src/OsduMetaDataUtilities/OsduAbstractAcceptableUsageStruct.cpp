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

#include "../OsduMetaDataUtilities/OsduAbstractAcceptableUsageStruct.h"

namespace segysdk::metadata::osdu
{

OsduAbstractAcceptableUsageStruct::OsduAbstractAcceptableUsageStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractAcceptableUsageStruct::OsduAbstractAcceptableUsageStruct(const OsduAbstractAcceptableUsageStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    workflowUsageTypeID(other.workflowUsageTypeID),
    workflowPersonaTypeID(other.workflowPersonaTypeID),
    dataQualityRuleSetID(other.dataQualityRuleSetID),
    dataQualityID(other.dataQualityID),
    valueChainStatusTypeID(other.valueChainStatusTypeID)
{
    /* Empty */
}

OsduAbstractAcceptableUsageStruct::OsduAbstractAcceptableUsageStruct(OsduAbstractAcceptableUsageStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    workflowUsageTypeID(std::move(other.workflowUsageTypeID)),
    workflowPersonaTypeID(std::move(other.workflowPersonaTypeID)),
    dataQualityRuleSetID(std::move(other.dataQualityRuleSetID)),
    dataQualityID(std::move(other.dataQualityID)),
    valueChainStatusTypeID(std::move(other.valueChainStatusTypeID))
{
    /* Empty */
}

OsduAbstractAcceptableUsageStruct&
OsduAbstractAcceptableUsageStruct::operator=(const OsduAbstractAcceptableUsageStruct& rhs)
{
    return *this = OsduAbstractAcceptableUsageStruct(rhs);
}

OsduAbstractAcceptableUsageStruct&
OsduAbstractAcceptableUsageStruct::operator=(OsduAbstractAcceptableUsageStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    workflowUsageTypeID = std::move(rhs.workflowUsageTypeID);
    workflowPersonaTypeID = std::move(rhs.workflowPersonaTypeID);
    dataQualityRuleSetID = std::move(rhs.dataQualityRuleSetID);
    dataQualityID = std::move(rhs.dataQualityID);
    valueChainStatusTypeID = std::move(rhs.valueChainStatusTypeID);
    return *this;
}

bool
OsduAbstractAcceptableUsageStruct::isValid() const
{
    return workflowUsageTypeIDValid() and workflowPersonaTypeIDValid() and dataQualityRuleSetIDValid() and dataQualityIDValid() and valueChainStatusTypeIDValid();
}

bool
OsduAbstractAcceptableUsageStruct::workflowUsageTypeIDValid() const
{
    return !workflowUsageTypeID.has_value() or isValidOSDUReferenceIdString(workflowUsageTypeID.value());
}

bool
OsduAbstractAcceptableUsageStruct::workflowPersonaTypeIDValid() const
{
    return !workflowPersonaTypeID.has_value() or isValidOSDUReferenceIdString(workflowPersonaTypeID.value());
}

bool
OsduAbstractAcceptableUsageStruct::dataQualityRuleSetIDValid() const
{
    return !dataQualityRuleSetID.has_value() or isValidOSDUReferenceIdString(dataQualityRuleSetID.value());
}

bool
OsduAbstractAcceptableUsageStruct::dataQualityIDValid() const
{
    return !dataQualityID.has_value() or isValidOSDUReferenceIdString(dataQualityID.value());
}

bool
OsduAbstractAcceptableUsageStruct::valueChainStatusTypeIDValid() const
{
    return !valueChainStatusTypeID.has_value() or isValidOSDUReferenceIdString(valueChainStatusTypeID.value());
}

std::vector<std::string>
OsduAbstractAcceptableUsageStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> result{"OsduAbstractAcceptableUsageStruct:isValid() failed"};

    if (!workflowUsageTypeIDValid()) {
        result.emplace_back("OsduAbstractAcceptableUsageStruct:workflowUsageTypeIDValid() failed");
    }
    if (!workflowPersonaTypeIDValid()) {
        result.emplace_back("OsduAbstractAcceptableUsageStruct:workflowPersonaTypeIDValid() failed");
    }
    if (!dataQualityRuleSetIDValid()) {
        result.emplace_back("OsduAbstractAcceptableUsageStruct:dataQualityRuleSetIDValid() failed");
    }
    if (!dataQualityIDValid()) {
        result.emplace_back("OsduAbstractAcceptableUsageStruct:dataQualityIDValid() failed");
    }
    if (!valueChainStatusTypeIDValid()) {
        result.emplace_back("OsduAbstractAcceptableUsageStruct:valueChainStatusTypeIDValid() failed");
    }

    return result;
}

} // namespace segysdk::metadata::osdu
