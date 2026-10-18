/*
Copyright 2021-2022 Schlumberger

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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../Utilities/auth/HttpCredentialProvider.h"

#include <json/json.h>

#include <memory>
#include <string>
#include <cpprest/http_msg.h>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC CrsConverterSvcClient
{
public:
    explicit CrsConverterSvcClient(std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider> credentialProvider);

    static std::string getToConvertJsonBody(const Json::Value& asIngested);

    std::string convertRecord(const Json::Value& geoJson) const;

private:
    static void addHeaders(web::http::http_request& request, std::vector<segysdk::utilities::auth::HttpCredentialProvider::Header>& additional_headers, std::string& data_partition_id);

private:
    std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider> credentialProvider_;
    inline static const std::string wgs84crs{"{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"4326\"},\"name\":\"GCS_WGS_1984\",\"type\":\"LBC\",\"ver\":\"PE_10_3_1\",\"wkt\":\"GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433],AUTHORITY[\\\"EPSG\\\",4326]]\"}"};
};

} // namespace SegyIndexing::Segy2D
