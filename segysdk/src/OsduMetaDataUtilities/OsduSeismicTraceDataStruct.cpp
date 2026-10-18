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

#include "../OsduMetaDataUtilities/OsduSeismicTraceDataStruct.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismicTraceDataStruct::OsduSeismicTraceDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    commonMetadata(getPartition()),
    helper_(getPartition())
{
    /* Empty */
}

bool
OsduSeismicTraceDataStruct::isValid() const
{
    return structValid(commonMetadata) and structValid(seismicTraceDataSpecificProperties);
}

std::vector<std::string>
OsduSeismicTraceDataStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduSeismicTraceDataStruct::isValid() failed.");

    performStructValidityScan(results, commonMetadata);
    performStructValidityScan(results, seismicTraceDataSpecificProperties);

    return results;
}

void
OsduSeismicTraceDataStruct::propagatePartition(const std::string& partition)
{
    commonMetadata.setPartition(partition);
    if (seismicTraceDataSpecificProperties.has_value()) {
        seismicTraceDataSpecificProperties->setPartition(partition);
    }
}

std::string
OsduSeismicTraceDataStruct::getArtefactRoleIdFromKind(const std::string& artefactKind) const
{
    if (artefactKind.find(OsduFileCollectionOpenZgyStruct::getClassName()) != std::string::npos) {
        return helper_.osduReferenceLookupString(ReferenceArtefactRole, ConvertedContent);
    }

    return helper_.osduReferenceLookupString(ReferenceArtefactRole, AdaptedContent);
}

void
OsduSeismicTraceDataStruct::addArtefact(const std::string& artefactId, const std::string& artefactKind)
{
    if (!seismicTraceDataSpecificProperties.has_value()) {
        seismicTraceDataSpecificProperties = OsduSeismicTraceDataSpecificPropertiesStruct(getPartition());
    }
    if (!seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.has_value()) {
        seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct = OsduAbstractWPCGroupTypeStruct(getPartition());
    }
    auto artefacts = seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.has_value()
                         ? seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.value()
                         : std::vector<OsduAbstractArtefactStruct>();

    OsduAbstractArtefactStruct artefact(getPartition());
    artefact.roleID = getArtefactRoleIdFromKind(artefactKind);
    artefact.resourceKind = artefactKind;
    artefact.resourceID = helper_.osduReferenceLookupString(artefactId);

    artefacts.emplace_back(artefact);
    seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts = artefacts;
}


} // namespace segysdk::metadata::osdu
