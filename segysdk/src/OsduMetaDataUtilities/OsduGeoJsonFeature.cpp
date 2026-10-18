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

#include "../OsduMetaDataUtilities/OsduGeoJsonFeature.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureProperties.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElement.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureLiterals.h"

    namespace segysdk::metadata::osdu
    {

    OsduGeoJsonFeature::OsduGeoJsonFeature(const std::string& osduPartition)
      : OsduMetadataBase(osduPartition), geoJsonFeatureStruct_(OsduGeoJsonFeatureStruct(osduPartition))
    {
        /* Empty */
    }

    OsduGeoJsonFeature::OsduGeoJsonFeature(const OsduGeoJsonFeature& other)
      : OsduMetadataBase(other.getOsduPartition()), geoJsonFeatureStruct_(other.geoJsonFeatureStruct_)
    {
        setJson(other.getJson());
        setIsArrayElement(other.getIsArrayElement());
    }

    OsduGeoJsonFeature::OsduGeoJsonFeature(OsduGeoJsonFeature&& other) noexcept
      : OsduMetadataBase(other.getOsduPartition()), geoJsonFeatureStruct_(std::move(other.geoJsonFeatureStruct_))
    {
        setJson(other.getJson());
        setIsArrayElement(other.getIsArrayElement());
    }

    OsduGeoJsonFeature&
    OsduGeoJsonFeature::operator=(const OsduGeoJsonFeature& rhs)
    {
        return *this = OsduGeoJsonFeature(rhs);
    }

    OsduGeoJsonFeature&
    OsduGeoJsonFeature::operator=(OsduGeoJsonFeature&& rhs) noexcept
    {
        setOsduPartition(rhs.getOsduPartition());
        setIsArrayElement(rhs.getIsArrayElement());
        setJson(rhs.getAsJson());

        geoJsonFeatureStruct_ = std::move(rhs.geoJsonFeatureStruct_);

        return *this;
    }

    OsduGeoJsonFeature::OsduGeoJsonFeature(OsduGeoJsonFeatureStruct geoJsonFeatureStruct)
      : OsduMetadataBase(geoJsonFeatureStruct.getPartition()), geoJsonFeatureStruct_(std::move(geoJsonFeatureStruct))
    {
        populateJsonObject();
    }

    OsduGeoJsonFeature::OsduGeoJsonFeature(Json::Value json, const std::string& osduPartition)
      : OsduMetadataBase(std::move(json), osduPartition), geoJsonFeatureStruct_(OsduGeoJsonFeatureStruct(osduPartition))
    {
        populateMetadataStruct();
    }

    bool
    OsduGeoJsonFeature::isValidInput() const
    {
        return geoJsonFeatureStruct_.isValid();
    }

    void
    OsduGeoJsonFeature::initializeJson()
    {
        removeMember(OsduGeoJsonFeatureLiterals::type);
        removeMember(OsduGeoJsonFeatureLiterals::geometry);
        removeMember(OsduGeoJsonFeatureLiterals::geometries);
        removeMember(OsduGeoJsonFeatureLiterals::bbox);
        removeMember(OsduGeoJsonFeatureLiterals::properties);
    }

    void
    OsduGeoJsonFeature::populateJsonObject()
    {
        clearJsonObject();

        setParameter(OsduGeoJsonFeatureLiterals::type, geoJsonFeatureStruct_.type);

        OsduGeoJsonFeatureProperties properties(geoJsonFeatureStruct_.properties);
        setParameter(OsduGeoJsonFeatureLiterals::properties, properties.getAsJson());

        if (geoJsonFeatureStruct_.geometryStruct.has_value()) {
            OsduGeoJsonGeometryElement geometry(geoJsonFeatureStruct_.geometryStruct.value());
            setParameter(OsduGeoJsonFeatureLiterals::geometry, geometry.getAsJson());
        }
        if (geoJsonFeatureStruct_.geometryStructVector.has_value()) {
            InitializeJsonArray(OsduGeoJsonFeatureLiterals::geometries);
            for (const auto& geometryStruct : geoJsonFeatureStruct_.geometryStructVector.value()) {
                OsduGeoJsonGeometryElement geometry(geometryStruct);
                appendArrayElement(OsduGeoJsonFeatureLiterals::geometries, geometry.getAsJson());
            }
        }
        if (geoJsonFeatureStruct_.bbox.has_value()) {
            InitializeJsonArray(OsduGeoJsonFeatureLiterals::bbox);
            for (const auto& value : geoJsonFeatureStruct_.bbox.value()) {
                appendArrayElement(OsduGeoJsonFeatureLiterals::bbox, value);
            }
        }
    }

    void
    OsduGeoJsonFeature::initializeMetadataStruct()
    {
        geoJsonFeatureStruct_ = OsduGeoJsonFeatureStruct(getOsduPartition());
    }

    std::shared_ptr<OsduGeoJsonFeaturePropertiesStruct>
    OsduGeoJsonFeature::getGeoJsonFeaturePropertiesStructPtr(const Json::Value& json) const
    {
        OsduGeoJsonFeatureProperties properties(json, getOsduPartition());
        return std::dynamic_pointer_cast<OsduGeoJsonFeaturePropertiesStruct>(properties.getMetaDataStruct());
    }

    std::shared_ptr<OsduGeoJsonFeatureStruct>
    OsduGeoJsonFeature::getGeoJsonFeatureStructPtr(const Json::Value& json) const
    {
        OsduGeoJsonFeature feature(json, getOsduPartition());
        return std::dynamic_pointer_cast<OsduGeoJsonFeatureStruct>(feature.getMetaDataStruct());
    }

    std::shared_ptr<OsduGeoJsonGeometryElementStruct>
    OsduGeoJsonFeature::getGeoJsonGeometryElementStructPtr(const Json::Value& json) const
    {
        OsduGeoJsonGeometryElement geometry(json, getOsduPartition());
        return std::dynamic_pointer_cast<OsduGeoJsonGeometryElementStruct>(geometry.getMetaDataStruct());
    }

    void
    OsduGeoJsonFeature::populateMetadataStruct()
    {
        initializeMetadataStruct();
        const auto members = getJsonMemberNames();
        geoJsonFeatureStruct_.type = getParameter<std::string>(OsduGeoJsonFeatureLiterals::type);

        if (isMember(OsduGeoJsonFeatureLiterals::geometry)) {
            auto json = getParameter<Json::Value>(OsduGeoJsonFeatureLiterals::geometry);
            const auto ptr = getGeoJsonGeometryElementStructPtr(json);
            if (ptr != nullptr) {
                geoJsonFeatureStruct_.geometryStruct = *ptr;
            }
        }
        if (isMember(OsduGeoJsonFeatureLiterals::geometries)) {
            const auto jsonArray = getAllArrayElements<Json::Value>(OsduGeoJsonFeatureLiterals::geometries);
            std::vector<OsduGeoJsonGeometryElementStruct> result;
            for (const Json::Value& json : jsonArray) {
                const auto ptr = getGeoJsonGeometryElementStructPtr(json);
                if (ptr != nullptr) {
                    result.emplace_back(*ptr);
                }
            }
            geoJsonFeatureStruct_.geometryStructVector = result;
        }
        if (isMember(OsduGeoJsonFeatureLiterals::properties)) {
            auto json = getParameter<Json::Value>(OsduGeoJsonFeatureLiterals::properties);
            auto ptr = getGeoJsonFeaturePropertiesStructPtr(json);
            if (ptr != nullptr) {
                geoJsonFeatureStruct_.properties = *ptr;
            }
        }

        if (isMember(OsduGeoJsonFeatureLiterals::bbox)) {
            const auto values = getAllArrayElements<double>(OsduGeoJsonFeatureLiterals::bbox);
            geoJsonFeatureStruct_.bbox = values;
        }
    }

    std::shared_ptr<OsduMetadataStructBase>
    OsduGeoJsonFeature::getMetaDataStruct()
    {
        populateMetadataStruct();
        return std::make_shared<OsduGeoJsonFeatureStruct>(geoJsonFeatureStruct_);
    }

    } // namespace segysdk::metadata::osdu
