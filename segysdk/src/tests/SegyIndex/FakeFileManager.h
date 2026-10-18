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

#include "FileIO/FileManager.h"

#include <string>

namespace segysdk::fileio
{
class FileReader;
class FileWriter;
enum class WriteMode;
} // namespace segysdk::fileio

namespace segysdk::segyindex::tests
{

class FakeFileManager : public fileio::FileManager
{
public:
    bool exists(const std::string&) override { return true; }
    size_t getSize(const std::string&) override { return 0; }
    fileio::TextFileReadResult readTextFile(const fileio::FileAccessParams&) override { return {}; }
    bool writeTextFile(const std::string&, const std::string&, bool) override { return false; }
    std::shared_ptr<fileio::FileReader> createReader(const fileio::FileAccessParams&) override { return nullptr; }
    std::shared_ptr<fileio::FileWriter> createWriter(const std::string&, fileio::WriteMode) override { return nullptr; }
    std::shared_ptr<fileio::FileReader> createRawReader(const fileio::FileAccessParams&) override { return nullptr; }
    bool remove(const std::string&) override { return false; }
};

} // namespace segysdk::segyindex::tests
