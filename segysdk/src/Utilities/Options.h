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

#include "../FileIO/SDCredentials.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/BrickSize.h"
#include "../Utilities/ZgyOutputParams.h"

#include <SDAPI/SDManager.h>
#include <json/json.h>
#include <openzgy/api.h>
#include <openzgy/iocontext.h>

#include <memory>
#include <string>
#include <vector>

namespace segysdk::utilities
{

enum class SEGYSDK_DLL_PUBLIC OptionsFileType
{
    OpenZGY,
    SEGY,
    Uninitialized
};

class SEGYSDK_DLL_PUBLIC Options
{
public:
    virtual ~Options();

    static std::shared_ptr<Options> parse(int argc, const char** argv, const OptionsFileType oft);

    OptionsFileType optionsFileType() const;
    virtual std::vector<std::string> sourceFiles() const = 0;
    virtual std::string segyConfig() const = 0;
    virtual BinGrid binGrid() const = 0;
    virtual std::string targetFile() const = 0;
    std::unique_ptr<seismicdrive::SDManager> createSDManager() const;
    virtual void finalize() const = 0;
    static BrickSize brickSize();
    SDCredentials const& sdCredentials() const;
    bool isOsdu() const;
    void setIsOsdu(bool isOsdu);
    static ZgyOutputParams getOutputParamsFromJson(const std::string& json_file);

protected:
    explicit Options(const OptionsFileType oft);
    Options();

    static std::string sdAuthCallback(const void* data);
    static OpenZGY::SampleDataType getSampleSize(int value);
    static std::string getBrickSize(Json::Value brick);

private:
    std::unique_ptr<auth::TokenSource> token_source_;
    const SDCredentials sdCredentials_;
    bool isOsdu_ = false;
    const OptionsFileType oft_;
};

} // namespace segysdk::utilities
