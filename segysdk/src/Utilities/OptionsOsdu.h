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

#include "Options.h"
#include "StorageSvcClient.h"

#include "../Utilities/osdu/OsduFileCollectionSegy100.h"

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC OptionsOsdu final : public Options
{
private:
    class Args
    {
    public:
        Args(int argc, const char** argv, const segysdk::utilities::StorageSvcClient& storageSvcClient);

        std::string fileCollectionId() const;
        std::string seismicTraceDataId() const;
        std::string binGridId() const;

    private:
        static void
        help(const std::string& programName);

    private:
        std::string fileCollectionId_;
        std::string seismicTraceDataId_;
        std::string binGridId_;
    };

public:
    OptionsOsdu(int argc, const char** argv, const OptionsFileType oft);

    virtual std::vector<std::string> sourceFiles() const override;
    virtual std::string segyConfig() const override;
    virtual segysdk::utilities::BinGrid binGrid() const override;
    virtual std::string targetFile() const override;
    std::string getFileCollectionId() const;

    virtual void finalize() const override;

private:
    const segysdk::utilities::StorageSvcClient storageSvcClient_;
    Args args_;
    const std::string runId_;
    const segysdk::utilities::osdu::OsduFileCollectionSegy100 fileCollection_;
    const std::string binGridContent_;
};

} // namespace segysdk::utilities
