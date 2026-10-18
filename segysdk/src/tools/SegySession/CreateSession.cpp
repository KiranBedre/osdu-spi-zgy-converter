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

#include "tools/SegySession/CreateSession.h"

#include "Utilities/JsonUtilities.h"
#include "Utilities/TokenSourceFactory.h"
#include "segysdk/SegySessionFactory.h"
#include "tools/Error/Error.h"
#include "tools/Keywords/Keywords.h"

namespace tool
{

SegySessionPtr
createSessionWithIndex(const Params& params)
{
    auto result = createSession(params);
    if (!result->getIndex(segysdk::request::Params{}.withCorrelationId(params.correlationId_))->hasIndexFile()) {
        throw Error{std::string{"SEG-Y session index has no index file"}.append(params.toString())};
    }
    return result;
}

SegySessionPtr
createSession(const Params& params)
{
    using namespace segysdk::utilities;
    const auto& fileCollectionRecord = JsonUtilities::getJsonObjectFromPathOrOsduId(params.fileCollectionId_).toStyledString();
    constexpr auto cacheBlockSize = std::size_t{1024 * 1024 * 2};
    const auto driveParams = segysdk::segyio::StorageAccessParams{segysdk::request::Params{}.withCorrelationId(params.correlationId_)}
                                 .withSDCredentials({params.sdUrl_, keywords::unused, TokenSourceFactory::getTokenSource()})
                                 .withCacheSizeOption({cacheBlockSize, 4, 1, cacheBlockSize});
    auto result = segysdk::api::SegySessionFactory{fileCollectionRecord}.CreateSegySession(driveParams);
    if (result->getIndex(driveParams) == nullptr) {
        throw Error{std::string{"failed to get SEG-Y session index"}.append(params.toString())};
    }
    return result;
}

} // namespace tool
