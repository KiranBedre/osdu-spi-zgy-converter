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

#include "../OsduMetaDataUtilities/OsduMetaElementStruct.h"

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfoStruct.h"

namespace segysdk::metadata::osdu
{

OsduMetaElementStruct::OsduMetaElementStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduMetaElementStruct::OsduMetaElementStruct(const OsduMetaElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    kind(other.kind),
    name(other.name),
    persistableReference(other.persistableReference),
    unitOfMeasureID(other.unitOfMeasureID),
    propertyNames(other.propertyNames)
{
    /* Empty */
}

OsduMetaElementStruct::OsduMetaElementStruct(OsduMetaElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    kind(std::move(other.kind)),
    name(std::move(other.name)),
    persistableReference(std::move(other.persistableReference)),
    unitOfMeasureID(std::move(other.unitOfMeasureID)),
    propertyNames(std::move(other.propertyNames))
{
    /* Empty */
}

OsduMetaElementStruct&
OsduMetaElementStruct::operator=(const OsduMetaElementStruct& rhs)
{
    return *this = OsduMetaElementStruct(rhs);
}

OsduMetaElementStruct&
OsduMetaElementStruct::operator=(OsduMetaElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    kind = std::move(rhs.kind);
    name = std::move(rhs.name);
    persistableReference = std::move(rhs.persistableReference);
    unitOfMeasureID = std::move(rhs.unitOfMeasureID);
    propertyNames = std::move(rhs.propertyNames);

    return *this;
}

bool 
OsduMetaElementStruct::operator==(const OsduMetaElementStruct& rhs)
{
    bool propsOK =  (propertyNames.has_value() and rhs.propertyNames.has_value())
        or (!propertyNames.has_value() and !rhs.propertyNames.has_value());
    // Do NOT check that propertyNames and count are exact matches as this will not necessarily
    // be the case durin meta consolidation work (ensuring trace property names use same unit)

    return kind == rhs.kind
           and name == rhs.name
           and unitOfMeasureID == rhs.unitOfMeasureID
           and persistableReference == rhs.persistableReference
           and propsOK;
}


OsduMetaElementStruct::OsduMetaElementStruct(std::string osduPartition, std::string metaKind, std::string metaPersistableReference)
    : OsduMetadataStructBase(std::move(osduPartition)), kind(std::move(metaKind)), persistableReference(std::move(metaPersistableReference))
{
    /* Empty */
}

bool
OsduMetaElementStruct::isValid() const
{
    return !kind.empty() or !persistableReference.empty();
}

std::vector<std::string>
OsduMetaElementStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;

    if (!isValid()) {
        results.emplace_back("OsduMetaElementStruct::isValid() failed.");

        if (kind.empty()) {
            results.emplace_back("Kind cannot be empty.");
        }
        else {
            if (!isValidOSDUReferenceIdString(kind)) {
                results.emplace_back("Kind is not of expected format.");
            }
        }
        if (persistableReference.empty()) {
            results.emplace_back("PersistableReference cannot be empty.");
        }
        else if (!isValidOSDUReferenceIdString(persistableReference)) {
            results.emplace_back("PersistableReference is not of expected format.");
        }
    }

    return results;
}

} // namespace segysdk::metadata::osdu
