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

#include "tools/Params/Params.h"

namespace tool
{

std::string
Params::toString() const
{
    return std::string{"\nparameters provided:"}
        .append("\n  config file: '")
        .append(configFile_)
        .append("'\n  seismic trace data id: '")
        .append(seismicTraceDataId_)
        .append("'\n  work product id: '")
        .append(workProductId_)
        .append("'\n  sd url: '")
        .append(sdUrl_)
        .append("'\n  correlation id: '")
        .append(correlationId_)
        .append("'\nparameters calculated:")
        .append("\n  file collection id: '")
        .append(fileCollectionId_)
        .append("'\n  slice type: '")
        .append(sliceType_)
        .append("'");
}

Params&
Params::withConfigFile(std::string_view value)
{
    configFile_ = value;
    return *this;
}

Params&
Params::withSeismicTraceDataId(std::string_view value)
{
    seismicTraceDataId_ = value;
    return *this;
}

Params&
Params::withWorkProductId(std::string_view value)
{
    workProductId_ = value;
    return *this;
}

Params&
Params::withSdUrl(std::string_view value)
{
    sdUrl_ = value;
    return *this;
}

Params&
Params::withVerbosity(std::string_view value)
{
    verbosity_ = value;
    return *this;
}

Params&
Params::withCorrelationId(std::string_view value)
{
    correlationId_ = value;
    return *this;
}

Params&
Params::withFileCollectionId(std::string_view value)
{
    fileCollectionId_ = value;
    return *this;
}

Params&
Params::withFileSource(std::string_view value)
{
    fileSource_ = value;
    return *this;
}

Params&
Params::withSliceType(std::string_view value)
{
    sliceType_ = value;
    return *this;
}

Params&
Params::withLegalTag(std::string_view value)
{
    legalTag_ = value;
    return *this;
}

Params&
Params::withSeismicLineGeometryId(std::string_view value)
{
    seismicLineGeometryId_ = value;
    return *this;
}

} // namespace tool
