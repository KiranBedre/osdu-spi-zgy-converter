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

#include "tools/Params/Params.h"
#include "tools/SaveThumbnails/Drive.h"
#include "tools/SaveThumbnails/DriveConnector.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace tool::thumbnail::live
{

class DriveConnector : public thumbnail::DriveConnector
{
private:
    using DrivePtr = std::unique_ptr<Drive>;
    mutable std::unordered_map<std::string, DrivePtr> drives_;
    Drive& getDrive(const Params& params) const;
    BitmapSamplesCollection doReadSamples(const Params& params) const override;
    std::string doWriteFile(const Params& params, const std::string& fileNameSuffix, const ByteBuffer& buffer) const override;
};

} // namespace tool::thumbnail::live
