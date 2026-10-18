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

#include "AutoCleanupTestFile.h"

#include "TestBackdoor.h"

#include "../Utilities/Environment.h"
#include "../segysdk/SegySessionFactory.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace segysdk;

static std::random_device rnd_dev;
static std::default_random_engine rnd(rnd_dev());
static std::uniform_int_distribution ud;

static std::string
testdata_file_copy(const std::string& file_name, const std::string& random_id)
{
    auto const testdata_dir = utilities::Environment::instance().SegyLocalTestDataPath();
    if (testdata_dir.empty()) {
        std::cerr << "Missing environment variable " << utilities::EnvironmentVariableStringConsts::SEGY_LOCALTESTDATA_PATH << std::endl;
        exit(1);
    }

    auto source_path = std::filesystem::path(testdata_dir).append(file_name);
    auto temp_path = (std::filesystem::temp_directory_path().append("_tmp_").concat(random_id)).concat(source_path.filename().string());

    std::ifstream in_file(source_path, std::ios::binary);
    std::ofstream temp_file(temp_path, std::ios::binary);
    temp_file << in_file.rdbuf();
    in_file.close();
    temp_file.close();

    return temp_path.string();
}

AutoCleanupTestFile::AutoCleanupTestFile(const std::string& source_file, std::string segy_config_json_string)
  : random_id_(std::to_string(ud(rnd))),
    path_(testdata_file_copy(source_file, random_id_)),
    segy_config_json_string_(std::move(segy_config_json_string))
{
    testdata_file_copy(source_file + ".params.json", random_id_);
}

AutoCleanupTestFile::~AutoCleanupTestFile()
{
    TestBackdoor::getImpl(createSession()->getIndex())->deleteIndex();
}

void
AutoCleanupTestFile::create_index(bool is2D) const
{
    auto session = createSession();
    session->setSegy2DFlag(is2D);
    session->generateIndexFile();
}

std::unique_ptr<api::SegySession>
AutoCleanupTestFile::createSession() const
{
    const api::SegySessionFactory segySessionFactory({path_}, segy_config_json_string_);
    return segySessionFactory.CreateSegySession();
}
