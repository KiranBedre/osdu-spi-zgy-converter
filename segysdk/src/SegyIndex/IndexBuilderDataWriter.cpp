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

#include "IndexBuilderDataWriter.h"

#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/WriteMode.h"
#include "../Logger/Logger.h"

#include <cstring>
#include <iostream>
#include <system_error>

namespace segysdk::segyindex
{

IndexBuilderDataWriter::IndexBuilderDataWriter(std::string& tmpFileName)
  : _tmpFileName(tmpFileName)

{
    const fileio::FileReference fr(_tmpFileName, fileio::FileReaderOptions::localOnly());
    writer_ = fileio::GenericFileManager::createWriter(fr, fileio::WriteMode::Create);
}

bool
IndexBuilderDataWriter::WriteData(IndexBuilderData& data)
{
    bool success = true;

    const size_t pos = (writePtr_ % maxBufferedTraceCount) * sizeof(IndexBuilderData);
    const auto* my_s_bytes = static_cast<char*>(static_cast<void*>(&data));
    auto* dest = &buffer[pos];
    memcpy(dest, my_s_bytes, sizeof(IndexBuilderData));

    writePtr_++;
    if (writePtr_ % maxBufferedTraceCount == 0) {
        try {
            success = writer_->write(buffer, maxBufferedTraceCount * sizeof(IndexBuilderData));
            return success;
        }
        catch (const std::system_error& e) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; writeData; system_error: " + std::string(e.what())
                                 + "; code: " + std::to_string(e.code().value())
                                 + "; category: " + std::string(e.code().category().name())
                                 + "; message: " + e.code().message());
        }
        catch (const std::exception& e) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; writeData; exception: " + std::string(e.what()));
        }
        catch (...) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; writeData; unexpected exception");
        }
    }

    return false;
}

void
IndexBuilderDataWriter::Close() const
{
    bool success = true;
    if (writePtr_ % maxBufferedTraceCount != 0) {
        try {
            const auto count = writePtr_ % maxBufferedTraceCount;
            success = writer_->write(buffer, count * sizeof(IndexBuilderData));
        }
        catch (const std::system_error& e) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; close; system_error: " + std::string(e.what())
                                 + "; code: " + std::to_string(e.code().value())
                                 + "; category: " + std::string(e.code().category().name())
                                 + "; message: " + e.code().message());
        }
        catch (const std::exception& e) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; close; exception: " + std::string(e.what()));
        }
        catch (...) {
            SEGYLOG_REPORT_ERROR("indexBuilderDataWriter; close; unexpected exception");
        }
    }
    if (!success) {
        SEGYLOG_INFO("Update of output temp file before close did not succeed.");
    }
    writer_->close();
}
} // namespace segysdk::segyindex
