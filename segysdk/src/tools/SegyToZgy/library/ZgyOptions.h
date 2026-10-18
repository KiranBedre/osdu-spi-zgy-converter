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

#include "../FileIO/SDCredentials.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/BrickSize.h"
#include "../FileIO/FileReference.h"

#include "../FileIO/GenericFileManager.h"

#include "../Utilities/Options.h"
#include "../Utilities/ZgyOutputParams.h"

#include <SDAPI/SDManager.h>
#include <json/json.h>
#include <openzgy/api.h>
#include <openzgy/iocontext.h>

#include <memory>
#include <string>
#include <vector>

class ZgyOptions : public segysdk::utilities::Options
{
public:
    static std::shared_ptr<ZgyOptions> parse(int argc, const char** argv, const segysdk::utilities::OptionsFileType oft);
    static void showUsageInfo(int argc, const char** argv);
    static void dumpParameterTemplateJson();
    static void dumpJsonSampleToConsoleOutput();
    static bool isLocalFile(const std::string& fileName);

    virtual ~ZgyOptions();

    OpenZGY::ZgyWriterArgs zgyWriterArgs() const;

protected:
    explicit ZgyOptions(const segysdk::utilities::OptionsFileType oft);
    ZgyOptions();
    const OpenZGY::IOContext* createOpenZGYIoContext() const;
    static segysdk::fileio::FileReaderOptions getFileReaderOptions(const std::string& fileName, const std::string& legalTag);
    static segysdk::fileio::FileReference getFileReference(const std::string& fileName);
    static std::string storeLocalRecord(const std::string& fileName, const std::string& content);
};
