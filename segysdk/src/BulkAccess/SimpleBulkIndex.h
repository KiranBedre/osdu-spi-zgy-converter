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

#include "../BulkAccess/BulkIndex.h"

namespace segysdk::bulkaccess
{

class SEGYSDK_DLL_PUBLIC SimpleBulkIndex : public BulkIndex
{
private:
    class Indexer;

private:
    std::shared_ptr<BulkIndex::TraceIndexer> createTraceIndexerImpl();

public:
    SimpleBulkIndex(size_t sizeI, size_t sizeJ, size_t sizeK, SampleFormat sampleFormat);

    std::shared_ptr<BulkIndex::TraceIndexer> createTraceIndexer(std::shared_ptr<fileio::BufferedFileReaderBase> reader) override;
};

} // namespace segysdk::bulkaccess
