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

#include "IndexBuilderDataManager.h"

#include "../Logger/Logger.h"

#include <chrono>
#include <filesystem>
#include <random>

namespace segysdk::segyindex
{

static auto
makeRandomString(const std::size_t length = 16)
{
    auto result = std::string{};
    result.reserve(length);
    std::generate_n(
        std::back_inserter(result), length,
        []() {
            static const auto& alphanumeric = std::string{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
            using namespace std::chrono;
            static auto engine = std::mt19937{
                static_cast<std::mt19937::result_type>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count())};
            return alphanumeric[engine() % alphanumeric.size()];
        });
    return result;
}

IndexBuilderDataManager::IndexBuilderDataManager()
  : tempFileName_{(std::filesystem::temp_directory_path() / makeRandomString()).string()},
    writer_{std::make_shared<IndexBuilderDataWriter>(tempFileName_)},
    writerOK{(writer_ != nullptr) && !tempFileName_.empty()}
{
}

IndexBuilderDataManager::~IndexBuilderDataManager()
{
    if (writer_ != nullptr) {
        CloseWriter();
        writer_ = nullptr;
    }
    if (reader_ != nullptr) {
        CloseReader();
        reader_ = nullptr;
    }

    try {
        if (std::filesystem::exists(tempFileName_) && std::filesystem::is_regular_file(tempFileName_)) {
            std::filesystem::remove(tempFileName_);
        }
    }
    catch (const std::system_error& e) {
        SEGYLOG_REPORT_ERROR("indexBuilderDataManager; system_error: " + std::string(e.what())
                             + "; code: " + std::to_string(e.code().value())
                             + "; category: " + std::string(e.code().category().name())
                             + "; message: " + e.code().message());
    }
    catch (const std::exception& e) {
        SEGYLOG_REPORT_ERROR("indexBuilderDataManager; exception: " + std::string(e.what()));
    }
    catch (...) {
        SEGYLOG_REPORT_ERROR("indexBuilderDataManager; unexpected exception");
    }
}

size_t
IndexBuilderDataManager::GetTraceCount() const
{
    return writer_ != nullptr ? writer_->getTraceCount() : traceCount_;
}

bool
IndexBuilderDataManager::WriteData(IndexBuilderData& data)
{
    if (writer_ == nullptr) {
        return false;
    }
    const bool success = writer_->WriteData(data);
    if (success) {
        traceCount_ = writer_->getTraceCount();
    }
    return success;
}

bool
IndexBuilderDataManager::CloseWriter()
{
    if (writer_ == nullptr)
        return true;

    traceCount_ = writer_->getTraceCount();
    writer_->Close();
    writerOK = std::filesystem::exists(tempFileName_) && std::filesystem::file_size(tempFileName_) >= sizeof(IndexBuilderData);

    writer_ = nullptr;

    return writerOK;
}

bool
IndexBuilderDataManager::AtReaderEnd() const
{
    if (!readerOK)
        return true;
    return reader_->AtEnd();
}

bool
IndexBuilderDataManager::IsReaderReady()
{
    if (!writerOK)
        return false;
    if (!readerOK || reader_ == nullptr) {
        startRead();
    }
    return writerOK && readerOK;
}

void
IndexBuilderDataManager::startRead()
{
    if (reader_ != nullptr) {
        CloseReader();
    }

    if (writerOK) {
        reader_ = std::make_shared<IndexBuilderDataReader>(tempFileName_);
        readerOK = reader_ != nullptr;
    }
}

IndexBuilderData
IndexBuilderDataManager::ReadNext() const
{
    if (writerOK) {
        if (!readerOK || !reader_->ReaderInitialized()) {
            return reader_->ReadTrace(0);
        }
        return reader_->ReadNext();
    }
    return IndexBuilderData();
}

void
IndexBuilderDataManager::CloseReader()
{

    if (reader_ == nullptr)
        return;

    reader_->Close();
    reader_ = nullptr;
}

} // namespace segysdk::segyindex
