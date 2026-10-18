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

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonFeature.h"

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonFeatureLiterals.h"
#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonGeometryElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureProperties.h"

namespace segysdk::metadata::osdu
    {

    OsduAnyCrsGeoJsonFeature::OsduAnyCrsGeoJsonFeature(const std::string& osduPartition)
      : OsduMetadataBase(osduPartition), geoAnyCrsJsonFeatureStruct_(OsduAnyCrsGeoJsonFeatureStruct(osduPartition))
    {
        /* Empty */
    }

    OsduAnyCrsGeoJsonFeature::OsduAnyCrsGeoJsonFeature(const OsduAnyCrsGeoJsonFeature& other)
      : OsduMetadataBase(other.getOsduPartition()), geoAnyCrsJsonFeatureStruct_(other.geoAnyCrsJsonFeatureStruct_)
    {
        setJson(other.getJson());
        setIsArrayElement(other.getIsArrayElement());
    }

    OsduAnyCrsGeoJsonFeature::OsduAnyCrsGeoJsonFeature(OsduAnyCrsGeoJsonFeature&& other) noexcept
      : OsduMetadataBase(other.getOsduPartition()), geoAnyCrsJsonFeatureStruct_(std::move(other.geoAnyCrsJsonFeatureStruct_))
    {
        setJson(other.getJson());
        setIsArrayElement(other.getIsArrayElement());
    }

    OsduAnyCrsGeoJsonFeature&
    OsduAnyCrsGeoJsonFeature::operator=(const OsduAnyCrsGeoJsonFeature& rhs)
    {
        return *this = OsduAnyCrsGeoJsonFeature(rhs);
    }

    OsduAnyCrsGeoJsonFeature&
    OsduAnyCrsGeoJsonFeature::operator=(OsduAnyCrsGeoJsonFeature&& rhs) noexcept
    {
        setOsduPartition(rhs.getOsduPartition());
        setIsArrayElement(rhs.getIsArrayElement());
        setJson(rhs.getAsJson());

        geoAnyCrsJsonFeatureStruct_ = std::move(rhs.geoAnyCrsJsonFeatureStruct_);

        return *this;
    }

    OsduAnyCrsGeoJsonFeature::OsduAnyCrsGeoJsonFeature(OsduAnyCrsGeoJsonFeatureStruct geoJsonFeatureStruct)
      : OsduMetadataBase(geoJsonFeatureStruct.getPartition()), geoAnyCrsJsonFeatureStruct_(std::move(geoJsonFeatureStruct))
    {
        populateJsonObject();
    }

    OsduAnyCrsGeoJsonFeature::OsduAnyCrsGeoJsonFeature(Json::Value json, const std::string& osduPartition)
      : OsduMetadataBase(std::move(json), osduPartition), geoAnyCrsJsonFeatureStruct_(OsduAnyCrsGeoJsonFeatureStruct(osduPartition))
    {
        populateMetadataStruct();
    }

    bool
    OsduAnyCrsGeoJsonFeature::isValidInput() const
    {
        return geoAnyCrsJsonFeatureStruct_.isValid();
    }

    void
    OsduAnyCrsGeoJsonFeature::initializeJson()
    {
        removeMember(OsduAnyCrsGeoJsonFeatureLiterals::type);
        removeMember(OsduAnyCrsGeoJsonFeatureLiterals::geometry);
        removeMember(OsduAnyCrsGeoJsonFeatureLiterals::bbox);
        removeMember(OsduAnyCrsGeoJsonFeatureLiterals::properties);
    }

    void
    OsduAnyCrsGeoJsonFeature::populateJsonObject()
    {
        clearJsonObject();

        setParameter(OsduAnyCrsGeoJsonFeatureLiterals::type, geoAnyCrsJsonFeatureStruct_.type);

        const OsduGeoJsonFeatureProperties properties(geoAnyCrsJsonFeatureStruct_.properties);
        setParameter(OsduAnyCrsGeoJsonFeatureLiterals::properties, properties.getAsJson());

        if (geoAnyCrsJsonFeatureStruct_.geometry.has_value()) {
            const OsduAnyCrsGeoJsonGeometryElement geometry(geoAnyCrsJsonFeatureStruct_.geometry.value());
            setParameter(OsduAnyCrsGeoJsonFeatureLiterals::geometry, geometry.getAsJson());
        }

        if (geoAnyCrsJsonFeatureStruct_.geometries.has_value()) {
            InitializeJsonArray(OsduAnyCrsGeoJsonFeatureLiterals::geometries);
            for (auto& myGeometry : geoAnyCrsJsonFeatureStruct_.geometries.value()) {
                const OsduAnyCrsGeoJsonGeometryElement geometry(myGeometry);
                appendArrayElement(OsduAnyCrsGeoJsonFeatureLiterals::geometries, geometry.getAsJson());
            }
        }

        if (geoAnyCrsJsonFeatureStruct_.bbox.has_value()) {
            InitializeJsonArray(OsduAnyCrsGeoJsonFeatureLiterals::bbox);
            for (const auto& value : geoAnyCrsJsonFeatureStruct_.bbox.value()) {
                appendArrayElement(OsduAnyCrsGeoJsonFeatureLiterals::bbox, value);
            }
        }
    }

    void
    OsduAnyCrsGeoJsonFeature::initializeMetadataStruct()
    {
        geoAnyCrsJsonFeatureStruct_ = OsduAnyCrsGeoJsonFeatureStruct(getOsduPartition());
    }

    void
    OsduAnyCrsGeoJsonFeature::populateMetadataStruct()
    {
        initializeMetadataStruct();

        geoAnyCrsJsonFeatureStruct_.type = getParameter<std::string>(OsduAnyCrsGeoJsonFeatureLiterals::type);

        OsduGeoJsonFeatureProperties properties(getParameter<Json::Value>(OsduAnyCrsGeoJsonFeatureLiterals::properties), getOsduPartition());
        const auto propertiesPtr = std::reinterpret_pointer_cast<OsduGeoJsonFeaturePropertiesStruct>(properties.getMetaDataStruct());
        if (propertiesPtr != nullptr) {
            geoAnyCrsJsonFeatureStruct_.properties = *propertiesPtr;
        }

        if (isMember(OsduAnyCrsGeoJsonFeatureLiterals::geometry)) {
            OsduAnyCrsGeoJsonGeometryElement geometry(getParameter<Json::Value>(OsduAnyCrsGeoJsonFeatureLiterals::geometry), getOsduPartition());
            const auto geometryPtr = std::reinterpret_pointer_cast<OsduAnyCrsGeoJsonGeometryElementStruct>(geometry.getMetaDataStruct());
            if (geometryPtr != nullptr) {
                geoAnyCrsJsonFeatureStruct_.geometry = *geometryPtr;
            }
        }
        if (isMember(OsduAnyCrsGeoJsonFeatureLiterals::geometries)) {
            const auto geometriesJson = getAllArrayElements<Json::Value>(OsduAnyCrsGeoJsonFeatureLiterals::geometries);
            std::vector<OsduAnyCrsGeoJsonGeometryElementStruct> result;
            for (const auto& geoJson : geometriesJson) {
                OsduAnyCrsGeoJsonGeometryElement geometry(geoJson, getOsduPartition());
                const auto geometryPtr = std::reinterpret_pointer_cast<OsduAnyCrsGeoJsonGeometryElementStruct>(geometry.getMetaDataStruct());
                if (geometryPtr != nullptr) {
                    result.emplace_back(*geometryPtr);
                }
            }
            geoAnyCrsJsonFeatureStruct_.geometries = result;
        }

        if (isMember(OsduAnyCrsGeoJsonFeatureLiterals::bbox)) {
            const auto values = getAllArrayElements<double>(OsduAnyCrsGeoJsonFeatureLiterals::bbox);
            geoAnyCrsJsonFeatureStruct_.bbox = values;
        }
    }

    std::shared_ptr<OsduMetadataStructBase>
    OsduAnyCrsGeoJsonFeature::getMetaDataStruct()
    {
        populateMetadataStruct();
        return std::make_shared<OsduAnyCrsGeoJsonFeatureStruct>(geoAnyCrsJsonFeatureStruct_);
    }

} // namespace segysdk::metadata::osdu
