/*
Copyright 2025 Schlumberger

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

#include "DataPartition.h"

#include "OsduMetaDataUtilities/OsduAbstractAnyCrsFeatureCollectionLiterals.h"
#include "OsduMetaDataUtilities/OsduAbstractSpatialLocationLiterals.h"
#include "OsduMetaDataUtilities/OsduAbstractWorkProductComponentLiterals.h"
#include "OsduMetaDataUtilities/OsduSeismicTraceDataLiterals.h"

#include <algorithm>

namespace indexer
{

constexpr auto idSeparator = ':';

static bool
isValidOsduReference(std::string_view value)
{
    // Disk path should have at most 1 colon: E:/seismic/Segy3D/Test/Test.json or sd://opendes/Test/test.json
    // OSDU id values will have 2 colons    : <partition>:<type>:<id value>
    // references should have 3             : <partition>:<type>:<id value>:<version>
    return std::count(value.begin(), value.end(), idSeparator) >= 2;
}

static std::string
getPartitionFromId(const Json::Value& object, const std::string& key)
{
    const auto value = object[key].asString();
    return isValidOsduReference(value) ? value.substr(0, value.find(idSeparator)) : std::string{};
}

std::string
getPartitionFromSeismicTraceData(const Json::Value& object)
{
    using namespace segysdk::metadata::osdu::OsduSeismicTraceDataLiterals;
    using namespace segysdk::metadata::osdu::OsduAbstractWorkProductComponentLiterals;
    using namespace segysdk::metadata::osdu::OsduAbstractSpatialLocationLiterals;
    using namespace segysdk::metadata::osdu::OsduAbstractAnyCrsFeatureCollectionLiterals;
    auto result = getPartitionFromId(object, Id);
    if (result.empty()) {
        result = getPartitionFromId(object[Data][SpatialArea], SpatialGeometryTypeID);
    }
    if (result.empty()) {
        result = getPartitionFromId(object[Data][SpatialArea][AsIngestedCoordinates], CoordinateReferenceSystemID);
    }
    return result.empty() ? "dummyPartition" : result;
}

} // namespace indexer
