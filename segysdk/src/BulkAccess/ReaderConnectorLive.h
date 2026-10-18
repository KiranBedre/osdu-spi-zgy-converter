/*
Copyright 2024-2025 Schlumberger

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

#include "../BulkAccess/IndexedBulkIndex.h"
#include "../BulkAccess/ReaderConnector.h"
#include "../segysdk/SegySession.h"

#include <memory>

namespace segysdk
{

namespace segyio
{

class SegyFile;

namespace valueconversion
{

class SampleValueConverter;

} // namespace valueconversion

} // namespace segyio

namespace segyindex
{

class SegyIndex;

} // namespace segyindex

} // namespace segysdk

namespace segysdk::bulkaccess::live
{

using SegyFile = std::shared_ptr<segyio::SegyFile>;
using SegyIndex = std::shared_ptr<segyindex::SegyIndex>;

struct SEGYSDK_DLL_PUBLIC ReaderConnectorParams : public segysdk::api::ReaderCreateParams
{
    ReaderConnectorParams& withSegyFile(const SegyFile& value);
    ReaderConnectorParams& withSegyIndex(const SegyIndex& value);
    ReaderConnectorParams& withFileLayout(const api::FileLayout& value);
    SegyFile segyFile_{};
    SegyIndex segyIndex_{};
    api::FileLayout fileLayout_{};
};

class SEGYSDK_DLL_PUBLIC ReaderConnector : public bulkaccess::ReaderConnector
{
public:
    explicit ReaderConnector(const ReaderConnectorParams& params);

private:
    std::size_t doGetSampleSize() const override;
    std::size_t doGetSizeI() const override;
    std::size_t doGetSizeJ() const override;
    std::size_t doGetSizeK() const override;
    bool doFindTrace(const std::size_t i, const std::size_t j) override;
    std::size_t doGetTraceOffset() const override;
    ByteBuffer doReadSamples(const fileio::FileReadParams& params) const override;
    Samples doConvertBytesToSamples(const ByteBuffer& bytes) const override;
    std::shared_ptr<fileio::FileReader> reader_;
    SegyIndex index_;
    IndexedBulkIndex bulkIndex_;
    std::shared_ptr<BulkIndex::TraceIndexer> traceIndexer_;
    std::unique_ptr<segyio::valueconversion::SampleValueConverter> converter_;
    api::FileLayout fileLayout_;
};

} // namespace segysdk::bulkaccess::live
