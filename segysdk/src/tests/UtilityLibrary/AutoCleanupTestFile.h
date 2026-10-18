/*
Copyright 2021 Schlumberger

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

#include "../segysdk/SegySession.h"

#include <memory>
#include <string>

static constexpr auto empty_segy_config_json_string = "{}";

class AutoCleanupTestFile
{
public:
    explicit AutoCleanupTestFile(const std::string& source_file, std::string segy_config_json_string = empty_segy_config_json_string);
    AutoCleanupTestFile(const AutoCleanupTestFile&) = delete;
    AutoCleanupTestFile& operator=(const AutoCleanupTestFile&) = delete;
    ~AutoCleanupTestFile();

    std::string get_path() const { return path_; }
    std::string get_segy_config_json_string() const { return segy_config_json_string_; }

    void create_index(bool is2D = false) const;

private:
    std::string random_id_;
    std::string path_;
    std::string segy_config_json_string_;

    std::unique_ptr<segysdk::api::SegySession> createSession() const;
};
