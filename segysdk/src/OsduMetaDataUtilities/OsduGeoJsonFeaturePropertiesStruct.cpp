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

#include <utility>

#include "../OsduMetaDataUtilities/OsduGeoJsonFeaturePropertiesStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduGeoJsonFeaturePropertiesStruct::OsduGeoJsonFeaturePropertiesStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesStruct::OsduGeoJsonFeaturePropertiesStruct(const OsduGeoJsonFeaturePropertiesStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    ijNumbers(other.ijNumbers),
    pointProperties(other.pointProperties),
    labelProperties(other.labelProperties),
    featureProperties(other.featureProperties),
    points2DProperties(other.points2DProperties)
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesStruct::OsduGeoJsonFeaturePropertiesStruct(OsduGeoJsonFeaturePropertiesStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    ijNumbers(std::move(other.ijNumbers)),
    pointProperties(std::move(other.pointProperties)),
    labelProperties(std::move(other.labelProperties)),
    featureProperties(std::move(other.featureProperties)),
    points2DProperties(std::move(other.points2DProperties))
{
    /* Empty */
}

OsduGeoJsonFeaturePropertiesStruct&
OsduGeoJsonFeaturePropertiesStruct::operator=(const OsduGeoJsonFeaturePropertiesStruct& rhs)
{
    return *this = OsduGeoJsonFeaturePropertiesStruct(rhs);
}

OsduGeoJsonFeaturePropertiesStruct&
OsduGeoJsonFeaturePropertiesStruct::operator=(OsduGeoJsonFeaturePropertiesStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    ijNumbers  = std::move(rhs.ijNumbers);
    pointProperties = std::move(rhs.pointProperties);
    labelProperties = std ::move(rhs.labelProperties);
    featureProperties = std ::move(rhs.featureProperties);
    points2DProperties = std::move(rhs.points2DProperties);

    return *this;
}

bool
OsduGeoJsonFeaturePropertiesStruct::ijNumbersValid() const
{
    if (!ijNumbers.has_value() || ijNumbers.value().empty()) {
        return true;
    }

    const size_t expected_elements = ijNumbers.value().at(0).size();

    return std::all_of(ijNumbers.value().cbegin(), ijNumbers.value().cend(), [expected_elements](const std::vector<int>& ij) { return ij.size() == expected_elements; });
}

bool
OsduGeoJsonFeaturePropertiesStruct::pointPropertiesValid() const
{
    return !pointProperties.has_value() or !pointProperties.value().empty();
}

bool
OsduGeoJsonFeaturePropertiesStruct::featurePropertyValid() const
{
    if (!featureProperties.has_value()) {
        return true;
    }

    return featureProperties.value().isValid();
}

bool
OsduGeoJsonFeaturePropertiesStruct::labelPropertiesValid() const
{
    if (!labelProperties.has_value()) {
        return true;
    }

    return std::all_of(labelProperties.value().cbegin(), labelProperties.value().cend(), [](OsduGeoJsonLabelPropertiesElementStruct x) { return x.isValid(); });
}
bool
OsduGeoJsonFeaturePropertiesStruct::points2dPropertiesValid() const
{
    if (!points2DProperties.has_value()) {
        return true;
    }

    return points2DProperties.value().isValid();
}

bool
OsduGeoJsonFeaturePropertiesStruct::isValid() const
{
    return ijNumbersValid() and pointPropertiesValid() and labelPropertiesValid() and featurePropertyValid() and points2dPropertiesValid();
}

std::vector<std::string>
OsduGeoJsonFeaturePropertiesStruct::performAttributeValidityScan() const
{
        if (isValid()) {
            return {};
        }

    std::vector<std::string> result;
    result.emplace_back("OsduGeoJsonFeaturePropertiesStruct::isValid() failed.");

    if (ijNumbers.has_value() and ijNumbers.value().empty()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: ijNumbers cannot be empty.");
    }
    else {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: ijNumbers must have constant size ij pairs.");
    }

    if (!pointPropertiesValid()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: pointProperties cannot be empty.");
    }

    if (!labelPropertiesValid()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: labelProperties is not valid.");
        for (const auto& label : labelProperties.value()) {
            if (!label.isValid()) {
                appendStringVector(result, label.performAttributeValidityScan());
            }
        }
    }
    if (!featurePropertyValid()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: FeatureProperties cannot be empty.");
    }
    if (!points2dPropertiesValid()) {
        result.emplace_back("OsduGeoJsonFeaturePropertiesStruct: points2dProperties are not valid.");
        appendStringVector(result, points2DProperties.value().performAttributeValidityScan());
    }
    return result;
}

} // namespace segysdk::metadata::osdu
