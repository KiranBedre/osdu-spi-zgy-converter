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

#include "../OsduMetaDataUtilities/OsduMetafileApi.h"

#include <gtest/gtest.h>
#include <string>

namespace segysdk::osduMetadata::OsduOverridesTests
{

using namespace segysdk::metadata::osdu;

TEST(OsduMetadata, OsduOverridesTests1)
{
    const auto partition{"opendes"};

    const OsduMetadataHelper helper(partition);

    OsduAbstractCommonResourcesStruct acrStruct(partition);
    acrStruct.existenceKind = helper.osduReferenceLookupString("reference-data--ExistenceKind", "Prototype");
    acrStruct.resourceCurationStatus = helper.osduReferenceLookupString("reference-data--ResourceCurationStatus", "Created");
    acrStruct.source = "Example Data Source";
    const OsduAbstractCommonResources acr(acrStruct);

    ASSERT_TRUE(acr.isValidInput());
    const std::vector<std::string> expected_members{OsduAbstractCommonResourcesLiterals::ExistenceKind, OsduAbstractCommonResourcesLiterals::ResourceCurationStatus, OsduAbstractCommonResourcesLiterals::Source};
    ASSERT_EQ(acr.getJsonMemberNames(), expected_members);
}

TEST(OsduMetadata, OsduOverridesTests2)
{
    const auto partition{"opendes"};

    const OsduMetadataHelper helper(partition);

    OsduAbstractCommonResourcesStruct acrStruct(partition);
    acrStruct.existenceKind = helper.osduReferenceLookupString("reference-data--ExistenceKind", "Prototype");
    acrStruct.resourceCurationStatus = helper.osduReferenceLookupString("reference-data--ResourceCurationStatus", "Created");
    acrStruct.source = "Example Data Source";
    OsduAbstractCommonResources acr(acrStruct);

    const std::shared_ptr<OsduAbstractCommonResourcesStruct> acrStructPtr = std::reinterpret_pointer_cast<OsduAbstractCommonResourcesStruct>(acr.getMetaDataStruct());

    ASSERT_FALSE(acrStructPtr == nullptr);
    ASSERT_EQ(acrStruct.existenceKind, acrStructPtr->existenceKind);
    ASSERT_EQ(acrStruct.resourceCurationStatus, acrStructPtr->resourceCurationStatus);

    acrStructPtr->resourceCurationStatus = helper.osduReferenceLookupString("reference-data--ResourceCurationStatus", "Deleted");
    ASSERT_NE(acrStruct.resourceCurationStatus, acrStructPtr->resourceCurationStatus);

    ASSERT_TRUE(acrStructPtr->existenceKind.has_value());
    ASSERT_TRUE(acrStructPtr->resourceCurationStatus.has_value());

    ASSERT_TRUE(acrStructPtr->source.has_value());

    ASSERT_FALSE(acrStructPtr->resourceLifecycleStatus.has_value());
}

} // namespace segysdk::osduMetadata::tests
