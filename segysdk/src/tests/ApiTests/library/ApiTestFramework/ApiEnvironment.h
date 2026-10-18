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

#pragma once

#include <memory>
#include <string>

namespace segysdk::api::tests
{

class ApiInputOptions
{
public:
    inline static const std::string KEEP_OUTPUT_FILES = "-keepOutputFiles";
    inline static const std::string DUMP_BASELINES = "-dumpBaselines";
};

class ApiEnvironment
{
public:
    virtual ~ApiEnvironment() = default;

    ApiEnvironment(const ApiEnvironment&) = delete;
    ApiEnvironment& operator=(const ApiEnvironment&) = delete;

    ApiEnvironment(ApiEnvironment&&) = delete;
    ApiEnvironment& operator=(ApiEnvironment&&) = delete;

    static ApiEnvironment& instantiateFromCLI(int argc, char** argv);
    static ApiEnvironment& instance();

    bool keepOutputFiles() const;
    bool dumpBaselines() const;

protected:
    ApiEnvironment(int argc, char** argv);
    ApiEnvironment() = default;

private:
    void parse(int args, char** argv);

private:
    static std::shared_ptr<ApiEnvironment> instance_;

    bool keepOutputFiles_ = false;
    bool dumpBaselines_ = false;
};

} // namespace segysdk::api::tests