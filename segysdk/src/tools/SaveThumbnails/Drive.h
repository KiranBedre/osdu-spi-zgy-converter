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
#include "tools/SaveThumbnails/Types.h"

namespace tool::thumbnail
{

class Drive
{
public:
    virtual ~Drive();
    BitmapSamplesCollection readSamples(const Params& params) const;
    std::string writeFile(const Params& params, std::string_view fileNameSuffix, const ByteBuffer& buffer) const;

private:
    virtual BitmapSamplesCollection doReadSamples(const Params& params) const = 0;
};

} // namespace tool::thumbnail
