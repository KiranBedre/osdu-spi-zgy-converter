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

#include "tools/SaveThumbnails/Drive.h"

#include "FileIO/GenericFileManager.h"
#include "Utilities/TokenSourceFactory.h"
#include "tools/Keywords/Keywords.h"

namespace tool::thumbnail
{

BitmapSamplesCollection
Drive::readSamples(const Params& params) const
{
    return doReadSamples(params);
}

std::string
Drive::writeFile(const Params& params, std::string_view fileNameSuffix, const ByteBuffer& buffer) const
{
    constexpr auto cacheBlockSize = std::size_t{1024 * 512};
    auto driveFileName = std::string{params.fileSource_}.append(fileNameSuffix);
    using namespace segysdk;
    fileio::GenericFileManager::createWriter(
        fileio::FileReference{
            driveFileName, fileio::FileReaderOptions{
                               SDCredentials{
                                   params.sdUrl_, keywords::unused, utilities::TokenSourceFactory::getTokenSource()},
                               CacheSizeOption{cacheBlockSize, 4, 1, cacheBlockSize},
                               params.legalTag_}},
        fileio::WriteMode::Truncate)
        ->write(buffer.data(), buffer.size());
    return driveFileName;
}

Drive::~Drive()
{
    segysdk::fileio::GenericFileManager::reset();
}

} // namespace tool::thumbnail
