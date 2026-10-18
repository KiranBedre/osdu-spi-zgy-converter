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

#include "../BulkAccess/ReaderConnectorLive.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"

namespace segysdk::bulkaccess::live
{

bulkaccess::live::ReaderConnector::ReaderConnector(const ReaderConnectorParams& params)
  : reader_(params.segyFile_->getCloneFile(params, true)),
    index_(params.segyIndex_),
    bulkIndex_(index_, params.segyFile_->getFileHeaderDescription(), params.segyFile_->getDerivedHeader()->getConversionDetails()),
    traceIndexer_(bulkIndex_.createTraceIndexer(std::make_shared<fileio::BufferedFileReader>(params.segyFile_->getCloneFile()))),
    converter_(segyio::valueconversion::SampleValueConverter::create(bulkIndex_.getSampleFormat(), bulkIndex_.isByteSwapped())),
    fileLayout_(params.fileLayout_)
{
}

std::size_t
ReaderConnector::doGetSizeI() const
{
    return fileLayout_.sizeI;
}

std::size_t
ReaderConnector::doGetSizeJ() const
{
    return fileLayout_.sizeJ;
}

std::size_t
ReaderConnector::doGetSizeK() const
{
    return fileLayout_.sizeK;
}

bool
ReaderConnector::doFindTrace(const std::size_t i, const std::size_t j)
{
    return index_->isSegy3D() ? traceIndexer_->findTrace(i, j) : traceIndexer_->findTraceByTraceNo(i);
}

std::size_t
ReaderConnector::doGetTraceOffset() const
{
    return traceIndexer_->getOffset();
}

ByteBuffer
ReaderConnector::doReadSamples(const fileio::FileReadParams& params) const
{
    return reader_->read(params);
}

Samples
ReaderConnector::doConvertBytesToSamples(const ByteBuffer& bytes) const
{
    auto result = Samples(bytes.size() / getSampleSize());
    if (bulkIndex_.hasTraceWeights()) {
        converter_->convertScaled(bytes.data(), result.data(), result.size(), traceIndexer_->getTraceWeight());
    }
    else {
        converter_->convert(bytes.data(), result.data(), result.size());
    }
    return result;
}

std::size_t
ReaderConnector::doGetSampleSize() const
{
    const auto result = segyio::Primitives::getSampleSize(bulkIndex_.getSampleFormat());
    if (result == 0) {
        throw segyio::SegyFormatException("Invalid sample size");
    }
    return result;
}

ReaderConnectorParams&
ReaderConnectorParams::withSegyFile(const SegyFile& value)
{
    segyFile_ = value;
    return *this;
}

ReaderConnectorParams&
ReaderConnectorParams::withSegyIndex(const SegyIndex& value)
{
    segyIndex_ = value;
    return *this;
}

ReaderConnectorParams&
ReaderConnectorParams::withFileLayout(const api::FileLayout& value)
{
    fileLayout_ = value;
    return *this;
}

} // namespace segysdk::bulkaccess::live
