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

#pragma once

#include "../OsduMetaDataUtilities/OsduAbstractDatasetStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"
#include "../OsduMetaDataUtilities/OsduContentSeismicHistogramStruct.h"
#include "../OsduMetaDataUtilities/OsduMetaArrayStruct.h"
#include "../OsduMetaDataUtilities/OsduTagArrayStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>
#include <vector>

namespace segysdk::metadata::osdu
{
/*

"SchemaKind": "osd:wks:content--SeismicHistogram:1.0.0",
  "Name": "ST0202R08_PS_PSDM_RAW_PP_TIME.MIG_RAW.POST_Stack.3D.JS-017534",
  "Description": "Example histogram for time-migrated trace data.",
  "ExtensionProperties": {
    "Kind": "abc:xyz:CustomKind:0.0.0",
    "Custom": "Value"
  },
  "SeismicRecordID": "namespace:work-product-component--SeismicTraceData:ST0202R08-time-volume:",
  "RangeAmplitudeMin": -32076.25,
  "RangeAmplitudeMean": 0.000205,
  "RangeAmplitudeMax": 32022.96,
  "RangeAmplitudeRMS": 16035.45,
  "AmplitudeUnitOfMeasureID": "namespace:reference-data--UnitOfMeasure:Euc:",
  "TraceCountBinGrid": 232925,
  "TraceCountFileCollection": 231839,
  "TraceCountLive": 231762,
  "Histograms": [
    {
      "Name": "32 bin histogram",
      "Description": "Example Description",
      "BucketMin": -32076.25,
      "BucketMax": 32022.96,
      "BucketSize": 15.65,
      "BucketCount": 8,
      "Buckets": [
        0,
        1,
        0,
        0,
        0,
        3,
        3,
        2,
        5,
        3,
        8
      ]
    }
  ]
}
*/

class SEGYSDK_DLL_PUBLIC OsduContentSeismicHistogram final : public OsduMetadataBase
{
public:
    OsduContentSeismicHistogram(const std::string& osduPartition);
    OsduContentSeismicHistogram(const OsduContentSeismicHistogram& other);
    OsduContentSeismicHistogram(OsduContentSeismicHistogram&& other) noexcept;
    OsduContentSeismicHistogram& operator=(const OsduContentSeismicHistogram& rhs);
    OsduContentSeismicHistogram& operator=(OsduContentSeismicHistogram&& rhs) noexcept;
    ~OsduContentSeismicHistogram() override = default;

    explicit OsduContentSeismicHistogram(OsduContentSeismicHistogramStruct fileHistogramStruct);
    explicit OsduContentSeismicHistogram(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;


private:
    OsduContentSeismicHistogramStruct seismicHistogramStruct_;
};

}
