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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReader.h"
#include "../FileIO/SDConnection.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <SDAPI/SDGenericDataset.h>
#pragma warning(pop)

#include <mutex>

namespace segysdk::fileio
{

struct SEGYSDK_DLL_PUBLIC SDFileReaderParams : public FileAccessParams
{
    SDFileReaderParams& withConnection(const SDConnection& value);
    SDConnection connection_{};
};

class SEGYSDK_DLL_PUBLIC SDFileReader : public FileReader
{
private:
    /**
     * Keep a copy of an owning pointer to make sure the SDManager instance used in dataSet_ is not destructed while this object is in use.
     */
    SDConnection sdConnection_;
    seismicdrive::SDGenericDataset dataSet_;
    std::recursive_mutex mutex_{};
    uint64_t fileSize_{};
    uint64_t blockSize_{};
    bool hasVariableBlockSize_{};
    bool isOpen_{};
    void closeDataset();
    ByteBuffer doRead(const FileReadParams& params) override;

public:
    explicit SDFileReader(const SDFileReaderParams& params);
    ~SDFileReader() override;

    uint64_t getFileSize() override;
    void close() override;

    std::string getSDLegalTag() { return dataSet_.getLegalTag(); }
};

} // namespace segysdk::fileio
