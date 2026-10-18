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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "BulkIndex.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIndex/SegyIndex.h"

#include <memory>

namespace segysdk::bulkaccess
{
using segyio::FileHeaderDescription;

class SEGYSDK_DLL_PUBLIC IndexedBulkIndex : public BulkIndex
{
private:
    class Cube;
    template<typename READER>
    class Indexer;
    template<typename READER>
    class Indexer3D;
    template<typename READER>
    class Indexer2D;

private:
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_;
    std::shared_ptr<const FileHeaderDescription> headerDescription_;

private:
    template<typename READER>
    std::shared_ptr<BulkIndex::TraceIndexer>
    createTraceIndexerImpl(std::shared_ptr<READER> reader);

public:
    IndexedBulkIndex(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                     std::shared_ptr<const FileHeaderDescription> headerDescription,
                     const segysdk::segyio::ConversionDetails& details);
    virtual ~IndexedBulkIndex();

    std::shared_ptr<BulkIndex::TraceIndexer> createTraceIndexer(std::shared_ptr<fileio::BufferedFileReaderBase> reader) override;
};

} // namespace segysdk::bulkaccess
