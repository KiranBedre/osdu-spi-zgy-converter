/*
Copyright 2023 Schlumberger

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

#include "ApiEnvironment.h"

#include <Utilities/InputParser.h>

namespace segysdk::api::tests
{

std::shared_ptr<ApiEnvironment> ApiEnvironment::instance_ = nullptr;

ApiEnvironment::ApiEnvironment(int argc, char** argv)
{
    parse(argc, argv);
}

void
ApiEnvironment::parse(int argc, char** argv)
{
    segysdk::utilities::InputParser input(argc, argv);

    keepOutputFiles_ = input.cmdOptionExists(ApiInputOptions::KEEP_OUTPUT_FILES);
    dumpBaselines_ = input.cmdOptionExists(ApiInputOptions::DUMP_BASELINES);
}

ApiEnvironment&
ApiEnvironment::instantiateFromCLI(int argc, char** argv)
{
    if (instance_) {
        instance_->parse(argc, argv);
        return *instance_;
    }
    instance_ = std::shared_ptr<ApiEnvironment>(new ApiEnvironment(argc, argv));
    return *instance_;
}

ApiEnvironment&
ApiEnvironment::instance()
{
    if (instance_) {
        return *instance_;
    }
    instance_ = std::shared_ptr<ApiEnvironment>(new ApiEnvironment());
    return *instance_;
}

bool
ApiEnvironment::keepOutputFiles() const
{
    return keepOutputFiles_;
}

bool
ApiEnvironment::dumpBaselines() const
{
    return dumpBaselines_;
}

} // namespace segysdk::api::tests

