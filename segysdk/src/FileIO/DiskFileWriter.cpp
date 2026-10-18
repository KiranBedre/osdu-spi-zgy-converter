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

#include "../FileIO/DiskFileWriter.h"
#include "../FileIO/FileIOException.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

namespace segysdk::fileio
{

DiskFileWriter::DiskFileWriter(const std::string& fileName, WriteMode mode)
  : FileWriter(fileName)
{
    open(fileName, mode);
}

DiskFileWriter::~DiskFileWriter()
{
    std::lock_guard lock(mutex_);
    file_.close();
}

uint64_t
DiskFileWriter::getFileSize()
{
    std::lock_guard lock(mutex_);
    file_.seekg(0, std::ios::end);
    return file_.tellg();
}

bool
DiskFileWriter::write(const uint8_t* buffer, size_t bufferSize)
{
    std::lock_guard lock(mutex_);

    // TODO: throw some exceptions, failure here is probably fatal
    if (file_.good()) {
        file_.write(reinterpret_cast<const char*>(buffer), bufferSize);
        if (file_.fail())
            throw FileIOException(errno, std::system_category(), "DiskFileWriter::write: Error writing to - " + getFileName());
    }

    return file_.good();
}

void
DiskFileWriter::close()
{
    std::lock_guard lock(mutex_);
    file_.close();
    fileName_ = "";
}

void
DiskFileWriter::open(const std::string& fileName, WriteMode mode)
{

    switch (mode) {
    case WriteMode::Create:
        if (std::filesystem::exists(fileName))
            throw FileIOException("DiskFileWriter::open: File already exists - " + fileName);

        file_.open(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
        break;

    case WriteMode::Truncate:
        file_.open(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
        break;

    case WriteMode::Append:
        file_.open(fileName, std::ios::binary | std::ios::out | std::ios::ate);
        break;
    default:
        throw SegyInvalidArgumentException("Unknown WriteMode");
    }

    if (!file_.fail()) {
        fileName_ = fileName;
    }
    else {
        throw FileIOException(errno, std::system_category(), "DiskFileWriter::open: Error opening output file - " + fileName);
    }
}

} // namespace segysdk::fileio
