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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../Utilities/BinGrid.h"
#include "../Utilities/Options.h"

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC OptionsDelfiCommandLine final : public Options
{
public:
    OptionsDelfiCommandLine(int argc, const char** argv, OptionsFileType oft);

    std::vector<std::string> sourceFiles() const override;
    std::string segyConfig() const override;
    segysdk::utilities::BinGrid binGrid() const override;
    std::string targetFile() const override;
    void finalize() const override;

private:
    std::string sourceFile_;
    std::string segyConfig_;
    std::string binGridFile_;
    std::string targetFile_;

    static void help(const std::string& programName);
};

} // namespace segysdk::utilities
