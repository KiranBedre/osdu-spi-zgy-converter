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

#include "tools/SaveThumbnails/DriveConnectorLive.h"

#include "tools/FileCollection/FileCollection.h"
#include "tools/SaveThumbnails/DriveSegy.h"
#include "tools/SaveThumbnails/DriveZgy.h"

namespace tool::thumbnail::live
{

BitmapSamplesCollection
DriveConnector::doReadSamples(const Params& params) const
{
    return getDrive(params).readSamples(params);
}

std::string
DriveConnector::doWriteFile(const Params& params, const std::string& fileNameSuffix, const ByteBuffer& buffer) const
{
    return getDrive(params).writeFile(params, fileNameSuffix, buffer);
}

Drive&
DriveConnector::getDrive(const Params& params) const
{
    auto it = drives_.find(params.fileCollectionId_);
    if (it == drives_.end()) {
        it = drives_.try_emplace(
                        params.fileCollectionId_,
                        isFileCollectionSlbOpenZGY(params.fileCollectionId_)
                            ? DrivePtr{std::make_unique<zgy::Drive>()}
                            : DrivePtr{std::make_unique<segy::Drive>(params)})
                 .first;
    }
    return *(it->second);
}

} // namespace tool::thumbnail::live
