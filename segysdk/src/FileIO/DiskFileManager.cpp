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

#include "../FileIO/DiskFileManager.h"
#include "../FileIO/DiskFileReader.h"
#include "../FileIO/DiskFileWriter.h"
#include "../FileIO/FileIOException.h"
#include "../FileIO/FileWriter.h"
#include "../FileIO/PagedFileReader.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace segysdk::fileio
{

DiskFileManager::DiskFileManager()
{
}

bool
DiskFileManager::exists(const std::string& fileName)
{
    return std::filesystem::exists(fileName);
}

size_t
DiskFileManager::getSize(const std::string& fileName)
{
    if (std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName))
        return std::filesystem::file_size(fileName);

    return 0;
}

TextFileReadResult
DiskFileManager::readTextFile(const FileAccessParams& params)
{
    std::ifstream file(params.fileName_, std::ios::binary | std::ios::in);
    if (file.good()) {
        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        auto text = std::string(size, 0);
        file.seekg(0);
        file.read(text.data(), size);

        if (!file.fail())
            return {true, text};
    }

    throw FileIOException(errno, std::system_category(), "DiskFileManager::readTextFile: " + params.fileName_);
}

bool
DiskFileManager::writeTextFile(const std::string& fileName, const std::string& text, bool allowOverwrite)
{
    if (!allowOverwrite && exists(fileName))
        throw FileIOException("DiskFileManager::writeTextFile: Create failed, file already exists - " + fileName);

    std::ofstream file(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
    if (file.good()) {
        file.write(text.data(), text.size());

        if (!file.fail()) {
            file.flush();
            file.close();

            return file.good();
        }
    }

    throw FileIOException(errno, std::system_category(), "DiskFileManager::writeTextFile - " + fileName);
}

std::shared_ptr<FileReader>
DiskFileManager::createReader(const FileAccessParams& params)
{
    return std::make_shared<PagedFileReader>(createRawReader(params));
}

std::shared_ptr<FileWriter>
DiskFileManager::createWriter(const std::string& fileName, WriteMode mode)
{
    if (mode == WriteMode::Create && exists(fileName))
        throw FileIOException("DiskFileManager::createWriter: Create failed, file already exists - " + fileName);

    return std::make_shared<DiskFileWriter>(fileName, mode);
}

std::shared_ptr<FileReader>
DiskFileManager::createRawReader(const FileAccessParams& params)
{
    return std::make_shared<DiskFileReader>(params.fileName_);
}

bool
DiskFileManager::remove(const std::string& fileName)
{
    if (std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName))
        return std::filesystem::remove(fileName);

    return false;
}

} // namespace segysdk::fileio
