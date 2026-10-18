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

#include "tools/SaveThumbnails/Dispatcher.h"

#include "tools/SaveThumbnails/BitmapCreator.h"
#include "tools/SaveThumbnails/PngCreator.h"

#include <string_view>

namespace tool::thumbnail
{

Dispatcher::~Dispatcher() = default;

Dispatcher::Dispatcher(DriveConnectorPtr driveConnector, StorageConnectorPtr storageConnector)
  : drive_{std::move(driveConnector)}, storage_{std::move(storageConnector)}
{
}

static std::string
makeFileNameSuffix(const Bitmap& bitmap, std::string_view sliceType)
{
    return std::string{".thumbnail"}
        .append("-")
        .append(std::to_string(bitmap.width_))
        .append("x")
        .append(std::to_string(bitmap.height_))
        .append("-")
        .append(sliceType)
        .append(".png");
}

void
Dispatcher::saveThumbnails(const Params& params) const
{
    const auto [seismicTraceDataDescriptors] = storage_->getContext(params);
    for (const auto& seismicTraceDataDescriptor : seismicTraceDataDescriptors) {
        auto workingParams = Params{params}.withSeismicTraceDataId(seismicTraceDataDescriptor.id_);
        workingParams.withFileCollectionId(seismicTraceDataDescriptor.fileCollectionDescriptor_.id_);
        workingParams.withFileSource(seismicTraceDataDescriptor.fileCollectionDescriptor_.fileSource_);
        workingParams.withLegalTag(seismicTraceDataDescriptor.fileCollectionDescriptor_.legalTag_);
        auto artefactIds = segysdk::Strings{};
        for (const auto& bitmapSamples : drive_->readSamples(workingParams)) {
            workingParams.withSliceType(bitmapSamples.sliceType_);
            const auto& bitmap = createBitmap(bitmapSamples);
            const auto& png = createPng(bitmap);
            artefactIds.push_back(
                storage_->addFileImagePngRecord(
                    workingParams,
                    drive_->writeFile(
                        workingParams, makeFileNameSuffix(bitmap, bitmapSamples.sliceType_), png),
                    png.size()));
        }
        storage_->addFileImagePngArtefacts(workingParams, artefactIds);
    }
}

} // namespace tool::thumbnail
