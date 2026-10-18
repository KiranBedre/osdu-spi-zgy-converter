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

#include "../OsduMetaDataUtilities/OsduAbstractFeatureCollectionStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractFeatureCollectionStruct::OsduAbstractFeatureCollectionStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

OsduAbstractFeatureCollectionStruct::OsduAbstractFeatureCollectionStruct(const OsduAbstractFeatureCollectionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    type(other.type),
    features(other.features),
    bbox(other.bbox)
{
    /* Empty */
}

OsduAbstractFeatureCollectionStruct::OsduAbstractFeatureCollectionStruct(OsduAbstractFeatureCollectionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    type(std::move(other.type)),
    features(std::move(other.features)),
    bbox(std::move(other.bbox))
{
    /* Empty */
}

OsduAbstractFeatureCollectionStruct&
OsduAbstractFeatureCollectionStruct::operator=(const OsduAbstractFeatureCollectionStruct& rhs)
{
    return *this = OsduAbstractFeatureCollectionStruct(rhs);
}

OsduAbstractFeatureCollectionStruct&
OsduAbstractFeatureCollectionStruct::operator=(OsduAbstractFeatureCollectionStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    type = std::move(rhs.type);
    features = std::move(rhs.features);
    bbox = std::move(rhs.bbox);

    return *this;
}

bool
OsduAbstractFeatureCollectionStruct::isValid() const
{
    return !type.empty() and featuresValid();
}


bool
OsduAbstractFeatureCollectionStruct::featuresValid() const
{
    return std::all_of(features.cbegin(), features.cend(), [](const OsduGeoJsonFeatureStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduAbstractFeatureCollectionStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduAbstractFeatureCollectionStruct::isValid() failed.");

    if (type.empty()) {
        result.emplace_back("OsduAbstractFeatureCollectionStruct:: type cannot be empty");
    }
    if (!featuresValid()) {
        result.emplace_back("OsduAbstractFeatureCollectionStruct::featuresValid() failed.");
        for (const auto& feature : features) {
            if (!feature.isValid()) {
                appendStringVector(result, feature.performAttributeValidityScan());
            }
        }
    }
    return result;
}

void
OsduAbstractFeatureCollectionStruct::propagatePartition(const std::string& partition)
{
    for (auto& feature : features) {
        feature.setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
