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

#pragma once

#include <string>
#include <string_view>

namespace tool
{

struct Params
{
    std::string configFile_;
    std::string seismicTraceDataId_;
    std::string workProductId_;
    std::string sdUrl_;
    std::string verbosity_;
    std::string correlationId_;
    std::string fileCollectionId_;
    std::string fileSource_;
    std::string sliceType_;
    std::string legalTag_;
    std::string seismicLineGeometryId_;

    std::string toString() const;
    Params& withConfigFile(std::string_view value);
    Params& withSeismicTraceDataId(std::string_view value);
    Params& withWorkProductId(std::string_view value);
    Params& withSdUrl(std::string_view value);
    Params& withVerbosity(std::string_view value);
    Params& withCorrelationId(std::string_view value);
    Params& withFileCollectionId(std::string_view value);
    Params& withFileSource(std::string_view value);
    Params& withSliceType(std::string_view value);
    Params& withLegalTag(std::string_view value);
    Params& withSeismicLineGeometryId(std::string_view value);
};

} // namespace tool
