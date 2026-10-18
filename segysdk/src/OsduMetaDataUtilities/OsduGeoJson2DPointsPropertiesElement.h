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

#pragma once

#include "../OsduMetaDataUtilities/OsduGeoJsonFeatureElementStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"
#include "../OsduMetaDataUtilities/OsduGeoJson2DPointsPropertiesElementStruct.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*

"properties": {
							"PointProperties": [
								{
									"Inline": 1001,
									"Crossline": 1
								},
								{
									"Inline": 1360,
									"Crossline": 1
								},
								{
									"Inline": 1360,
									"Crossline": -225
								},
								{
									"Inline": 1001,
									"Crossline": -225
								},
								{
									"Inline": 1001,
									"Crossline": 1
								}
							],
							"Kind": "osdu:wks:AbstractGeoJson.PropertiesBinGridCorners:1.0.0"
						}

*/

class SEGYSDK_DLL_PUBLIC OsduGeoJson2DPointsPropertiesElement final : public OsduMetadataBase
{
public:
    OsduGeoJson2DPointsPropertiesElement(const std::string& osduPartition);
    OsduGeoJson2DPointsPropertiesElement(const OsduGeoJson2DPointsPropertiesElement& other);
    OsduGeoJson2DPointsPropertiesElement(OsduGeoJson2DPointsPropertiesElement&& other) noexcept;
    OsduGeoJson2DPointsPropertiesElement& operator=(const OsduGeoJson2DPointsPropertiesElement& rhs);
    OsduGeoJson2DPointsPropertiesElement& operator=(OsduGeoJson2DPointsPropertiesElement&& rhs) noexcept;
    ~OsduGeoJson2DPointsPropertiesElement() override = default;
    OsduGeoJson2DPointsPropertiesElement(OsduGeoJson2DPointsPropertiesElementStruct constantValueOverrideStruct);
    explicit OsduGeoJson2DPointsPropertiesElement(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduGeoJson2DPointsPropertiesElementStruct points2DPropertyStruct_;

};

}
