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

#include "tools/Context/Context.h"

#include "tools/Error/Error.h"
#include "tools/Keywords/Keywords.h"

namespace tool
{

static Error
makeRecordError(std::string_view message, const Json::Value& record)
{
    return Error{std::string{message}.append(", record: ").append(record.toStyledString())};
}

static std::string
getFileSource(const Json::Value& record)
{
    for (const auto& info : record[keywords::data][keywords::datasetProperties][keywords::fileSourceInfos]) {
        if (info.isMember(keywords::fileSource)) {
            return info[keywords::fileSource].asString();
        }
    }
    throw makeRecordError("no file source", record);
}

static std::string
getLegalTag(const Json::Value& record)
{
    if (const auto result = record[keywords::legal][keywords::legaltags][0]; !result.empty()) {
        return result.asString();
    }
    throw makeRecordError("no legal tag", record);
}

FileCollectionDescriptor::FileCollectionDescriptor(const std::string& id, const Json::Value& record)
  : RecordDescriptor{id},
    fileSource_{getFileSource(record)},
    legalTag_{getLegalTag(record)}
{
}

SeismicTraceDataDescriptor::SeismicTraceDataDescriptor(const std::string& id)
  : RecordDescriptor{id}
{
}

SeismicTraceDataDescriptor&
SeismicTraceDataDescriptor::withFileCollectionDescriptor(FileCollectionDescriptor&& value)
{
    fileCollectionDescriptor_ = std::move(value);
    return *this;
}

SeismicTraceDataDescriptor&
SeismicTraceDataDescriptor::withSeismicLineGeometryId(std::string_view value)
{
    seismicLineGeometryId_ = value;
    return *this;
}

RecordDescriptor::RecordDescriptor(const std::string& id)
  : id_{id}
{
}

} // namespace tool
