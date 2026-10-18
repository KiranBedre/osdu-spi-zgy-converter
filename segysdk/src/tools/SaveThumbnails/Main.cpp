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

#include "Logger/Logger.h"
#include "tools/Logger/InitializeLogger.h"
#include "tools/Options/Options.h"
#include "tools/SaveThumbnails/Dispatcher.h"
#include "tools/SaveThumbnails/DriveConnectorLive.h"
#include "tools/StorageClient/StorageClientLive.h"
#include "tools/StorageConnector/StorageConnector.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int
main(const int argc, char* argv[])
{
    auto params = tool::Params{};
    const auto options = tool::ProgramOptions{argc, argv};
    try {
        params = options.getBasicParams();
        initializeLogger(params);
        tool::thumbnail::Dispatcher{
            std::make_unique<tool::thumbnail::live::DriveConnector>(),
            std::make_unique<tool::StorageConnector>(std::make_unique<tool::live::StorageClient>())}
            .saveThumbnails(params);
    }
    catch (const std::exception& error) {
        auto message = std::stringstream{};
        message << "failed to save thumbnails: "
                << error.what()
                << '\n'
                << options.getDescription()
                << params.toString();
        std::cerr << message.str() << std::endl;
        SEGYLOG_REPORT_ERROR(message.str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
