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

#include "../FileIO/DiskFileReader.h"
#include "../FileIO/FileIOException.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace segysdk::fileio
{

DiskFileReader::DiskFileReader(const std::string& fileName)
  : FileReader(fileName)
{
    open(fileName);
    obtainFileSize();
}

DiskFileReader::~DiskFileReader()
{
    std::lock_guard lock(mutex_);
    file_.close();
}

uint64_t
DiskFileReader::getFileSize()
{
    return fileSize_;
}

void
DiskFileReader::open(const std::string& fileName)
{
    file_.open(fileName, std::ios::binary | std::ios::in);
    if (file_.fail())
        throw FileIOException(errno, std::generic_category(), "DiskFileReader::open: Error opening file - " + fileName);
}

void
DiskFileReader::obtainFileSize()
{
    file_.seekg(0, std::ios::end);
    fileSize_ = file_.tellg();
    file_.seekg(0, std::ios::beg);
}

ByteBuffer
DiskFileReader::doRead(const FileReadParams& params)
{
    std::lock_guard lock(mutex_);

    if (file_.seekg(params.offset_).fail()) {
        return {};
    }
    auto result = ByteBuffer(params.size_);
    const auto bytesRead = file_.read(reinterpret_cast<char*>(result.data()), result.size()).gcount();
    if (file_.fail()) {
        if (file_.bad()) {
            throw FileIOException("Unable to read");
        }
        file_.clear();
    }
    result.resize(std::min<std::size_t>(params.size_, bytesRead));
    return result;
}

void
DiskFileReader::close()
{
    std::lock_guard lock(mutex_);
    file_.close();
}

} // namespace segysdk::fileio
