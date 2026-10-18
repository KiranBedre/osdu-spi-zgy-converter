/*
Copyright 2022 Schlumberger

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

#include "../SegyIndex/SegyIndexFactory.h"

#include "../SegyIndex/SegyIndexImpl.h"

namespace segysdk::api
{

SegyIndexFactory::SegyIndexFactory()
{
    /* EMPTY */
}

std::shared_ptr<segyindex::SegyIndex>
SegyIndexFactory::CreateSegyIndex(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                                  std::vector<std::string> const& segyFileNames,
                                  segysdk::fileio::FileReaderOptions const& fileReaderOptions,
                                  std::optional<std::string> const& indexBaseFileNameOverride) const
{
    return CreateSegyIndex(
        segyindex::Params{segyio::StorageAccessParams{}
                              .withSDCredentials(fileReaderOptions.sdCredentials)
                              .withCacheSizeOption(fileReaderOptions.cacheSize)}
            .withSegyFiles(segyFilesVector)
            .withSegyFileNames(segyFileNames)
            .withIndexBaseFileNameOverride(indexBaseFileNameOverride)
            .withLegalTag(fileReaderOptions.sdLegalTag));
}

std::shared_ptr<segyindex::SegyIndex>
SegyIndexFactory::CreateSegyIndex(const segyindex::Params& params) const
{
    return std::make_shared<segyindex::SegyIndexImpl>(params);
}

} // namespace segysdk::api
