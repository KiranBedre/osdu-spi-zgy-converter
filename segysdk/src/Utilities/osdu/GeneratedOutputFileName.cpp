/*
Copyright 2021-2022 Schlumberger

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

#include "GeneratedOutputFileName.h"

#include "../Utilities/StringUtils.h"

#include <filesystem>
#include <string>

namespace segysdk::utilities::osdu
{

static std::string
removeSgyExtension(const std::string& segyFileName)
{
    auto ext = StringUtils::ToUppercase(std::filesystem::path(segyFileName).extension().string());
    
    if (!ext.empty() && (ext == ".SGY" || ext == ".SEGY")){
        std::string newName = segyFileName;
        return newName.replace(segyFileName.size() - ext.size(), segyFileName.size(), "");
    }

    return segyFileName;
}

GeneratedOutputFileName::GeneratedOutputFileName(const std::string& inputFileName, const std::string& customSuffix)
  : fileName(removeSgyExtension(inputFileName)
             + (customSuffix.empty() ? "" : "." + customSuffix)
             + ".zgy")
{
}

} // namespace segysdk::utilities::osdu
